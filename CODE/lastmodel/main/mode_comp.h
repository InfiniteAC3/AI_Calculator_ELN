#pragma once
// ============================================================
//  CALCU IA — mode_comp.h
//  Mode COMP : calculatrice classique + fonctions scientifiques
//  LCD1 : expression + résultat
//  LCD2 : info ANS, mode, débordement
// ============================================================
#include "config.h"
#include "display_mgr.h"
#include "math_engine.h"

class CompMode {
public:
  char   expr[EXPR_MAX];   // Expression en cours
  uint8_t exprLen;

  // ---- Réinitialise le mode ----
  void reset() {
    expr[0]  = '\0';
    exprLen  = 0;
    gState.hasError  = false;
    gState.newEntry  = false;
    gState.shift     = false;
    gState.hyp       = false;
    disp.clearOverflow();
    disp.showExpr(expr);
    disp.showModeInfo();
  }

  // ========================================================
  //  CLAVIER 1 — CALCUL DE BASE
  // ========================================================
  void onCalcKey(char key) {
    if (gState.hasError && key != 'C') {
      disp.showError("Appuyer [C]!    ");
      return;
    }

    // --- CLEAR ---
    if (key == 'C') {
      reset();
      disp.showIA(F("Efface"), F("Pret"));
      return;
    }

    // --- EGAL ---
    if (key == '=') {
      if (exprLen == 0) return;
      _computeAndShow();
      return;
    }

    // Après un résultat : opérateur → continuer avec ANS, chiffre → nouveau
    if (gState.newEntry) {
      bool isOp = (key == '+' || key == '-' || key == '*' || key == '/');
      if (isOp) {
        char ansBuf[RES_MAX];
        formatNum(gState.ans, ansBuf);
        exprLen = strlen(ansBuf);
        strncpy(expr, ansBuf, EXPR_MAX);
      } else {
        expr[0] = '\0'; exprLen = 0;
      }
      gState.newEntry = false;
    }

    if (exprLen >= EXPR_MAX - 2) {
      disp.showIA(F("Expr trop longue"), F("Max 48 car."));
      return;
    }

    expr[exprLen++] = key;
    expr[exprLen]   = '\0';
    disp.showExpr(expr);
  }

