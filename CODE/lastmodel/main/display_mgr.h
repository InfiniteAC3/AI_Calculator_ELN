#pragma once
// ============================================================
//  SANDERS IA — display_mgr.h
//  Gestionnaire des 2 écrans LCD I2C (0x27 et 0x21)
// ============================================================
#include <LiquidCrystal_I2C.h>
#include "config.h"

extern LiquidCrystal_I2C lcd1;
extern LiquidCrystal_I2C lcd2;

byte charSqrt[8]  = {0x00,0x01,0x01,0x05,0x0D,0x16,0x04,0x00};
byte charDeg[8]   = {0x06,0x09,0x09,0x06,0x00,0x00,0x00,0x00};
byte charRight[8] = {0x00,0x08,0x0C,0x0E,0x0C,0x08,0x00,0x00};
byte charLeft[8]  = {0x00,0x02,0x06,0x0E,0x06,0x02,0x00,0x00};

class DisplayManager {
public:
  void begin() {
    lcd1.init();  lcd1.backlight();
    lcd2.init();  lcd2.backlight();
    lcd1.createChar(0, charSqrt);
    lcd1.createChar(1, charDeg);
    lcd1.createChar(2, charRight);
    lcd1.createChar(3, charLeft);
  }

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

  void showExpr(const char* expr, int cursorPos = -1) {
    lcd1.clear();
    int len = strlen(expr);
    lcd1.setCursor(0, 0);
    int start = (len > 16) ? len - 16 : 0;
    for (int i = start; i < len && i < start + 16; i++) {
      lcd1.print(expr[i]);
    }
    lcd1.setCursor(0, 1);
    _printStatusBar();
  }

  void showResult(const char* expr, const char* result) {
    lcd1.clear();
    int elen = strlen(expr);
    int rlen = strlen(result);
    lcd1.setCursor(0, 0);
    int start = (elen > 16) ? elen - 16 : 0;
    for (int i = start; i < elen; i++) lcd1.print(expr[i]);
    lcd1.setCursor(0, 1);
    lcd1.print('=');
    if (rlen <= 15) {
      lcd1.print(result);
    } else {
      for (int i = 0; i < 15; i++) lcd1.print(result[i]);
      lcd2.clear();
      lcd2.setCursor(0, 0); lcd2.print(F(">> Suite result:"));
      lcd2.setCursor(0, 1);
      for (int i = 15; i < rlen && i < 31; i++) lcd2.print(result[i]);
      _overflowActive = true;
      return;
    }
    _overflowActive = false;
  }

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

  void showModeInfo() {
    if (_overflowActive) return;
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

  // --- CORRECTION DES ERREURS D'AMBIGUÏTÉ ---

  // Version 1 : Pour les chaînes classiques, String ou nullptr
  void showIA(String line1, String line2 = "") {
    lcd2.clear();
    lcd2.setCursor(0, 0);
    _printTrunc(lcd2, line1.c_str(), 16);
    if (line2.length() > 0) {
      lcd2.setCursor(0, 1);
      _printTrunc(lcd2, line2.c_str(), 16);
    }
  }

  // Version 2 : Pour les chaînes en Flash (macro F)
  void showIA(const __FlashStringHelper* line1, const __FlashStringHelper* line2 = nullptr) {
    lcd2.clear();
    lcd2.setCursor(0, 0);
    lcd2.print(line1);
    if (line2 != nullptr) {
      lcd2.setCursor(0, 1);
      lcd2.print(line2);
    }
  }
  
  // ------------------------------------------

  void showIAStep(uint8_t stepNum, const char* stepText) {
    lcd2.clear();
    lcd2.setCursor(0, 0);
    lcd2.print(F("Etape "));
    lcd2.print(stepNum);
    lcd2.print(F(":        "));
    lcd2.setCursor(0, 1);
    _printTrunc(lcd2, stepText, 16);
  }

  void showMatrixEntry(uint8_t row, uint8_t col, uint8_t size, const char* valStr) {
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

  void showEquation(uint8_t degree, const char* coeffStr, uint8_t step) {
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
    lcd1.setCursor(2, 1);
    lcd1.print(coeffStr);
    lcd1.print(F("_    "));
  }

  // ---- LCD1 spécifique MODE IA ----
  // Ligne 0 : expression avec fenêtre glissante (scrollOffset)
  // Ligne 1 : résultat aligné à droite  "           =3.14"
  void showIAExpr(const char* expr, uint8_t scrollOffset,
                  const char* result = nullptr) {
    lcd1.clear();

    // --- Ligne 0 : fenêtre de 16 chars dans l'expression ---
    lcd1.setCursor(0, 0);
    uint8_t len = strlen(expr);
    if (len == 0) {
      lcd1.print(F("f(x)=?          "));
    } else {
      uint8_t start = scrollOffset;
      if (start >= len) start = 0;
      for (uint8_t i = start; i < start + 16 && i < len; i++) {
        lcd1.print(expr[i]);
      }
      // Indicateur de défilement si expr trop longue
      if (len > 16) {
        lcd1.setCursor(15, 0);
        lcd1.print(start + 16 < len ? '>' : '<');
      }
    }

    // --- Ligne 1 : résultat aligné à droite ---
    lcd1.setCursor(0, 1);
    if (result && result[0] != '\0') {
      uint8_t rlen = strlen(result) + 1; // +1 pour '='
      uint8_t pad  = (rlen < 16) ? (16 - rlen) : 0;
      for (uint8_t i = 0; i < pad; i++) lcd1.print(' ');
      lcd1.print('=');
      for (uint8_t i = 0; i < 15 && result[i]; i++) lcd1.print(result[i]);
    } else {
      lcd1.print(F("                "));
    }
  }

  // ---- LCD2 spécifique MODE IA : ligne d'explication ----
  // Ligne 0 : "SIN (2/5) ►" ou "SIN (2/5) ◄"
  // Ligne 1 : texte de l'étape courante
  void showIAExpl(const char* title, const char* line,
                  uint8_t idx, uint8_t total,
                  bool scrollFwd, bool scrollBwd) {
    lcd2.clear();
    // Ligne 0 : titre + numéro + indicateur de défilement
    lcd2.setCursor(0, 0);
    _printTrunc(lcd2, title, 8);
    lcd2.print(F(" ("));
    lcd2.print(idx + 1);
    lcd2.print(F("/"));
    lcd2.print(total);
    lcd2.print(F(")"));
    if (scrollFwd)      lcd2.print(F(" >"));
    else if (scrollBwd) lcd2.print(F(" <"));

    // Ligne 1 : texte de l'étape
    lcd2.setCursor(0, 1);
    _printTrunc(lcd2, line, 16);
  }

  void clearOverflow() { _overflowActive = false; }
  void clearAll() {
    lcd1.clear(); lcd2.clear();
    _overflowActive = false;
  }

private:
  bool _overflowActive = false;
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
    if (!s) return;
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

extern DisplayManager disp;