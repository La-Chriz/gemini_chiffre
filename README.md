# GEMINI Block Cipher 🔐

![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)
![C Standard](https://img.shields.io/badge/Language-C99-orange.svg)

98 % der Zeit lautet die goldene Regel der Softwareentwicklung: *Schreibe niemals deine eigene Krypto, nutze etablierte Bibliotheken.* Aber manchmal muss man sich bewusst für die restlichen 2 % entscheiden – den Weg des Ausprobierens, des Scheiterns und des tiefen Verständnisses, um herauszufinden, wie das Chaos auf Bitebene wirklich funktioniert. 

Dieses Projekt ist genau das: Eine von Grund auf selbst entwickelte, experimentelle 128-Bit-Blockchiffre, die als Lern- und Forschungsprojekt entstanden ist.

## 🏗 Architektur & Design

GEMINI ist ein **symmetrisches ARX-Feistel-Netzwerk** (Addition, Rotation, XOR) mit einer Block- und Schlüssellänge von jeweils 128 Bit. 

* **Cross-Mixed Channels:** Im Gegensatz zu einfachen Feistel-Netzwerken verschmilzt GEMINI die Datenhälften vor der Substitution (`r_mix`), was Isolationsschwachstellen eliminiert und eine rasante Diffusion erzwingt.
* **Optimierter Key-Schedule:** Generiert aus dem 16-Byte-Masterkey asymmetrische Rundenschlüssel unter Einbezug des Goldenen Schnitts, um lineare Muster zu zerstören.
* **Single-Header Library:** Das gesamte Projekt ist in einer einzigen Datei (`gemini_chiffre.h`) gekapselt. Keine komplizierten Makefiles, keine externen Abhängigkeiten.

## 📊 Kryptoanalyse (Avalanche-Effekt)

Die Chiffre wurde intensiv auf den **Avalanche-Effekt (Lawineneffekt)** getestet. Ein starker Algorithmus erfordert, dass das Kippen eines einzigen Bits im Klartext dazu führt, dass sich jedes Bit im Geheimtext mit einer Wahrscheinlichkeit von 50 % ändert (Strict Avalanche Criterion / SAC).

GEMINI durchbricht die lineare Isolation extrem früh und erreicht die vollständige kryptografische Sättigung bereits ab **Runde 5**:

| Runden | ø Verfärbte Bits (von 128) | Prozentualer Effekt |
| :---: | :---: | :---: |
| 1 | 5.75 | 4.49% |
| 2 | 22.39 | 17.49% |
| 3 | 44.37 | 34.66% |
| 4 | 59.03 | 46.12% |
| **5** | **63.78** | **49.83%** |
| ... | ... | ... |
| 16 | 63.39 | 49.52% |

*Empfohlene Rundenanzahl für eine solide Sicherheitsmarge: **20 Runden**.*

## 🚀 Integration (Quick Start)

Da GEMINI als STB-Style Single-Header-Library geschrieben ist, reicht es aus, die Datei in dein Projekt zu kopieren. 

In **genau einer** deiner `.c`-Dateien musst du die Implementierung definieren:

```c
#define GEMINI_CHIFFRE_IMPLEMENTATION
#include "gemini_chiffre.h"
```


P.S. 

Sowie die Chiffre als auch diese Datei wurden teilweise durch eine KI (Google Gemini (PRO)) generiert.
Dieses Projekt hat ungefähr 4 Stunden meiner Lebenszeit gekostet und diente ausschließlich meiner Unterhaltung und der Befriedigung meiner Wissbegier.
