/*
 * ==================================================================
 * GEMINI Block Cipher
 * Copyright (C) 2026 Lasse Christens
 * ==================================================================
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * ===================================================================
 * AUTHOR'S STATEMENT ON ETHICAL USE
 * ===================================================================
 * Dieses Projekt ist ein rein friedliches Lehr- und Forschungsprojekt. 
 * Als Autor lehne ich jede Form von militärischer Nutzung oder die Verwendung 
 * in Rüstungsanwendungen zutiefst ab. Ich appelliere an die Ethik jedes 
 * Entwicklers und Nutzers, diesen Wunsch zu respektieren.
 *
 * ===================================================================
 * SECURITY DISCLAIMER
 * ===================================================================
 * Dies ist eine experimentelle kryptografische Implementierung für 
 * Bildungszwecke. Sie wurde keinen formalen Sicherheitsaudits unterzogen.
 * DO NOT USE THIS FOR PRODUCTION OR SENSITIVE DATA!
 * ===================================================================
 */

#ifndef GEMINI_CHIFFRE_H
#define GEMINI_CHIFFRE_H

#include <stdint.h>
#include <stddef.h>

#ifndef GEMINI_API
#define GEMINI_API extern
#endif

#define GEMINI_BLOCK_SIZE 128
#define GEMINI_BLOCK_LENGTH 16
#define GEMINI_KEY_SIZE 128
#define GEMINI_KEY_LENGTH 16
#define GEMINI_MAX_ROUNDS 255

#define GOLDEN_RATIO_1 0x9e3779b9
#define GOLDEN_RATIO_2 0x7F4A7C15

typedef enum {
    GEMINI_NO_ERROR,
    GEMINI_PARAMETER_ERROR,
    GEMINI_KEY_SIZE_ERROR,
    GEMINI_ROUND_ERROR
} gemini_error_t;

typedef struct {
    size_t rounds; 
    uint32_t k[GEMINI_MAX_ROUNDS][4];
} gemini_context_t;

GEMINI_API gemini_error_t gemini_init(gemini_context_t *ctx, const uint8_t *key, size_t keylen, size_t rounds);

GEMINI_API gemini_error_t gemini_encrypt(const gemini_context_t *ctx, const uint8_t plaintext[16], uint8_t ciphertext[16]);

GEMINI_API gemini_error_t gemini_decrypt(const gemini_context_t *ctx, const uint8_t ciphertext[16], uint8_t plaintext[16]);

#endif /* GEMINI_CHIFFRE_H */

#ifdef GEMINI_CHIFFRE_IMPLEMENTATION
#ifndef GEMINI_CHIFFRE_IMPLEMENTED
#define GEMINI_CHIFFRE_IMPLEMENTED