  // ========================================================
  //  CLAVIER 2 — SCIENTIFIQUE
  // ========================================================
  void onSciKey(char key) {
    if (gState.hasError && key != 'C' && key != '>') {
      disp.showError("Appuyer [C]!    ");
      return;
    }

    // ---- Touches de contrôle ----
    switch (key) {

      case 'M': // Mode → géré dans main.ino
        return;

      case 'S': // SHIFT
        gState.shift = !gState.shift;
        if (gState.shift) gState.hyp = false;
        disp.showModeInfo();
        return;

      case 'h': // HYP
        gState.hyp = !gState.hyp;
        if (gState.hyp) gState.shift = false;
        disp.showModeInfo();
        return;

      case 'A': { // ANS
        if (gState.newEntry) { expr[0] = '\0'; exprLen = 0; gState.newEntry = false; }
        char ansBuf[RES_MAX];
        formatNum(gState.ans, ansBuf);
        _appendStr(ansBuf);
        disp.showExpr(expr);
        disp.showIA(F("ANS insere"), nullptr);
        return;
      }

      case '.': // Décimal
        if (gState.newEntry) { expr[0] = '\0'; exprLen = 0; gState.newEntry = false; }
        if (exprLen < EXPR_MAX - 2) { expr[exprLen++] = '.'; expr[exprLen] = '\0'; }
        disp.showExpr(expr);
        return;

      case '(': // Parenthèse ouvrante
      case ')': // Parenthèse fermante
        if (gState.newEntry && key == '(') { expr[0] = '\0'; exprLen = 0; gState.newEntry = false; }
        if (exprLen < EXPR_MAX - 2) { expr[exprLen++] = key; expr[exprLen] = '\0'; }
        disp.showExpr(expr);
        return;

      case '^': // Puissance
        if (exprLen < EXPR_MAX - 2) { expr[exprLen++] = '^'; expr[exprLen] = '\0'; }
        disp.showExpr(expr);
        return;

      case 'B': // Backspace
        if (exprLen > 0) { expr[--exprLen] = '\0'; disp.showExpr(expr); }
        return;

      case '>': // Clear total
        reset();
        return;

      case '<': // Pas de navigation spéciale en COMP → backspace
        if (exprLen > 0) { expr[--exprLen] = '\0'; disp.showExpr(expr); }
        return;
    }

    // ---- Fonctions scientifiques ----
    // Extraire l'opérande courante (dernier nombre dans expr)
    double operand = 0.0;
    bool   opFromAns = false;

    if (gState.newEntry || exprLen == 0) {
      operand = gState.ans;
      opFromAns = true;
    } else {
      operand = _extractLastNum();
    }

    bool   valid = true;
    char   lbl[STEP_MAX];
    double result = applyFunc(key, operand, gState.shift, gState.hyp, valid, lbl);

    if (!valid) {
      gState.hasError = true;
      disp.showError(lbl);
      gState.shift = false; gState.hyp = false;
      return;
    }

    // Insérer le résultat
    char resBuf[RES_MAX];
    formatNum(result, resBuf);
    gState.ans     = result;
    gState.newEntry = true;

    // Remplacer le dernier nombre ou mettre en tant que résultat
    if (opFromAns) {
      strncpy(expr, resBuf, EXPR_MAX);
      exprLen = strlen(expr);
    } else {
      // Supprimer le dernier nombre et le remplacer
      _replaceLastNum(resBuf);
    }

    // LCD1 : expression
    disp.showExpr(expr);

    // LCD2 : info fonction
    char infoLine[STEP_MAX];
    snprintf(infoLine, STEP_MAX, "%s(%s)", lbl,
             _numStr(operand));
    disp.showIA(infoLine, resBuf);

    gState.shift = false;
    gState.hyp   = false;
  }

private:
  // ---- Calcule l'expression et affiche le résultat ----
  void _computeAndShow() {
    bool err = false;
    char errMsg[STEP_MAX];
    errMsg[0] = '\0';
    double result = evalExpr(expr, err, errMsg);

    if (err) {
      gState.hasError = true;
      disp.showError(errMsg[0] ? errMsg : "Erreur syntaxe!");
      return;
    }
    if (isnan(result) || isinf(result)) {
      gState.hasError = true;
      disp.showError(isnan(result) ? "Resultat: NaN!" : "Resultat: Infini");
      return;
    }

    char resBuf[RES_MAX];
    formatNum(result, resBuf);
    gState.ans      = result;
    gState.newEntry = true;
    gState.hasError = false;

    disp.showResult(expr, resBuf);

    // LCD2 : ANS confirmé
    char line2[STEP_MAX];
    snprintf(line2, STEP_MAX, "ANS=%s", resBuf);
    disp.showIA(F("Resultat OK"), line2);
  }

  // ---- Ajoute une chaîne à l'expression ----
  void _appendStr(const char* s) {
    while (*s && exprLen < EXPR_MAX - 2) {
      expr[exprLen++] = *s++;
    }
    expr[exprLen] = '\0';
  }

  // ---- Extrait le dernier token numérique (double) ----
  double _extractLastNum() {
    // Chercher le dernier opérateur +,-,*,/,^,(
    int lastOp = -1;
    for (int i = exprLen - 1; i >= 0; i--) {
      char c = expr[i];
      if (c=='+' || c=='-' || c=='*' || c=='/' || c=='^' || c=='(') {
        lastOp = i; break;
      }
    }
    bool err = false;
    return evalExpr(expr + lastOp + 1, err);
  }

  // ---- Remplace le dernier nombre par une chaîne ----
  void _replaceLastNum(const char* newVal) {
    int lastOp = -1;
    for (int i = exprLen - 1; i >= 0; i--) {
      char c = expr[i];
      if (c=='+' || c=='-' || c=='*' || c=='/' || c=='^' || c=='(') {
        lastOp = i; break;
      }
    }
    // Tronquer à la position lastOp+1
    uint8_t newStart = lastOp + 1;
    exprLen = newStart;
    expr[exprLen] = '\0';
    _appendStr(newVal);
  }

  // ---- Formate un double court pour affichage ----
  char* _numStr(double v) {
    static char buf[12];
    formatNum(v, buf, 12);
    return buf;
  }
};

extern CompMode compMode;
