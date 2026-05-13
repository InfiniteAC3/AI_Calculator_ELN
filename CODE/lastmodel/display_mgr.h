#pragma once
// ============================================================
//  SANDERS IA — display_mgr.h
//  Gestionnaire des 2 écrans LCD I2C (0x27 et 0x21)
//  LCD1 : Expression + résultat principal
//  LCD2 : Info mode, débordement, explications IA
// ============================================================
#include <LiquidCrystal_I2C.h>
#include "config.h"

// Instances LCD déclarées dans main.ino
extern LiquidCrystal_I2C lcd1;
extern LiquidCrystal_I2C lcd2;

// ---- Caractères personnalisés ----
// Sera chargé dans setup()
byte charSqrt[8]  = {0x00,0x01,0x01,0x05,0x0D,0x16,0x04,0x00};
byte charDeg[8]   = {0x06,0x09,0x09,0x06,0x00,0x00,0x00,0x00};
byte charRight[8] = {0x00,0x08,0x0C,0x0E,0x0C,0x08,0x00,0x00};
byte charLeft[8]  = {0x00,0x02,0x06,0x0E,0x06,0x02,0x00,0x00};

// ============================================================
//  Classe DisplayManager
// ============================================================
class DisplayManager {
public:
  // ---- Initialisation ----
  void begin() {
    lcd1.init();  lcd1.backlight();
    lcd2.init();  lcd2.backlight();

    // Charger les custom chars sur lcd1
    lcd1.createChar(0, charSqrt);
    lcd1.createChar(1, charDeg);
    lcd1.createChar(2, charRight);
    lcd1.createChar(3, charLeft);
  }

  // ---- Splash Screen ----
  void splash() {
    lcd1.clear();
    lcd1.setCursor(2, 0); lcd1.print(F("SANDERS  IA"));
    lcd1.setCursor(0, 1); lcd1.print(F("Calculatrice v2.0"));
    lcd2.clear();
    lcd2.setCursor(1, 0); lcd2.print(F("--- BIENVENUE ---"));
    lcd2.setCursor(0, 1); lcd2.print(F("Mode: COMP | DEG"));
    delay(2200);
    lcd1.clear(); lcd2.clear();
  }

  // ---- LCD1 : Affiche expression + barre de statut ----
  // expr : chaîne de l'expression (max EXPR_MAX)
  // cursorPos : position du curseur dans expr
  void showExpr(const char* expr, int cursorPos = -1) {
    lcd1.clear();
    int len = strlen(expr);

    // Ligne 0 : expression (fenêtre glissante si >16)
    lcd1.setCursor(0, 0);
    int start = (len > 16) ? len - 16 : 0;
    for (int i = start; i < len && i < start + 16; i++) {
      lcd1.print(expr[i]);
    }

    // Ligne 1 : barre de statut [mode][ang][SH][HY]
    lcd1.setCursor(0, 1);
    _printStatusBar();
  }

  // ---- LCD1 : Affiche résultat ----
  void showResult(const char* expr, const char* result) {
    lcd1.clear();
    int elen = strlen(expr);
    int rlen = strlen(result);

    // Ligne 0 : expression (16 derniers chars)
    lcd1.setCursor(0, 0);
    int start = (elen > 16) ? elen - 16 : 0;
    for (int i = start; i < elen; i++) lcd1.print(expr[i]);

    // Ligne 1 : = résultat
    lcd1.setCursor(0, 1);
    lcd1.print('=');
    // Si résultat > 15 chars → débordement sur LCD2
    if (rlen <= 15) {
      lcd1.print(result);
    } else {
      for (int i = 0; i < 15; i++) lcd1.print(result[i]);
      // Débordement sur LCD2
      lcd2.clear();
      lcd2.setCursor(0, 0); lcd2.print(F(">> Suite result:"));
      lcd2.setCursor(0, 1);
      for (int i = 15; i < rlen && i < 31; i++) lcd2.print(result[i]);
      _overflowActive = true;
      return;
    }
    _overflowActive = false;
  }

  // ---- LCD1 : Affiche erreur ----
  void showError(const char* msg) {
    lcd1.clear();
    lcd1.setCursor(0, 0); lcd1.print(F("!! ERREUR !!    "));
    lcd1.setCursor(0, 1);
    int l = strlen(msg);
    for (int i = 0; i < 16 && i < l; i++) lcd1.print(msg[i]);
    lcd2.clear();
    lcd2.setCursor(0, 0); lcd2.print(F("Appuyer [C] pour"));
    lcd2.setCursor(0, 1); lcd2.print(F("effacer erreur  "));
  }