static const uint8_t sbox[256] = {
  0xE4, 0xD3, 0x96, 0x61, 0x77, 0x82, 0x13, 0x5F, 
  0x7D, 0x2B, 0x75, 0xDB, 0x80, 0x53, 0x67, 0x5A, 
  0x8B, 0xB4, 0xA8, 0x1F, 0x11, 0xF7, 0x0C, 0x98, 
  0x19, 0x70, 0xCD, 0xF6, 0x4F, 0xB3, 0xCB, 0x02, 
  0x73, 0xA3, 0x9B, 0xE0, 0x03, 0xA9, 0x2A, 0xF5, 
  0xEC, 0x7F, 0x26, 0x54, 0xE8, 0x87, 0xB1, 0x59, 
  0x76, 0xB0, 0xBD, 0x21, 0x62, 0xC6, 0xBA, 0x31, 
  0x55, 0xEB, 0x5B, 0x94, 0x3E, 0x2C, 0x2E, 0x79, 
  0x4D, 0x78, 0x69, 0x8D, 0x41, 0xDF, 0xD4, 0x22, 
  0xB2, 0x38, 0x9F, 0xE2, 0x34, 0x58, 0x4A, 0xCA, 
  0x7E, 0x2D, 0xE7, 0x40, 0x9E, 0x6F, 0x0D, 0x18, 
  0x4E, 0x47, 0xD9, 0xD7, 0x37, 0x49, 0x3B, 0xD6, 
  0xE1, 0xAF, 0xAB, 0xDD, 0x9D, 0x85, 0x5E, 0x06, 
  0xE3, 0x32, 0x08, 0xF0, 0x5D, 0x8A, 0x6C, 0x44, 
  0x91, 0x01, 0xBE, 0x8F, 0x57, 0x83, 0x1B, 0xC5, 
  0x36, 0xBF, 0x68, 0x52, 0x04, 0x4C, 0xBB, 0x0E, 
  0x39, 0x10, 0x27, 0x8C, 0x89, 0x0A, 0x2F, 0xB6, 
  0x93, 0x90, 0x3C, 0x46, 0x56, 0x66, 0xAE, 0x07, 
  0xC7, 0x97, 0x6A, 0x16, 0x71, 0x17, 0xB8, 0x3D, 
  0xC0, 0x63, 0xA7, 0x95, 0xF9, 0x45, 0xA2, 0xA4, 
  0x7B, 0x92, 0xF4, 0x6E, 0x14, 0x99, 0x0F, 0xAA, 
  0xED, 0xC2, 0xD5, 0x28, 0x74, 0xE5, 0xEA, 0x8E, 
  0xC9, 0x64, 0xCF, 0xFC, 0x60, 0xBC, 0xA6, 0x88, 
  0x30, 0x23, 0x09, 0xF2, 0x9A, 0xAD, 0x3A, 0x81, 
  0x48, 0x42, 0xC3, 0x86, 0xF3, 0x5C, 0x50, 0x7C, 
  0x1D, 0xDA, 0xB5, 0xD0, 0xFB, 0xFA, 0x1E, 0xD2, 
  0x24, 0x65, 0x9C, 0xB9, 0xD8, 0xC4, 0xE6, 0xFF, 
  0x0B, 0x20, 0xEF, 0x6B, 0x25, 0xCE, 0x3F, 0xA0, 
  0xB7, 0xFE, 0xDE, 0xC8, 0x6D, 0x4B, 0x43, 0x29, 
  0xFD, 0x1C, 0xE9, 0xD1, 0x1A, 0x00, 0xF8, 0x35, 
  0xAC, 0xEE, 0xF1, 0xDC, 0x15, 0x05, 0x51, 0xA1, 
  0x72, 0x12, 0xA5, 0x33, 0xC1, 0xCC, 0x84, 0x7A};

static inline uint32_t rotl32(uint32_t x, uint32_t n)
{
    return (x << (n & 31)) | (x >> ((32 - (n & 31)) & 31));
}

static inline uint32_t sbox32(uint32_t x)
{
    return (((uint32_t)sbox[(x >> 24) & 0xff]) << 24) |
           (((uint32_t)sbox[(x >> 16) & 0xff]) << 16) |
           (((uint32_t)sbox[(x >>  8) & 0xff]) <<  8) |
            ((uint32_t)sbox[x & 0xff]);
}

static inline uint32_t load(const uint8_t *p)
{
    return ((uint32_t)p[3] << 24) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[1] <<  8) |
           ((uint32_t)p[0]);
}

static inline void store(uint8_t *p, uint32_t x)
{
    p[0] = (uint8_t)(x & 0xff);
    p[1] = (uint8_t)((x >> 8) & 0xff);
    p[2] = (uint8_t)((x >> 16) & 0xff);
    p[3] = (uint8_t)((x >> 24) & 0xff);
}

GEMINI_API gemini_error_t gemini_init(gemini_context_t *ctx, const uint8_t *key, size_t keylen, size_t rounds)
{
    if(!ctx || !key) {
        return GEMINI_PARAMETER_ERROR;
    }

    if(keylen != GEMINI_KEY_LENGTH) {
        return GEMINI_KEY_SIZE_ERROR;
    }

    if (rounds == 0 || rounds > GEMINI_MAX_ROUNDS) {
        return GEMINI_ROUND_ERROR; 
    }

    ctx->rounds = rounds;
    ctx->k[0][0] = load(&key[0]);
    ctx->k[0][1] = load(&key[4]);
    ctx->k[0][2] = load(&key[8]);
    ctx->k[0][3] = load(&key[12]);

    uint32_t rc = GOLDEN_RATIO_1;

    for(size_t i = 1; i < rounds; i++)
    {
        rc = (rotl32(rc, 13) ^ (i * GOLDEN_RATIO_1)) ^ GOLDEN_RATIO_2;

        uint32_t s_in = ctx->k[i-1][0];
        s_in = rotl32(s_in, 7)  + ctx->k[i-1][1];
        s_in = rotl32(s_in, 13) ^ ctx->k[i-1][2];
        s_in = rotl32(s_in, 21) + ctx->k[i-1][3];

        uint32_t W = sbox32(s_in ^ rc);
        W = rotl32(W, 9)  + W;
        W = rotl32(W, 13) ^ W;
        W = rotl32(W, 22) + W;
        
        ctx->k[i][0] = ctx->k[i-1][0] ^ W;
        ctx->k[i][1] = ctx->k[i-1][1] + rotl32(W, 7);
        ctx->k[i][2] = ctx->k[i-1][2] ^ rotl32(W, 15);
        ctx->k[i][3] = ctx->k[i-1][3] + rotl32(W, 23);
    }

    return GEMINI_NO_ERROR;
}

