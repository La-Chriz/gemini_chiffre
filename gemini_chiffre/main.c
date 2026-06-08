#define GEMINI_CHIFFRE_IMPLEMENTATION
#include "gemini_chiffre.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

static inline size_t popcount32(uint32_t x) {
    size_t count = 0;
    while (x) {
        count += (x & 1);
        x >>= 1;
    }
    return count;
}

int key_schedule_avalanche_test(void)
{
    size_t test_rounds = 16;
    uint8_t base_key[16] = {0}; 
    gemini_context_t base_ctx;

    if (gemini_init(&base_ctx, base_key, sizeof(base_key), test_rounds)) {
        return 1;
    }

    size_t total_diff_per_round[16] = {0};

    for (size_t bit = 0; bit < 128; bit++) {
        uint8_t modified_key[16] = {0};
        modified_key[bit / 8] |= (1 << (bit % 8));

        gemini_context_t mod_ctx;
        gemini_init(&mod_ctx, modified_key, sizeof(modified_key), test_rounds);

        for (size_t r = 0; r < test_rounds; r++) {
            size_t round_bits_flipped = 0;
            round_bits_flipped += popcount32(base_ctx.k[r][0] ^ mod_ctx.k[r][0]);
            round_bits_flipped += popcount32(base_ctx.k[r][1] ^ mod_ctx.k[r][1]);
            round_bits_flipped += popcount32(base_ctx.k[r][2] ^ mod_ctx.k[r][2]);
            round_bits_flipped += popcount32(base_ctx.k[r][3] ^ mod_ctx.k[r][3]);
            
            total_diff_per_round[r] += round_bits_flipped;
        }
    }

    printf("=== ANALYSE DES AVALANCHE-EFFEKTS (OPTIMIERT) ===\n");
    printf("Gemessen ueber alle 128 moeglichen 1-Bit-Flips im Master-Key.\n");
    printf("Zielwert fuer statistische Ununterscheidbarkeit: 50.00%%\n\n");
    printf("Runde | ø Verfaerbte Bits | Prozentualer Effekt\n");
    printf("------------------------------------------------\n");

    for (size_t r = 0; r < test_rounds; r++) {
        double average_flipped = (double)total_diff_per_round[r] / 128.0;
        double percentage = (average_flipped / 128.0) * 100.0;
        
        printf("  %2zu  |      %6.2f       |       %5.2f%%\n", r, average_flipped, percentage);
    }
    printf("------------------------------------------------\n");

    return 0;
}

int chiffre_avalanche_test(void)
{
    uint8_t key[16] = {0}; 
    
    printf("=== ANALYSE DES KLARTEXT-AVALANCHE-EFFEKTS (CROSS-MIXED) ===\n");
    printf("Gemessen ueber alle 128 moeglichen 1-Bit-Flips im Klartext.\n");
    printf("Zielwert fuer statistische Sicherheit: 50.00%%\n\n");
    printf("Runden | ø Verfaerbte Bits | Prozentualer Effekt\n");
    printf("-------------------------------------------------\n");

    for (size_t r = 1; r <= 16; r++) {
        gemini_context_t ctx;
        if (gemini_init(&ctx, key, sizeof(key), r)) {
            printf("Fehler bei der Key-Schedule Initialisierung!\n");
            return 1;
        }

        uint8_t base_pt[16] = {0};
        uint8_t base_ct[16] = {0};
        gemini_encrypt(&ctx, base_pt, base_ct);

        size_t total_diff = 0;

        for (size_t bit = 0; bit < 128; bit++) {
            uint8_t mod_pt[16] = {0};
            mod_pt[bit / 8] |= (1 << (bit % 8)); 

            uint8_t mod_ct[16] = {0};
            gemini_encrypt(&ctx, mod_pt, mod_ct);

            size_t diff = 0;
            diff += popcount32(load(&base_ct[0])  ^ load(&mod_ct[0]));
            diff += popcount32(load(&base_ct[4])  ^ load(&mod_ct[4]));
            diff += popcount32(load(&base_ct[8])  ^ load(&mod_ct[8]));
            diff += popcount32(load(&base_ct[12]) ^ load(&mod_ct[12]));

            total_diff += diff;
        }

        double average_flipped = (double)total_diff / 128.0;
        double percentage = (average_flipped / 128.0) * 100.0;
        
        printf("  %2zu   |      %6.2f       |       %5.2f%%\n", r, average_flipped, percentage);
    }
    printf("-------------------------------------------------\n");

    return 0;
}

int chiffre_test(void)
{
    gemini_context_t ctx;
    
    // Ein solider 128-Bit Test-Schluessel
    uint8_t key[16] = {
        0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
        0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
    };
    
    // Unser 16-Byte Datenblock (mit Leerzeichen auf 16 Byte aufgefuellt)
    uint8_t plaintext[16] = "Hallo Krypto!  "; 
    uint8_t ciphertext[16] = {0};
    uint8_t decrypted[16]  = {0};

    // Wir rufen 20 Runden auf – der empfohlene Sicherheitsstandard
    if (gemini_init(&ctx, key, sizeof(key), 20)) {
        printf("Fehler bei der Key-Schedule Initialisierung!\n");
        return 1;
    }

    // Die kryptografische Transformation
    gemini_encrypt(&ctx, plaintext, ciphertext);
    gemini_decrypt(&ctx, ciphertext, decrypted);

    // --- AUSGABE UND VERIFIKATION ---
    printf("=== ROUND-TRIP VERIFIKATION (16 RUNDEN) ===\n\n");
    
    printf("[+] Originaler Klartext : '%.16s'\n", plaintext);
    
    printf("[+] Geheimtext (Hex)    : ");
    for(size_t i = 0; i < 16; i++) {
        printf("%02X", ciphertext[i]);
    }
    printf("\n");
    
    printf("[+] Entschluesselt      : '%.16s'\n", decrypted);
    
    // Mathematische Gegenprobe
    bool success = true;
    for(size_t i = 0; i < 16; i++) {
        if(plaintext[i] != decrypted[i]) {
            success = false;
            break;
        }
    }
    
    printf("\n[STATUS] ");
    if(success) {
        printf("ERFOLGREICH. Die Mathematik geht perfekt auf.\n");
    } else {
        printf("FEHLSCHLAG. Anomalie in den Kanaelen entdeckt.\n");
    }
    printf("===========================================\n");

    return 0;
}

int main(void)
{
  key_schedule_avalanche_test();
  printf("\n\n");
  chiffre_avalanche_test();
  printf("\n\n");
  chiffre_test();

  return 0;
}