  // ---- LCD2 : Info mode ----
  void showModeInfo() {
    if (_overflowActive) return; // ne pas écraser le débordement
    lcd2.clear();
    lcd2.setCursor(0, 0);
    lcd2.print(F("Mode: "));
    lcd2.print(MODE_NAMES[gState.mode]);
    if (gState.mode == MODE_IA) {
      lcd2.print(F("-"));
      lcd2.print(IA_NAMES[gState.iaSubMode]);
    }
    lcd2.setCursor(0, 1);
    lcd2.print(F("["));
    lcd2.print(ANG_NAMES[gState.angleMode]);
    lcd2.print(F("]"));
    if (gState.shift) lcd2.print(F(" SHIFT"));
    if (gState.hyp)   lcd2.print(F(" HYP"));
    lcd2.print(F("  ANS="));
    _printShortDouble(lcd2, gState.ans);
  }

  // ---- LCD2 : Affiche une ligne IA / explication ----
  void showIA(const char* line1, const char* line2 = nullptr) {
    lcd2.clear();
    lcd2.setCursor(0, 0);
    _printTrunc(lcd2, line1, 16);
    if (line2) {
      lcd2.setCursor(0, 1);
      _printTrunc(lcd2, line2, 16);
    }
  }

  // ---- LCD2 : Affiche une expression longue en défilant ----
  // (pour les explications étape par étape)
  void showIAStep(uint8_t stepNum, const char* stepText) {
    lcd2.clear();
    lcd2.setCursor(0, 0);
    lcd2.print(F("Etape "));
    lcd2.print(stepNum);
    lcd2.print(F(":        "));
    lcd2.setCursor(0, 1);
    _printTrunc(lcd2, stepText, 16);
  }

  // ---- LCD1 + LCD2 : Affichage entrée matricielle ----
  void showMatrixEntry(uint8_t row, uint8_t col, uint8_t size,
                       const char* valStr) {
    lcd1.clear();
    lcd1.setCursor(0, 0);
    lcd1.print(F("Matrice "));
    lcd1.print(size); lcd1.print(F("x")); lcd1.print(size);
    lcd1.setCursor(0, 1);
    lcd1.print(F("M["));
    lcd1.print(row + 1);
    lcd1.print(F(","));
    lcd1.print(col + 1);
    lcd1.print(F("]="));
    lcd1.print(valStr);
    lcd1.print(F("_"));

    lcd2.clear();
    lcd2.setCursor(0, 0); lcd2.print(F("< >:naviguer   "));
    lcd2.setCursor(0, 1); lcd2.print(F("[=]:valider    "));
  }

  // ---- LCD1 : Affichage équation ----
  void showEquation(uint8_t degree, const char* coeffStr,
                    uint8_t step) {
    lcd1.clear();
    if (degree == 1) {
      lcd1.setCursor(0, 0); lcd1.print(F("Eq. lineaire    "));
      lcd1.setCursor(0, 1);
      if (step == 0) lcd1.print(F("a=?             "));
      else           lcd1.print(F("b=?             "));
    } else {
      lcd1.setCursor(0, 0); lcd1.print(F("Eq. quadratique "));
      lcd1.setCursor(0, 1);
      if (step == 0)      lcd1.print(F("a=?             "));
      else if (step == 1) lcd1.print(F("b=?             "));
      else                lcd1.print(F("c=?             "));
    }
    // Affiche la valeur en cours de saisie
    lcd1.setCursor(2, 1);
    lcd1.print(coeffStr);
    lcd1.print(F("_    "));
  }

  // ---- Efface overflow flag ----
  void clearOverflow() { _overflowActive = false; }

  // ---- Reset complet ----
  void clearAll() {
    lcd1.clear(); lcd2.clear();
    _overflowActive = false;
  }

private:
  bool _overflowActive = false;

  // Barre de statut ligne 1 de LCD1
  void _printStatusBar() {
    lcd1.print(F("["));
    lcd1.print(MODE_NAMES[gState.mode]);
    lcd1.print(F("|"));
    lcd1.print(ANG_NAMES[gState.angleMode]);
    lcd1.print(F("]"));
    if (gState.shift) lcd1.print(F("SH "));
    if (gState.hyp)   lcd1.print(F("HY "));
  }

  void _printTrunc(LiquidCrystal_I2C& lcd, const char* s, uint8_t maxLen) {
    for (uint8_t i = 0; i < maxLen && s[i]; i++) lcd.print(s[i]);
  }

  void _printShortDouble(LiquidCrystal_I2C& lcd, double v) {
    if (isnan(v))     { lcd.print(F("NaN")); return; }
    if (isinf(v))     { lcd.print(v > 0 ? F("+Inf") : F("-Inf")); return; }
    if (v == 0.0)     { lcd.print(F("0")); return; }
    if (abs(v) >= 1e6 || (abs(v) < 0.001 && v != 0)) {
      lcd.print(v, 2); return;
    }
    if (v == (long)v) { lcd.print((long)v); return; }
    lcd.print(v, 4);
  }
};

// Instance globale
extern DisplayManager disp;
