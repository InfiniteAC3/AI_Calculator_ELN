#pragma once
// ============================================================
//  SANDERS IA — mode_ia.h
//  Mode IA embarqué sur Arduino Mega
//  Sous-modes : LIMITE | DERIVEE | EXPLIQ | INTEGRALE
//  
//  Toutes les explications en PROGMEM pour économiser la RAM
//  Calculs numériques (pas CAS symbolique)
// ============================================================
#include "config.h"
#include "display_mgr.h"
#include "math_engine.h"

// ============================================================
//  EXPLICATIONS EN PROGMEM (économise SRAM)
// ============================================================
// Chaque tableau : série de lignes de 16 chars max, terminée par ""
const char EXP_SIN_0[] PROGMEM = "sin(x) oscillat.";
const char EXP_SIN_1[] PROGMEM = "Valeurs: [-1,+1]";
const char EXP_SIN_2[] PROGMEM = "sin(0)=0        ";
const char EXP_SIN_3[] PROGMEM = "sin(PI/2)=1     ";
const char EXP_SIN_4[] PROGMEM = "Periode: 2*PI   ";
const char EXP_SIN_5[] PROGMEM = "";

const char EXP_COS_0[] PROGMEM = "cos(x) oscillat.";
const char EXP_COS_1[] PROGMEM = "cos(0)=1        ";
const char EXP_COS_2[] PROGMEM = "cos(PI/2)=0     ";
const char EXP_COS_3[] PROGMEM = "Periode: 2*PI   ";
const char EXP_COS_4[] PROGMEM = "";

const char EXP_LN_0[] PROGMEM  = "ln(x): x>0 requis";
const char EXP_LN_1[] PROGMEM  = "ln(1)=0         ";
const char EXP_LN_2[] PROGMEM  = "ln(e)=1         ";
const char EXP_LN_3[] PROGMEM  = "ln(x*y)=ln(x)+..";
const char EXP_LN_4[] PROGMEM  = "Derivee: 1/x    ";
const char EXP_LN_5[] PROGMEM  = "";

const char EXP_LOG_0[] PROGMEM = "log10: base 10  ";
const char EXP_LOG_1[] PROGMEM = "log10(1)=0      ";
const char EXP_LOG_2[] PROGMEM = "log10(10)=1     ";
const char EXP_LOG_3[] PROGMEM = "log10(100)=2    ";
const char EXP_LOG_4[] PROGMEM = "";

const char EXP_SQRT_0[] PROGMEM= "sqrt(x): x>=0   ";
const char EXP_SQRT_1[] PROGMEM= "sqrt(4)=2       ";
const char EXP_SQRT_2[] PROGMEM= "sqrt(x)=x^(1/2) ";
const char EXP_SQRT_3[] PROGMEM= "Derivee:1/(2sqrtx)";
const char EXP_SQRT_4[] PROGMEM= "";

const char EXP_POW_0[] PROGMEM = "a^n: a puiss n  ";
const char EXP_POW_1[] PROGMEM = "2^10 = 1024     ";
const char EXP_POW_2[] PROGMEM = "a^0 = 1         ";
const char EXP_POW_3[] PROGMEM = "a^(-n)=1/(a^n)  ";
const char EXP_POW_4[] PROGMEM = "";

const char EXP_LIM_0[] PROGMEM = "Limite numerique";
const char EXP_LIM_1[] PROGMEM = "Approche x->a   ";
const char EXP_LIM_2[] PROGMEM = "si L=R: limite OK";
const char EXP_LIM_3[] PROGMEM = "si L!=R: pas lim";
const char EXP_LIM_4[] PROGMEM = "";

const char EXP_DER_0[] PROGMEM = "Derivee numeriq.";
const char EXP_DER_1[] PROGMEM = "f'(x)=[f(x+h)-  ";
const char EXP_DER_2[] PROGMEM = " f(x-h)]/(2h)   ";
const char EXP_DER_3[] PROGMEM = "h=0.0001        ";
const char EXP_DER_4[] PROGMEM = "";

