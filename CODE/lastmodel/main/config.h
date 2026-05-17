#pragma once
// ============================================================
//  CALCU IA — config.h  v3.0
// ============================================================
#include <Arduino.h>

#define LCD1_ADDR   0x27
#define LCD2_ADDR   0x21
#define LCD_COLS    16
#define LCD_ROWS    2

#define KP1_ROWS 4
#define KP1_COLS 4
const byte ROW_PINS_CALC[KP1_ROWS] = {22,23,24,25};
const byte COL_PINS_CALC[KP1_COLS] = {26,27,28,29};
const char KEYS_CALC[KP1_ROWS][KP1_COLS] = {
  {'7','8','9','/'},{'4','5','6','*'},
  {'1','2','3','-'},{'C','0','=','+'}
};

#define KP2_ROWS 4
#define KP2_COLS 4
const byte ROW_PINS_SCI[KP2_ROWS] = {30,31,32,33};
const byte COL_PINS_SCI[KP2_COLS] = {34,35,36,37};
const char KEYS_SCI[KP2_ROWS][KP2_COLS] = {
  {'Q','^','(', ')'},
  {'n','.', 'A', 'B'},
  {'h','s', 'd', 't'},
  {'M','S', '<', '>'}
};

// App states
#define APP_MENU  0
#define APP_RUN   1

// Menu states
#define MENU_ROOT    0
#define MENU_NORMAL  1
#define MENU_IA      2

// Normal sub-modes
#define NORMAL_COMP     0
#define NORMAL_MATRIX   1
#define NORMAL_EQUATION 2
#define NORMAL_COUNT    3

// IA sub-modes
#define IA_LIMITE    0
#define IA_DERIVEE   1
#define IA_INTEGRALE 2
#define IA_CONCEPT   3
#define IA_GUIDE     4
#define IA_COUNT     5

// Back-compat
#define MODE_COMP     0
#define MODE_MATRIX   1
#define MODE_EQUATION 2
#define MODE_IA       3
#define MODE_COUNT    4
#define IA_LIMIT      0
#define IA_DERIV      1
#define IA_INTEGRAL   2
#define IA_EXPLAIN    3

#define ANG_DEG  0
#define ANG_RAD  1
#define EXPR_MAX  48
#define RES_MAX   20
#define STEP_MAX  17

struct CalcState {
  uint8_t appState;
  uint8_t menuState;
  uint8_t menuSel;
  uint8_t mode;
  uint8_t iaSubMode;
  uint8_t angleMode;
  bool    shift;
  bool    hyp;
  double  ans;
  bool    hasError;
  bool    newEntry;
  char    lastExpr[EXPR_MAX];
  double  lastResult;
  uint8_t lastFuncType;
};
extern CalcState gState;

const char* const NORMAL_NAMES[NORMAL_COUNT] = {"COMP","MATRIX","EQUAT"};
const char* const NORMAL_DESC[NORMAL_COUNT]  = {
  "Calcul scientif.","Matrices 2x2/3x3","Eq.lineaire/deg2"
};
const char* const IA_NAMES[IA_COUNT] = {"LIMITE","DERIVEE","INTEGR.","CONCEPT","GUIDE"};
const char* const IA_DESC[IA_COUNT]  = {
  "Calcul lim x->a ","Derivee f'(x0)  ","Integrale [a,b] ",
  "Explorer les math","Aide pas-a-pas  "
};
const char* const MODE_NAMES[MODE_COUNT] = {"COMP","MATRIX","EQUAT","IA"};
const char* const ANG_NAMES[2] = {"DEG","RAD"};
