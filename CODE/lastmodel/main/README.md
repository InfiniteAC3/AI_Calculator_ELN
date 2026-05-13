# SANDERS IA — Calculatrice Scientifique v2.0
## Guide complet : câblage, installation et utilisation

---

## MATÉRIEL REQUIS
- Arduino Mega 2560
- 2× LCD 16×2 avec module I2C
- 2× Clavier matriciel 4×4
- Câbles de connexion

---

## CÂBLAGE I2C (LCD)
| LCD | Adresse | SDA | SCL | VCC | GND |
|-----|---------|-----|-----|-----|-----|
| LCD1 (principal)   | 0x27 | Pin 20 | Pin 21 | 5V | GND |
| LCD2 (secondaire)  | 0x21 | Pin 20 | Pin 21 | 5V | GND |

> Les deux LCD partagent le bus I2C (même SDA/SCL). Vérifier que les cavaliers
> d'adresse sont bien configurés sur les modules I2C.

---

## CÂBLAGE CLAVIER 1 (CALC)
| Broche clavier | Pin Arduino |
|----------------|-------------|
| Ligne A (AA)   | 22 |
| Ligne B (BB)   | 23 |
| Ligne C (CC)   | 24 |
| Ligne D (DD)   | 25 |
| Col 1 (11)     | 26 |
| Col 2 (22)     | 27 |
| Col 3 (33)     | 28 |
| Col 4 (44)     | 29 |

## CÂBLAGE CLAVIER 2 (SCI)
| Broche clavier | Pin Arduino |
|----------------|-------------|
| Ligne A (SA)   | 30 |
| Ligne B (SB)   | 31 |
| Ligne C (SC)   | 32 |
| Ligne D (SD)   | 33 |
| Col 1 (S1)     | 34 |
| Col 2 (S2)     | 35 |
| Col 3 (S3)     | 36 |
| Col 4 (S4)     | 37 |

---

## LIBRAIRIES À INSTALLER
Dans l'IDE Arduino → Outils → Gérer les bibliothèques :
1. **LiquidCrystal I2C** par Frank de Brabander
2. **Keypad** par Mark Stanley & Alexander Brevig

---

## STRUCTURE DES FICHIERS
```
sanders_calc/
├── main.ino          ← Ouvrir ce fichier dans l'IDE Arduino
├── config.h          ← Constantes, broches, état global
├── display_mgr.h     ← Gestionnaire 2 écrans LCD
├── math_engine.h     ← Parser + fonctions scientifiques
├── mode_comp.h       ← Mode COMP (calcul classique)
├── mode_advanced.h   ← Mode MATRIX + Mode EQUATION
├── mode_ia.h         ← Mode IA (limites, dérivées, intégrales)
└── README.md         ← Ce fichier
```

---

## LAYOUT DES CLAVIERS

### Clavier 1 (CALC)
```
[ 7 ] [ 8 ] [ 9 ] [ ÷ ]
[ 4 ] [ 5 ] [ 6 ] [ × ]
[ 1 ] [ 2 ] [ 3 ] [ - ]
[ON/C] [ 0 ] [ = ] [ + ]
```

### Clavier 2 (SCI)
```
[sqrt] [  ^  ] [  (  ] [  )  ]
[ ln ] [  .  ] [ ANS ] [  ⌫  ]
[hyp ] [ sin ] [ cos ] [ tan ]
[Mode] [shift] [  <  ] [  >  ]
```

---

## FONCTIONS DES TOUCHES SCI (avec SHIFT et HYP)

| Touche | Normal | + Shift | + Hyp | + Shift+Hyp |
|--------|--------|---------|-------|-------------|
| sqrt   | √x     | x²      | —     | —           |
| sin    | sin    | arcsin  | sinh  | asinh       |
| cos    | cos    | arccos  | cosh  | acosh       |
| tan    | tan    | arctan  | tanh  | atanh       |
| ln     | log₁₀  | 10^x    | —     | —           |
| ANS    | ln     | e^x     | —     | —           |
| <      | ⌫ back | —       | —     | —           |
| >      | Clear  | —       | —     | —           |