const char EXP_INT_0[] PROGMEM = "Integrale [a,b] ";
const char EXP_INT_1[] PROGMEM = "Methode Simpson ";
const char EXP_INT_2[] PROGMEM = "n=100 intervalles";
const char EXP_INT_3[] PROGMEM = "";

// Tables de pointeurs
const char* const EXP_SIN[]  PROGMEM = {EXP_SIN_0,EXP_SIN_1,EXP_SIN_2,EXP_SIN_3,EXP_SIN_4,EXP_SIN_5};
const char* const EXP_COS[]  PROGMEM = {EXP_COS_0,EXP_COS_1,EXP_COS_2,EXP_COS_3,EXP_COS_4};
const char* const EXP_LN[]   PROGMEM = {EXP_LN_0,EXP_LN_1,EXP_LN_2,EXP_LN_3,EXP_LN_4,EXP_LN_5};
const char* const EXP_LOG[]  PROGMEM = {EXP_LOG_0,EXP_LOG_1,EXP_LOG_2,EXP_LOG_3,EXP_LOG_4};
const char* const EXP_SQRT[] PROGMEM = {EXP_SQRT_0,EXP_SQRT_1,EXP_SQRT_2,EXP_SQRT_3,EXP_SQRT_4};
const char* const EXP_POW[]  PROGMEM = {EXP_POW_0,EXP_POW_1,EXP_POW_2,EXP_POW_3,EXP_POW_4};
const char* const EXP_LIM[]  PROGMEM = {EXP_LIM_0,EXP_LIM_1,EXP_LIM_2,EXP_LIM_3,EXP_LIM_4};
const char* const EXP_DER[]  PROGMEM = {EXP_DER_0,EXP_DER_1,EXP_DER_2,EXP_DER_3,EXP_DER_4};
const char* const EXP_INT[]  PROGMEM = {EXP_INT_0,EXP_INT_1,EXP_INT_2,EXP_INT_3};

// ============================================================
//  CLASSE IAMode
// ============================================================
class IAMode {
public:
  // --- Saisie expression avec 'x' ---
  char   fExpr[EXPR_MAX];    // f(x)
  uint8_t fLen;
  char   numBuf[16];          // Saisie d'un paramètre (a, b, N)
  uint8_t numLen;

  // --- Paramètres selon sous-mode ---
  double  paramA, paramB;    // Limite/Intégrale : point ou bornes
  uint8_t iaStep;            // Étape de saisie
  // 0=saisir f(x), 1=saisir a (ou point), 2=saisir b (intégrale)

  // --- Explication courante ---
  uint8_t  explIdx;           // Index dans la table d'explications
  char     explTitle[STEP_MAX];

  // ---- Reset ----
  void reset() {
    fExpr[0] = '\0'; fLen = 0;
    numBuf[0]= '\0'; numLen = 0;
    paramA = paramB = 0.0;
    iaStep = 0; explIdx = 0;
    _showSubModePrompt();
  }

  // ====================================================
  //  CLAVIER 1 — CHIFFRES & OPÉRATEURS
  // ====================================================
  void onCalcKey(char key) {
    if (key == 'C') { reset(); return; }

    if (iaStep == 0) {
      // Saisie de f(x)
      if (key == '=') { iaStep = 1; _showParamPrompt(); return; }
      if (fLen < EXPR_MAX - 2) {
        fExpr[fLen++] = key;
        fExpr[fLen]   = '\0';
      }
      _showFExpr();
    } else {
      // Saisie d'un paramètre numérique
      if ((key>='0'&&key<='9') || key=='.') {
        if (numLen < 15) { numBuf[numLen++]=key; numBuf[numLen]='\0'; }
        _showParamEdit();
        return;
      }
      if (key=='-' && numLen==0) {
        numBuf[numLen++]='-'; numBuf[numLen]='\0';
        _showParamEdit(); return;
      }
      if (key == '=') { _validateParam(); return; }
    }
  }