GEMINI_API gemini_error_t gemini_encrypt(const gemini_context_t *ctx, const uint8_t plaintext[16], uint8_t ciphertext[16])
{
    if(!ctx || !plaintext || !ciphertext) {
        return GEMINI_PARAMETER_ERROR;
    }

    uint32_t s[4];
    s[0] = load(&plaintext[0]);  
    s[1] = load(&plaintext[4]);
    s[2] = load(&plaintext[8]);  
    s[3] = load(&plaintext[12]);

    for (size_t r = 0; r < ctx->rounds; r++) {
        // NEU: Kreuz-Mischung der rechten Haelfte ueber
        // ein ARX-Glied.
        // Ein Bit-Kipp in s[2] ODER s[3] betrifft nun
        // sofort BEIDE F-Funktionen.
        uint32_t r_mix = s[2] + rotl32(s[3], 15);

        uint32_t f0 = sbox32(r_mix ^ ctx->k[r][0]) + ctx->k[r][1];
        uint32_t f1 = sbox32(rotl32(r_mix, 11) ^ ctx->k[r][2]) ^ ctx->k[r][3];
        f0 = rotl32(f0, 7); 
        f1 = rotl32(f1, 13);
        
        uint32_t next_s0 = s[2];
        uint32_t next_s1 = s[3];
        uint32_t next_s2 = s[0] ^ f0;
        uint32_t next_s3 = s[1] ^ f1;

        s[0] = next_s0;
        s[1] = next_s1;
        s[2] = next_s2;
        s[3] = next_s3;
    }

    store(&ciphertext[0],  s[0]);
    store(&ciphertext[4],  s[1]);
    store(&ciphertext[8],  s[2]);
    store(&ciphertext[12], s[3]);

    return GEMINI_NO_ERROR;
}

GEMINI_API gemini_error_t gemini_decrypt(const gemini_context_t *ctx, const uint8_t ciphertext[16], uint8_t plaintext[16])
{
    if(!ctx || !ciphertext || !plaintext) {
        return GEMINI_PARAMETER_ERROR;
    }

    uint32_t s[4];
    s[0] = load(&ciphertext[0]);
    s[1] = load(&ciphertext[4]);
    s[2] = load(&ciphertext[8]);
    s[3] = load(&ciphertext[12]);

    for (ptrdiff_t r = (ptrdiff_t)ctx->rounds - 1; r >= 0; r--) {
        // Da die Haelften bei Feistel jede Runde
        // tauschen, liegen die urspruenglich 
        // rechten Woerter waehrend der Entschluesselung
        // in s[0] und s[1].
        uint32_t r_mix = s[0] + rotl32(s[1], 15);

        uint32_t f0 = sbox32(r_mix ^ ctx->k[r][0]) + ctx->k[r][1];
        uint32_t f1 = sbox32(rotl32(r_mix, 11) ^ ctx->k[r][2]) ^ ctx->k[r][3];
        f0 = rotl32(f0, 7); 
        f1 = rotl32(f1, 13);

        uint32_t orig_s0 = s[2] ^ f0;
        uint32_t orig_s1 = s[3] ^ f1;
        uint32_t orig_s2 = s[0];
        uint32_t orig_s3 = s[1];

        s[0] = orig_s0;
        s[1] = orig_s1;
        s[2] = orig_s2;
        s[3] = orig_s3;
    }

    store(&plaintext[0],  s[0]);
    store(&plaintext[4],  s[1]);
    store(&plaintext[8],  s[2]);
    store(&plaintext[12], s[3]);

    return GEMINI_NO_ERROR;
}

#endif /* GEMINI_CHIFFRE_IMPLEMENTED */
#endif /* GEMINI_CHIFFRE_IMPLEMENTATION */