> **Note sur les touches ln/log** : La touche `ln` du clavier envoie le code 'l'
> (log10 en mode normal, e^x avec Shift). La touche `ANS` envoie 'A' (insère
> la dernière réponse dans l'expression).

---

## MODES DE CALCUL

### [M] pour cycler : COMP → MATRIX → EQUAT → IA

---

### MODE COMP (Calcul standard)
- Saisir l'expression avec les chiffres et opérateurs
- `=` pour calculer
- `ON/C` pour effacer
- Les fonctions scientifiques s'appliquent au dernier nombre
- Exemple : `3` `+` `4` `*` `2` `=` → `11`
- Exemple : `sin` avec `30` dans l'expr → `0.5` (mode DEG)

**Priorité des opérations :** × et ÷ avant + et −, ^ associatif à droite
**Parenthèses :** supportées, imbriquées

---

### MODE MATRIX
Après `[M]` × 1

**Taille :** `[/]` pour basculer 2×2 ↔ 3×3
**Opération :** `[+]` pour cycler ADD → SUB → MUL → DET → INV → TRA

**Saisie :**
1. Entrer chaque élément, confirmer avec `[=]`
2. Pour DET/INV/TRA : une seule matrice A
3. Pour ADD/SUB/MUL : saisir A puis B automatiquement

**Backspace :** touche `<` du clavier SCI

---

### MODE EQUATION
Après `[M]` × 2

`[+]` pour basculer Linéaire ↔ Quadratique

**Linéaire (ax + b = 0) :**
1. Saisir `a`, confirmer `[=]`
2. Saisir `b`, confirmer `[=]`
→ Solution : x = -b/a

**Quadratique (ax² + bx + c = 0) :**
1. Saisir `a`, puis `b`, puis `c`
→ Discriminant D = b² - 4ac
→ Si D > 0 : deux solutions affichées
→ Si D = 0 : solution double
→ Si D < 0 : pas de solution réelle

---

### MODE IA
Après `[M]` × 3

`[S]` pour cycler sous-modes : LIMITE → DERIVEE → EXPLIQ → INTEGR

#### Sous-mode LIMITE
Calcule lim(x→a) f(x) numériquement.
1. Saisir f(x) avec les chiffres et opérateurs
   - Utiliser `[ANS]` pour insérer 'x'
   - Ex : `[ANS]`^`2`+`2`*`[ANS]` = `x^2+2*x`
2. Appuyer `[=]`
3. Saisir le point a
4. Appuyer `[=]`
→ LCD1 : valeur de la limite
→ LCD2 : valeur gauche et droite

#### Sous-mode DERIVEE
Calcule f'(x) en un point par différences centrales (h=1e-5).
1. Saisir f(x) (même méthode)
2. Saisir x
→ LCD1 : f'(x) et f(x)

#### Sous-mode EXPLIQ
Appuyer une touche de fonction pour voir son explication :
- `sin` → explication de sin(x)
- `cos` → explication de cos(x)
- `ln`  → explication de ln(x)
- `sqrt`→ explication de √x
- `^`   → explication des puissances
- `hyp` → explication des limites
- `tan` → explication des dérivées
- `[<]` `[)]` pour naviguer entre les étapes

#### Sous-mode INTEGRALE
Calcule ∫[a,b] f(x)dx par la méthode de Simpson (n=100).
1. Saisir f(x)
2. Saisir borne a
3. Saisir borne b
→ LCD1 : résultat de l'intégrale

---

## GESTION MÉMOIRE ARDUINO MEGA
| Ressource | Disponible | Utilisé (estimé) |
|-----------|-----------|-----------------|
| Flash     | 256 KB    | ~45 KB          |
| SRAM      | 8 KB      | ~2.5 KB         |
| EEPROM    | 4 KB      | 0               |

Les textes d'explication sont stockés en **PROGMEM** pour économiser la SRAM.

---

## DÉPANNAGE

**LCD ne s'affiche pas :**
- Vérifier l'adresse I2C avec un scanner I2C
- Ajuster le contraste via le potentiomètre du module

**Clavier ne répond pas :**
- Vérifier le câblage des broches
- S'assurer que les pins dans `main.ino` correspondent au câblage

**Erreur de compilation :**
- Vérifier que tous les fichiers .h sont dans le même dossier que main.ino
- Vérifier que les librairies sont installées

---

*SANDERS IA v2.0 — Calculatrice scientifique embarquée*