  // ====================================================
  //  CLAVIER 2 — SCI
  // ====================================================
  void onSciKey(char key) {
    if (key == 'M') return;
    if (key == 'S') { // Shift → changer sous-mode IA
      gState.iaSubMode = (gState.iaSubMode + 1) % IA_COUNT;
      reset();
      return;
    }
    if (key == 'B' || key == '<') {
      if (iaStep == 0 && fLen > 0) {
        fExpr[--fLen] = '\0'; _showFExpr();
      } else if (iaStep > 0 && numLen > 0) {
        numBuf[--numLen] = '\0'; _showParamEdit();
      }
      return;
    }
    if (key == '>') { reset(); return; }

    // Explications de fonctions  (touche s/d/t/n/l/Q)
    if (iaStep == 0) {
      switch (key) {
        case 's': _showExplanation("SIN", EXP_SIN, 6);  break;
        case 'd': _showExplanation("COS", EXP_COS, 5);  break;
        case 'n': _showExplanation("LOG10",EXP_LOG,5);  break;
        case 'l': _showExplanation("LN",  EXP_LN,  6);  break;
        case 'Q': _showExplanation("SQRT",EXP_SQRT,5);  break;
        case '^': _showExplanation("PUIS",EXP_POW, 5);  break;
        case 'h': _showExplanation("LIM", EXP_LIM, 5);  break;
        case 't': _showExplanation("DER", EXP_DER, 5);  break;
      }
      return;
    }

    // Navigation dans l'explication (< >)
    if (key == '(' && explIdx > 0) { explIdx--; _showExplLine(); return; }
    if (key == ')') { explIdx++; _showExplLine(); return; }

    // Insérer 'x' dans f(x) via touche ANS en mode saisie
    if (key == 'A' && iaStep == 0) {
      if (fLen < EXPR_MAX - 2) { fExpr[fLen++]='x'; fExpr[fLen]='\0'; }
      _showFExpr();
      return;
    }
    if (key == '.') {
      if (iaStep == 0 && fLen < EXPR_MAX-2) {
        fExpr[fLen++]='.'; fExpr[fLen]='\0'; _showFExpr();
      } else if (iaStep > 0 && numLen < 15) {
        numBuf[numLen++]='.'; numBuf[numLen]='\0'; _showParamEdit();
      }
    }
  }

private:
  // ====================================================
  //  CALCUL DE LIMITE (numérique)
  // ====================================================
  void _computeLimit() {
    double xTarget = paramA;
    double eps[5] = {0.1, 0.01, 0.001, 0.0001, 0.00001};
    double leftVal = 0, rightVal = 0;
    bool err = false;

    // Approche par la gauche et par la droite
    for (int i = 4; i >= 0; i--) {
      leftVal  = evalAt(fExpr, xTarget - eps[i], err);
      rightVal = evalAt(fExpr, xTarget + eps[i], err);
      if (!err) break;
      err = false;
    }

    // Vérifier si la valeur directe est accessible
    double direct = evalAt(fExpr, xTarget, err);
    err = false;

    char lBuf[RES_MAX], rBuf[RES_MAX], dBuf[RES_MAX];
    formatNum(leftVal,  lBuf);
    formatNum(rightVal, rBuf);

    bool converge = (abs(leftVal - rightVal) < 1e-6);
    double limVal = (leftVal + rightVal) / 2.0;
    formatNum(limVal, dBuf);

    // Affichage
    lcd1.clear();
    lcd1.setCursor(0,0);
    lcd1.print(F("lim x->"));
    char apBuf[8]; formatNum(xTarget, apBuf, 8);
    lcd1.print(apBuf);
    lcd1.setCursor(0,1);
    if (converge) {
      lcd1.print(F("= "));
      lcd1.print(dBuf);
      gState.ans = limVal;
    } else {
      lcd1.print(F("n'existe pas!  "));
    }

    // LCD2 : détails
    lcd2.clear();
    lcd2.setCursor(0,0);
    lcd2.print(F("G:"));
    for(int i=0;i<7&&lBuf[i];i++) lcd2.print(lBuf[i]);
    lcd2.print(F(" D:"));
    for(int i=0;i<5&&rBuf[i];i++) lcd2.print(rBuf[i]);
    lcd2.setCursor(0,1);
    lcd2.print(converge ? F("Limite existe  ") : F("Pas de limite! "));

    gState.newEntry = true;
  }

