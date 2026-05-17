// ============================================================
//  CALCU IA — main.ino  v3.0
//
//  LIBRAIRIES : LiquidCrystal_I2C, Keypad
//  MATÉRIEL   : Arduino Mega 2560
//               LCD1 0x27 | LCD2 0x21
//               Clavier CALC : broches 22-29
//               Clavier SCI  : broches 30-37
//
//  NAVIGATION GÉNÉRALE :
//   [M]  = ouvrir/fermer le menu
//   [<]  = précédent dans menu ou explication
//   [>]  = suivant
//   [=]  = confirmer / calculer
//   [C]  = effacer
// ============================================================

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <math.h>
#include <avr/pgmspace.h>

#include "config.h"
#include "display_mgr.h"
#include "math_engine.h"
#include "mode_comp.h"
#include "mode_advanced.h"
#include "mode_ia.h"
#include "menu_system.h"
#include "ia_brain.h"

// ============================================================
//  INSTANCES GLOBALES
// ============================================================
LiquidCrystal_I2C lcd1(LCD1_ADDR, LCD_COLS, LCD_ROWS);
LiquidCrystal_I2C lcd2(LCD2_ADDR, LCD_COLS, LCD_ROWS);

byte rowC[KP1_ROWS] = {22,23,24,25};
byte colC[KP1_COLS] = {26,27,28,29};
byte rowS[KP2_ROWS] = {30,31,32,33};
byte colS[KP2_COLS] = {34,35,36,37};

Keypad kpCalc = Keypad(makeKeymap(KEYS_CALC), rowC, colC, KP1_ROWS, KP1_COLS);
Keypad kpSci  = Keypad(makeKeymap(KEYS_SCI),  rowS, colS, KP2_ROWS, KP2_COLS);

DisplayManager disp;
CalcState      gState;
CompMode       compMode;
MatrixMode     matrixMode;
EquationMode   equMode;
IAMode         iaMode;
MenuSystem     menuSys;
IABrain        iaBrain;

// ============================================================
//  SETUP
// ============================================================
void setup() {
  
  Wire.begin();
  delay(300);

  // État initial
  gState.appState    = APP_MENU;
  gState.menuState   = MENU_ROOT;
  gState.menuSel     = 0;
  gState.mode        = MODE_COMP;
  gState.iaSubMode   = IA_LIMITE;
  gState.angleMode   = ANG_DEG;
  gState.shift       = false;
  gState.hyp         = false;
  gState.ans         = 0.0;
  gState.hasError    = false;
  gState.newEntry    = false;
  gState.lastExpr[0] = '\0';
  gState.lastResult  = 0.0;
  gState.lastFuncType= 0;

  disp.begin();
  disp.splash();

  // Démarrer directement dans le menu
  menuSys.begin();
}

// ============================================================
//  LOOP PRINCIPAL
// ============================================================
void loop() {
  char k1 = kpCalc.getKey();
  char k2 = kpSci.getKey();

  // ============================================================
  //  APP_MENU : tout passe par le menu
  // ============================================================
  if (gState.appState == APP_MENU) {
    if (k2) menuSys.onSciKey(k2);
    if (k1) menuSys.onCalcKey(k1);
    menuSys.update();

    // Quand le menu lance un mode → initialiser
    if (gState.appState == APP_RUN) {
      _initCurrentMode();
    }
    return;
  }

  // ============================================================
  //  APP_RUN : [M] = retour menu, reste = mode courant
  // ============================================================
  if (k2 == 'M') {
    menuSys.enter();
    return;
  }

  // ---- Router vers le mode actif ----
  switch (gState.mode) {

    // ---- COMP ----
    case MODE_COMP:
      if (k1) {
        compMode.onCalcKey(k1);
        // Après calcul : donner contexte IA si résultat disponible
        if (gState.newEntry && gState.lastExpr[0] != '\0') {
          char rb[RES_MAX]; formatNum(gState.ans, rb);
          iaBrain.analyzeResult(gState.lastExpr, gState.ans);
        }
      }
      if (k2) compMode.onSciKey(k2);
      break;

    // ---- MATRIX ----
    case MODE_MATRIX:
      if (k1) matrixMode.onCalcKey(k1);
      if (k2) matrixMode.onSciKey(k2);
      break;

    // ---- EQUATION ----
    case MODE_EQUATION:
      if (k1) equMode.onCalcKey(k1);
      if (k2) equMode.onSciKey(k2);
      break;

    // ---- MODE IA ----
    case MODE_IA:
      switch (gState.iaSubMode) {

        case IA_LIMITE:
        case IA_DERIVEE:
        case IA_INTEGRALE:
          if (k1) iaMode.onCalcKey(k1);
          if (k2) iaMode.onSciKey(k2);
          iaMode.update();
          break;

        case IA_CONCEPT:
          if (k2) iaBrain.onSciKeyConc(k2);
          iaBrain.update();
          break;

        case IA_GUIDE:
          if (k1) iaBrain.onCalcKeyGuide(k1);
          if (k2) iaBrain.onSciKeyGuide(k2);
          break;
      }
      break;
  }
}

// ============================================================
//  INITIALISATION DU MODE SÉLECTIONNÉ PAR LE MENU
// ============================================================
void _initCurrentMode() {
  gState.hasError = false;
  gState.newEntry = false;
  gState.shift    = false;
  gState.hyp      = false;

  switch (gState.mode) {
    case MODE_COMP:
      compMode.reset();
      // LCD2 : aide contextuelle
      lcd2.clear();
      lcd2.setCursor(0,0); lcd2.print(F("COMP: saisir expr"));
      lcd2.setCursor(0,1); lcd2.print(F("[=]calc [M]menu "));
      break;

    case MODE_MATRIX:
      matrixMode.reset();
      break;

    case MODE_EQUATION:
      equMode.reset();
      break;

    case MODE_IA:
      switch (gState.iaSubMode) {
        case IA_LIMITE:
          gState.iaSubMode = IA_LIMIT; // alias
          iaMode.reset();
          break;
        case IA_DERIVEE:
          gState.iaSubMode = IA_DERIV;
          iaMode.reset();
          break;
        case IA_INTEGRALE:
          gState.iaSubMode = IA_INTEGRAL;
          iaMode.reset();
          break;
        case IA_CONCEPT:
          iaBrain.reset();
          break;
        case IA_GUIDE:
          iaBrain.reset();
          break;
      }
      break;
  }
}
