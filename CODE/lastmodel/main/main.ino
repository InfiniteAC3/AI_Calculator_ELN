// ============================================================
//  SANDERS IA — main.ino
//  Calculatrice Scientifique + IA pour Arduino Mega
//  
//  MATÉRIEL :
//   - Arduino Mega 2560
//   - 2x LCD 16x2 I2C : 0x27 (principal) | 0x21 (secondaire)
//   - Clavier CALC  : rows 22-25, cols 26-29
//   - Clavier SCI   : rows 30-33, cols 34-37
//
//  LIBRAIRIES REQUISES (Gestionnaire de bibliothèques Arduino) :
//   - LiquidCrystal_I2C  (par Frank de Brabander)
//   - Keypad             (par Mark Stanley)
//
//  MODES :
//   [M] → cycle COMP → MATRIX → EQUAT → IA
//   En IA : [S] cycle sous-mode LIMITE → DERIVEE → EXPLIQ → INTEGR
//
//  CLAVIER SCI MAPPING :
//   Row A : sqrt | ^  |  (  |  )
//   Row B : ln   | .  | ANS | ⌫
//   Row C : hyp  |sin |cos  |tan
//   Row D : Mode |shift| <  | >
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

// ============================================================
//  INSTANCES GLOBALES
// ============================================================
LiquidCrystal_I2C lcd1(LCD1_ADDR, LCD_COLS, LCD_ROWS);
LiquidCrystal_I2C lcd2(LCD2_ADDR, LCD_COLS, LCD_ROWS);

// Claviers — les tableaux de broches ne peuvent pas être const
// avec la lib Keypad (elle les modifie en interne)
byte rowC[KP1_ROWS] = {22, 23, 24, 25};
byte colC[KP1_COLS] = {26, 27, 28, 29};
byte rowS[KP2_ROWS] = {30, 31, 32, 33};
byte colS[KP2_COLS] = {34, 35, 36, 37};

Keypad kpCalc = Keypad(makeKeymap(KEYS_CALC), rowC, colC, KP1_ROWS, KP1_COLS);
Keypad kpSci  = Keypad(makeKeymap(KEYS_SCI),  rowS, colS, KP2_ROWS, KP2_COLS);

// Managers & modes
DisplayManager disp;
CalcState      gState;
CompMode       compMode;
MatrixMode     matrixMode;
EquationMode   equMode;
IAMode         iaMode;

// ============================================================
//  SETUP
// ============================================================
void setup() {
  Wire.begin();
  delay(300);

  // Initialiser l'état global
  gState.mode       = MODE_COMP;
  gState.iaSubMode  = IA_LIMIT;
  gState.angleMode  = ANG_DEG;
  gState.shift      = false;
  gState.hyp        = false;
  gState.ans        = 0.0;
  gState.hasError   = false;
  gState.newEntry   = false;

  // Init affichage
  disp.begin();
  disp.splash();

  // Démarrer en mode COMP
  compMode.reset();
}

// ============================================================
//  LOOP PRINCIPAL
// ============================================================
void loop() {
  char k1 = kpCalc.getKey();
  char k2 = kpSci.getKey();

  // La touche Mode (M) est gérée ici → priorité absolue
  if (k2 == 'M') {
    _cycleMode();
    return;
  }

  // Router vers le mode courant
  switch (gState.mode) {
    case MODE_COMP:
      if (k1) compMode.onCalcKey(k1);
      if (k2) compMode.onSciKey(k2);
      break;

    case MODE_MATRIX:
      if (k1) matrixMode.onCalcKey(k1);
      if (k2) matrixMode.onSciKey(k2);
      break;

    case MODE_EQUATION:
      if (k1) equMode.onCalcKey(k1);
      if (k2) equMode.onSciKey(k2);
      break;

    case MODE_IA:
      if (k1) iaMode.onCalcKey(k1);
      if (k2) iaMode.onSciKey(k2);
      iaMode.update(); // défilement automatique LCD1 (expr) + LCD2 (explication)
      break;
  }
}

// ============================================================
//  CYCLE DE MODE
// ============================================================
void _cycleMode() {
  gState.mode = (gState.mode + 1) % MODE_COUNT;
  gState.shift = false;
  gState.hyp   = false;
  gState.hasError = false;
  gState.newEntry = false;

  // Affichage transition
  lcd1.clear(); lcd2.clear();
  lcd1.setCursor(0, 0); lcd1.print(F(">> MODE :       "));
  lcd1.setCursor(0, 1); lcd1.print(F("                "));
  lcd1.setCursor(4, 1);

  switch (gState.mode) {
    case MODE_COMP:
      lcd1.print(F("  COMP  "));
      lcd2.setCursor(0,0); lcd2.print(F("Calcul standard "));
      lcd2.setCursor(0,1); lcd2.print(F("+ - * / ^ sin.."));
      break;
    case MODE_MATRIX:
      lcd1.print(F(" MATRICE"));
      lcd2.setCursor(0,0); lcd2.print(F("ADD SUB MUL     "));
      lcd2.setCursor(0,1); lcd2.print(F("DET INV TRA 2x2/3x3"));
      break;
    case MODE_EQUATION:
      lcd1.print(F("EQUATION"));
      lcd2.setCursor(0,0); lcd2.print(F("Lineaire :ax+b=0"));
      lcd2.setCursor(0,1); lcd2.print(F("Quadrat: ax2+..."));
      break;
    case MODE_IA:
      lcd1.print(F("   IA   "));
      lcd2.setCursor(0,0); lcd2.print(F("Limite|Derivee  "));
      lcd2.setCursor(0,1); lcd2.print(F("Integrale|Expliq"));
      break;
  }
  delay(1800);

  // Initialiser le mode
  switch (gState.mode) {
    case MODE_COMP:     compMode.reset();    break;
    case MODE_MATRIX:   matrixMode.reset();  break;
    case MODE_EQUATION: equMode.reset();     break;
    case MODE_IA:       iaMode.reset();      break;
  }
}