  // ====================================================
  //  CALCUL DE DÉRIVÉE (numérique, différences centrales)
  // ====================================================
  void _computeDerivative() {
    double x = paramA;
    double h = 1e-5;
    bool err = false;

    double fx_ph = evalAt(fExpr, x + h, err);
    if (err) { disp.showError("Erreur eval f(x+h)"); return; }
    double fx_mh = evalAt(fExpr, x - h, err);
    if (err) { disp.showError("Erreur eval f(x-h)"); return; }

    double deriv = (fx_ph - fx_mh) / (2.0 * h);
    double fx    = evalAt(fExpr, x, err);

    char xBuf[RES_MAX], dBuf[RES_MAX], fBuf[RES_MAX];
    formatNum(x,     xBuf);
    formatNum(deriv, dBuf);
    formatNum(fx,    fBuf);

    lcd1.clear();
    lcd1.setCursor(0,0);
    lcd1.print(F("f'("));
    for(int i=0;i<4&&xBuf[i];i++) lcd1.print(xBuf[i]);
    lcd1.print(F(")="));
    lcd1.print(dBuf);
    lcd1.setCursor(0,1);
    lcd1.print(F("f(x)="));
    lcd1.print(fBuf);

    lcd2.clear();
    lcd2.setCursor(0,0); lcd2.print(F("Derivee en x="));
    lcd2.print(xBuf);
    lcd2.setCursor(0,1); lcd2.print(F("Meth:diff centr."));

    gState.ans = deriv;
    gState.newEntry = true;
  }

  // ====================================================
  //  INTÉGRALE (méthode de Simpson, n=100)
  // ====================================================
  void _computeIntegral() {
    double a = paramA, bV = paramB;
    uint16_t n = 100;
    if ((int)n % 2 != 0) n++;

    double h = (bV - a) / n;
    double sum = 0.0;
    bool err = false;

    sum = evalAt(fExpr, a, err) + evalAt(fExpr, bV, err);
    if (err) { disp.showError("Erreur bornes!  "); return; }

    for (uint16_t i = 1; i < n; i++) {
      double xi = a + i * h;
      double fi = evalAt(fExpr, xi, err);
      if (err) { disp.showError("Erreur dans [a,b]"); return; }
      sum += (i % 2 == 0) ? 2.0 * fi : 4.0 * fi;
    }
    double result = sum * h / 3.0;

    char resBuf[RES_MAX], aBuf[8], bBuf[8];
    formatNum(result, resBuf);
    formatNum(a, aBuf, 8);
    formatNum(bV, bBuf, 8);

    lcd1.clear();
    lcd1.setCursor(0,0);
    lcd1.print(F("Int ["));
    lcd1.print(aBuf); lcd1.print(F(",")); lcd1.print(bBuf);
    lcd1.print(F("]"));
    lcd1.setCursor(0,1);
    lcd1.print(F("= "));
    lcd1.print(resBuf);

    lcd2.clear();
    lcd2.setCursor(0,0); lcd2.print(F("Simpson n=100   "));
    lcd2.setCursor(0,1); lcd2.print(F("Resultat OK     "));

    gState.ans = result;
    gState.newEntry = true;
  }

  // ====================================================
  //  AFFICHAGE
  // ====================================================
  void _showFExpr() {
    lcd1.clear();
    lcd1.setCursor(0,0);
    const char* promptMode[] = {"lim f(x)=", "f'(x)=  ", "expliq: ", "int f(x)="};
    lcd1.print(promptMode[gState.iaSubMode]);
    lcd1.setCursor(0,1);
    int start = (fLen > 16) ? fLen - 16 : 0;
    for (int i = start; i < fLen; i++) lcd1.print(fExpr[i]);
    lcd1.print(F("_"));
    lcd2.clear();
    lcd2.setCursor(0,0); lcd2.print(F("[A]=inserer x   "));
    lcd2.setCursor(0,1); lcd2.print(F("[=]:suivant     "));
  }

