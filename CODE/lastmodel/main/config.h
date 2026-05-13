#pragma once
// ============================================================
//  SANDERS IA — config.h
//  Constantes globales, état, mapping claviers
// ============================================================
#include <Arduino.h>

// --- LCD I2C ---
#define LCD1_ADDR   0x27   // Écran principal
#define LCD2_ADDR   0x21   // Écran secondaire / débordement
#define LCD_COLS    16
#define LCD_ROWS    2

// --- BROCHAGE CLAVIER 1 (CALC) : pins 22-29 ---
#define KP1_ROWS 4
#define KP1_COLS 4
const byte ROW_PINS_CALC[KP1_ROWS] PROGMEM = {22, 23, 24, 25}; // AA BB CC DD
const byte COL_PINS_CALC[KP1_COLS] PROGMEM = {26, 27, 28, 29}; // 11 22 33 44

// Clavier CALC :  7 8 9 ÷ / 4 5 6 × / 1 2 3 - / C 0 = +
const char KEYS_CALC[KP1_ROWS][KP1_COLS] = {
  {'7', '8', '9', '/'},
  {'4', '5', '6', '*'},
  {'1', '2', '3', '-'},
  {'C', '0', '=', '+'}
};

// --- BROCHAGE CLAVIER 2 (SCI) : pins 30-37 ---
#define KP2_ROWS 4
#define KP2_COLS 4
const byte ROW_PINS_SCI[KP2_ROWS] PROGMEM = {30, 31, 32, 33}; // SA SB SC SD
const byte COL_PINS_SCI[KP2_COLS] PROGMEM = {34, 35, 36, 37}; // S1 S2 S3 S4

// Clavier SCI :
//  Q=sqrt  ^=puiss  (=parentO  )=parentF
//  n=ln    .=dec    A=ANS      B=backsp
//  h=hyp   s=sin   d=cos      t=tan
//  M=Mode  S=shift  <=nav<     >=nav>
const char KEYS_SCI[KP2_ROWS][KP2_COLS] = {
  {'Q', '^', '(', ')'},
  {'n', '.', 'A', 'B'},
  {'h', 's', 'd', 't'},
  {'M', 'S', '<', '>'}
};

// --- MODES PRINCIPAUX ---
#define MODE_COMP       0   // Calculatrice classique
#define MODE_MATRIX     1   // Calcul matriciel
#define MODE_EQUATION   2   // Résolution d'équations
#define MODE_IA         3   // Mode IA (limites, dérivées, explications)
#define MODE_COUNT      4

// --- SOUS-MODES IA ---
#define IA_LIMIT        0
#define IA_DERIV        1
#define IA_EXPLAIN      2
#define IA_INTEGRAL     3
#define IA_COUNT        4

// --- MODE ANGULAIRE ---
#define ANG_DEG  0
#define ANG_RAD  1

// --- TAILLES BUFFERS ---
#define EXPR_MAX  48    // Expression principale
#define RES_MAX   20    // Résultat formaté
#define STEP_MAX  17    // Ligne d'explication (16 + '\0')

// ============================================================
//  STRUCTURE D'ÉTAT GLOBAL
// ============================================================
struct CalcState {
  uint8_t  mode;          // MODE_COMP / MODE_MATRIX / MODE_EQUATION / MODE_IA
  uint8_t  iaSubMode;     // Sous-mode IA
  uint8_t  angleMode;     // ANG_DEG / ANG_RAD
  bool     shift;         // Touche shift activée
  bool     hyp;           // Mode hyperbolique activé
  double   ans;           // Dernier résultat (ANS)
  bool     hasError;      // Erreur en cours
  bool     newEntry;      // Prochain chiffre = nouvel entrée
};

// Instance globale (définie dans main.ino)
extern CalcState gState;

// Noms des modes en RAM (courts, PROGMEM serait trop verbeux ici)
const char* const MODE_NAMES[MODE_COUNT] = {
  "COMP", "MATRIX", "EQUAT", "IA"
};
const char* const IA_NAMES[IA_COUNT] = {
  "LIMITE", "DERIVEE", "EXPLIQ", "INTEGR"
};
const char* const ANG_NAMES[2] = { "DEG", "RAD" };