  void _showParamPrompt() {
    lcd1.clear();
    lcd2.clear();
    switch (gState.iaSubMode) {
      case IA_LIMIT:
        lcd1.setCursor(0,0); lcd1.print(F("Saisir point a: "));
        lcd2.setCursor(0,0); lcd2.print(F("x->a            "));
        break;
      case IA_DERIV:
        lcd1.setCursor(0,0); lcd1.print(F("Saisir x=       "));
        lcd2.setCursor(0,0); lcd2.print(F("Calcule f'(x)   "));
        break;
      case IA_INTEGRAL:
        lcd1.setCursor(0,0); lcd1.print(F("Saisir borne a: "));
        lcd2.setCursor(0,0); lcd2.print(F("Integrale [a,b] "));
        break;
      case IA_EXPLAIN:
        // En mode EXPLAIN, on saute directement
        _showExplanation("AIDE", EXP_LIM, 5);
        return;
    }
    lcd2.setCursor(0,1); lcd2.print(F("[=]:confirmer   "));
    _showParamEdit();
  }

  void _showParamEdit() {
    lcd1.setCursor(0,1);
    lcd1.print(numBuf);
    lcd1.print(F("_           "));
  }

  void _validateParam() {
    double val = atof(numBuf);
    numBuf[0] = '\0'; numLen = 0;

    if (gState.iaSubMode == IA_LIMIT || gState.iaSubMode == IA_DERIV) {
      paramA = val;
      if (gState.iaSubMode == IA_LIMIT) _computeLimit();
      else _computeDerivative();
    } else if (gState.iaSubMode == IA_INTEGRAL) {
      if (iaStep == 1) {
        paramA = val;
        iaStep = 2;
        lcd1.clear();
        lcd1.setCursor(0,0); lcd1.print(F("Saisir borne b: "));
        _showParamEdit();
      } else {
        paramB = val;
        _computeIntegral();
        iaStep = 0;
      }
      return;
    }
    iaStep = 0;
  }

  // ====================================================
  //  AFFICHAGE EXPLICATIONS (PROGMEM)
  // ====================================================
  // lines est un tableau de const char* stocké en PROGMEM
  void _showExplanation(const char* title, const char* const* lines, uint8_t count) {
    strncpy(explTitle, title, STEP_MAX-1);
    explIdx = 0;
    _showExplLine(lines, count);
  }

  void _showExplLine(const char* const* lines = nullptr, uint8_t count = 0) {
    // Lire le pointeur depuis PROGMEM
    static const char* const* savedLines = nullptr;
    static uint8_t savedCount = 0;
    if (lines) { savedLines = lines; savedCount = count; }
    if (!savedLines) return;

    if (explIdx >= savedCount) explIdx = savedCount - 1;

    char lineBuf[STEP_MAX];
    PGM_P p = (PGM_P)pgm_read_word(&savedLines[explIdx]);
    strncpy_P(lineBuf, p, STEP_MAX-1);
    lineBuf[STEP_MAX-1] = '\0';

    if (lineBuf[0] == '\0') {
      explIdx = 0;
      p = (PGM_P)pgm_read_word(&savedLines[0]);
      strncpy_P(lineBuf, p, STEP_MAX-1);
    }

    lcd2.clear();
    lcd2.setCursor(0,0);
    lcd2.print(explTitle);
    lcd2.print(F(" ("));
    lcd2.print(explIdx+1);
    lcd2.print(F("/"));
    lcd2.print(savedCount);
    lcd2.print(F(")"));
    lcd2.setCursor(0,1);
    lcd2.print(lineBuf);

    lcd1.clear();
    lcd1.setCursor(0,0); lcd1.print(F(">> EXPLICATION  "));
    lcd1.setCursor(0,1); lcd1.print(F("[(]:prec [)]:suiv"));
  }

  void _showSubModePrompt() {
    lcd1.clear(); lcd2.clear();
    lcd1.setCursor(0,0); lcd1.print(F("MODE IA :       "));
    lcd1.setCursor(0,1); lcd1.print(IA_NAMES[gState.iaSubMode]);
    lcd2.setCursor(0,0); lcd2.print(F("[S]=chg sous-mode"));
    lcd2.setCursor(0,1); lcd2.print(F("f(x) avec [A]=x "));
    _showFExpr();
  }
};

extern IAMode iaMode;
