#pragma once
// ============================================================
//  SANDERS IA — mode_ia.h  (v2.1)
//
//  LCD1 : expression défilante (ligne 0) + résultat à droite (ligne 1)
//  LCD2 : explication TOUJOURS visible, navigable via :
//           [>]         → étape suivante  (manuel, 1 clic)
//           [<]         → étape précédente (manuel, 1 clic)
//           Shift + [>] → défilement auto avant  (toggle ON/OFF)
//           Shift + [<] → défilement auto arrière (toggle ON/OFF)
//
//  Sous-modes IA (touche [S]) :
//    LIMITE | DERIVEE | EXPLIQ | INTEGRALE
// ============================================================
#include "config.h"
#include "display_mgr.h"
#include "math_engine.h"

// ============================================================
//  EXPLICATIONS EN PROGMEM
// ============================================================
const char EXP_SIN_0[]  PROGMEM = "sin(x) oscillat.";
const char EXP_SIN_1[]  PROGMEM = "Valeurs: [-1,+1]";
const char EXP_SIN_2[]  PROGMEM = "sin(0) = 0      ";
const char EXP_SIN_3[]  PROGMEM = "sin(90deg) = 1  ";
const char EXP_SIN_4[]  PROGMEM = "Periode: 2*PI   ";
const char EXP_SIN_5[]  PROGMEM = "Derivee: cos(x) ";
const char EXP_SIN_6[]  PROGMEM = "";

const char EXP_COS_0[]  PROGMEM = "cos(x) oscillat.";
const char EXP_COS_1[]  PROGMEM = "Valeurs: [-1,+1]";
const char EXP_COS_2[]  PROGMEM = "cos(0) = 1      ";
const char EXP_COS_3[]  PROGMEM = "cos(90deg) = 0  ";
const char EXP_COS_4[]  PROGMEM = "Periode: 2*PI   ";
const char EXP_COS_5[]  PROGMEM = "Deriv: -sin(x)  ";
const char EXP_COS_6[]  PROGMEM = "";

const char EXP_TAN_0[]  PROGMEM = "tan=sin(x)/cos  ";
const char EXP_TAN_1[]  PROGMEM = "Undef si cos=0  ";
const char EXP_TAN_2[]  PROGMEM = "tan(45deg) = 1  ";
const char EXP_TAN_3[]  PROGMEM = "Periode: PI     ";
const char EXP_TAN_4[]  PROGMEM = "Deriv: 1/cos^2  ";
const char EXP_TAN_5[]  PROGMEM = "";

const char EXP_LN_0[]   PROGMEM = "ln(x) : x > 0  ";
const char EXP_LN_1[]   PROGMEM = "ln(1)  = 0      ";
const char EXP_LN_2[]   PROGMEM = "ln(e)  = 1      ";
const char EXP_LN_3[]   PROGMEM = "ln(a*b)=lna+lnb ";
const char EXP_LN_4[]   PROGMEM = "ln(a^n)=n*ln(a) ";
const char EXP_LN_5[]   PROGMEM = "Derivee : 1/x   ";
const char EXP_LN_6[]   PROGMEM = "";

const char EXP_LOG_0[]  PROGMEM = "log10 : base 10 ";
const char EXP_LOG_1[]  PROGMEM = "log10(1)   = 0  ";
const char EXP_LOG_2[]  PROGMEM = "log10(10)  = 1  ";
const char EXP_LOG_3[]  PROGMEM = "log10(100) = 2  ";
const char EXP_LOG_4[]  PROGMEM = "=ln(x)/ln(10)   ";
const char EXP_LOG_5[]  PROGMEM = "";

const char EXP_SQRT_0[] PROGMEM = "sqrt(x): x >= 0 ";
const char EXP_SQRT_1[] PROGMEM = "sqrt(4)  = 2    ";
const char EXP_SQRT_2[] PROGMEM = "sqrt(x) = x^0.5 ";
const char EXP_SQRT_3[] PROGMEM = "Deriv:1/(2sqrtx)";
const char EXP_SQRT_4[] PROGMEM = "sqrt(ab)=sqrta*b";
const char EXP_SQRT_5[] PROGMEM = "";

const char EXP_POW_0[]  PROGMEM = "a^n : puissance ";
const char EXP_POW_1[]  PROGMEM = "2^10 = 1024     ";
const char EXP_POW_2[]  PROGMEM = "a^0  = 1        ";
const char EXP_POW_3[]  PROGMEM = "a^(-n)=1/(a^n)  ";
const char EXP_POW_4[]  PROGMEM = "a^(1/n)=racine n";
const char EXP_POW_5[]  PROGMEM = "Deriv:n*x^(n-1) ";
const char EXP_POW_6[]  PROGMEM = "";

const char EXP_LIM_0[]  PROGMEM = "lim x->a f(x)=L ";
const char EXP_LIM_1[]  PROGMEM = "Appro. gauche   ";
const char EXP_LIM_2[]  PROGMEM = "Appro. droite   ";
const char EXP_LIM_3[]  PROGMEM = "L=R => lim exist";
const char EXP_LIM_4[]  PROGMEM = "L!=R => pas lim ";
const char EXP_LIM_5[]  PROGMEM = "ex: lim x->0 x^2";
const char EXP_LIM_6[]  PROGMEM = "     = 0 (cont.)";
const char EXP_LIM_7[]  PROGMEM = "";

const char EXP_DER_0[]  PROGMEM = "f'(x)=taux var. ";
const char EXP_DER_1[]  PROGMEM = "Methode numeriq.";
const char EXP_DER_2[]  PROGMEM = "f'~[f(x+h)-     ";
const char EXP_DER_3[]  PROGMEM = "  f(x-h)] / 2h  ";
const char EXP_DER_4[]  PROGMEM = "h = 0.00001     ";
const char EXP_DER_5[]  PROGMEM = "(x^n)' = n*x^n-1";
const char EXP_DER_6[]  PROGMEM = "(uv)'=u'v+uv'   ";
const char EXP_DER_7[]  PROGMEM = "";

const char EXP_INT_0[]  PROGMEM = "Int=aire sous f ";
const char EXP_INT_1[]  PROGMEM = "Methode Simpson ";
const char EXP_INT_2[]  PROGMEM = "n=100 intervalles";
const char EXP_INT_3[]  PROGMEM = "h=(b-a)/n       ";
const char EXP_INT_4[]  PROGMEM = "S=h/3*(f0+4f1+  ";
const char EXP_INT_5[]  PROGMEM = " 2f2+...+fn)    ";
const char EXP_INT_6[]  PROGMEM = "";

// Tables PROGMEM
const char* const EXP_SIN[]  PROGMEM = {EXP_SIN_0,EXP_SIN_1,EXP_SIN_2,EXP_SIN_3,EXP_SIN_4,EXP_SIN_5,EXP_SIN_6};
const char* const EXP_COS[]  PROGMEM = {EXP_COS_0,EXP_COS_1,EXP_COS_2,EXP_COS_3,EXP_COS_4,EXP_COS_5,EXP_COS_6};
const char* const EXP_TAN[]  PROGMEM = {EXP_TAN_0,EXP_TAN_1,EXP_TAN_2,EXP_TAN_3,EXP_TAN_4,EXP_TAN_5};
const char* const EXP_LN[]   PROGMEM = {EXP_LN_0,EXP_LN_1,EXP_LN_2,EXP_LN_3,EXP_LN_4,EXP_LN_5,EXP_LN_6};
const char* const EXP_LOG[]  PROGMEM = {EXP_LOG_0,EXP_LOG_1,EXP_LOG_2,EXP_LOG_3,EXP_LOG_4,EXP_LOG_5};
const char* const EXP_SQRT[] PROGMEM = {EXP_SQRT_0,EXP_SQRT_1,EXP_SQRT_2,EXP_SQRT_3,EXP_SQRT_4,EXP_SQRT_5};
const char* const EXP_POW[]  PROGMEM = {EXP_POW_0,EXP_POW_1,EXP_POW_2,EXP_POW_3,EXP_POW_4,EXP_POW_5,EXP_POW_6};
const char* const EXP_LIM[]  PROGMEM = {EXP_LIM_0,EXP_LIM_1,EXP_LIM_2,EXP_LIM_3,EXP_LIM_4,EXP_LIM_5,EXP_LIM_6,EXP_LIM_7};
const char* const EXP_DER[]  PROGMEM = {EXP_DER_0,EXP_DER_1,EXP_DER_2,EXP_DER_3,EXP_DER_4,EXP_DER_5,EXP_DER_6,EXP_DER_7};
const char* const EXP_INT[]  PROGMEM = {EXP_INT_0,EXP_INT_1,EXP_INT_2,EXP_INT_3,EXP_INT_4,EXP_INT_5,EXP_INT_6};

// ============================================================
//  CLASSE IAMode
// ============================================================
#define EXPL_SCROLL_MS 1200u   // ms entre deux pas de défilement LCD2
#define EXPR_SCROLL_MS  380u   // ms entre deux pas de défilement LCD1

class IAMode {
public:
  // Saisie f(x)
  char    fExpr[EXPR_MAX];
  uint8_t fLen;
  // Saisie paramètre numérique
  char    numBuf[16];
  uint8_t numLen;
  // Résultat affiché sur LCD1 ligne 1
  char    resultBuf[RES_MAX];
  // Étape de saisie : 0=f(x)  1=param a  2=param b
  uint8_t iaStep;
  // Paramètres calcul
  double  paramA, paramB;

  // --- Scroll LCD1 (expression) ---
  uint8_t       exprScrollOff;
  unsigned long lastExprScroll;
  bool          exprScrollActive;

  // --- Explication LCD2 ---
  const char* const* explLines;
  uint8_t     explCount;
  uint8_t     explIdx;
  char        explTitle[10];
  // Scroll explication
  bool          explScrollFwd;
  bool          explScrollBwd;
  unsigned long lastExplScroll;

  // ================================================================
  //  RESET
  // ================================================================
  void reset() {
    fExpr[0] = '\0';  fLen = 0;
    numBuf[0]= '\0';  numLen = 0;
    resultBuf[0] = '\0';
    paramA = paramB = 0.0;
    iaStep = 0;
    exprScrollOff    = 0;
    lastExprScroll   = 0;
    exprScrollActive = false;
    explScrollFwd    = false;
    explScrollBwd    = false;
    lastExplScroll   = 0;
    gState.shift     = false;
    gState.hyp       = false;

    switch (gState.iaSubMode) {
      case IA_LIMIT:    _loadExpl("LIMITE",  EXP_LIM,  8); break;
      case IA_DERIV:    _loadExpl("DERIVEE", EXP_DER,  8); break;
      case IA_EXPLAIN:  _loadExpl("AIDE",    EXP_SIN,  7); break;
      case IA_INTEGRAL: _loadExpl("INTEGR",  EXP_INT,  7); break;
    }
    _refreshLCD1();
    _refreshLCD2();
  }

  // ================================================================
  //  UPDATE — appelé depuis loop() en continu
  // ================================================================
  void update() {
    unsigned long now = millis();

    // ---- Défilement auto LCD2 (explication) ----
    if ((explScrollFwd || explScrollBwd) &&
        (now - lastExplScroll >= EXPL_SCROLL_MS)) {
      lastExplScroll = now;
      if (explScrollFwd) {
        explIdx = (explIdx + 1) % explCount;
        // Sauter la sentinelle vide
        char tmp[STEP_MAX];
        PGM_P p = (PGM_P)pgm_read_word(&explLines[explIdx]);
        strncpy_P(tmp, p, STEP_MAX-1);
        if (tmp[0] == '\0') explIdx = 0;
      } else {
        explIdx = (explIdx == 0) ? explCount - 2 : explIdx - 1;
        if ((int8_t)explIdx < 0) explIdx = 0;
      }
      _refreshLCD2();
    }

    // ---- Défilement auto LCD1 (expression) ----
    if (exprScrollActive && fLen > 16 &&
        (now - lastExprScroll >= EXPR_SCROLL_MS)) {
      lastExprScroll = now;
      exprScrollOff++;
      if (exprScrollOff + 16 >= fLen) exprScrollOff = 0;
      _refreshLCD1();
    }
  }

  // ================================================================
  //  CLAVIER 1
  // ================================================================
  void onCalcKey(char key) {
    if (key == 'C') { reset(); return; }

    if (iaStep == 0) {
      if (key == '=') {
        if (gState.iaSubMode == IA_EXPLAIN) {
          // EXPLIQ : [=] = étape suivante manuelle
          explIdx = (explIdx + 1) % explCount;
          char tmp[STEP_MAX];
          PGM_P p = (PGM_P)pgm_read_word(&explLines[explIdx]);
          strncpy_P(tmp, p, STEP_MAX-1);
          if (tmp[0] == '\0') explIdx = 0;
          _refreshLCD2(); return;
        }
        iaStep = 1;
        _showParamPrompt(); return;
      }
      if (fLen < EXPR_MAX - 2) {
        fExpr[fLen++] = key; fExpr[fLen] = '\0';
      }
      exprScrollOff    = 0;
      exprScrollActive = (fLen > 16);
      resultBuf[0]     = '\0';
      _refreshLCD1();

    } else {
      // Saisie paramètre
      if ((key >= '0' && key <= '9') || key == '.') {
        if (numLen < 15) { numBuf[numLen++] = key; numBuf[numLen] = '\0'; }
        _showParamEdit(); return;
      }
      if (key == '-' && numLen == 0) {
        numBuf[numLen++] = '-'; numBuf[numLen] = '\0';
        _showParamEdit(); return;
      }
      if (key == '=') { _validateParam(); return; }
    }
  }

  // ================================================================
  //  CLAVIER 2
  // ================================================================
  void onSciKey(char key) {
    if (key == 'M') return; // géré dans main.ino

    // ---- [S] sans shift : changer sous-mode IA ----
    if (key == 'S' && !gState.shift) {
      gState.iaSubMode = (gState.iaSubMode + 1) % IA_COUNT;
      reset(); return;
    }

    // ================================================================
    //  SHIFT + [>] / [<] : défilement auto LCD2
    // ================================================================
    if (gState.shift) {
      if (key == '>') {
        // Toggle défilement avant
        if (explScrollFwd) {
          explScrollFwd = false; // STOP
        } else {
          explScrollFwd = true;
          explScrollBwd = false;
          lastExplScroll = millis();
        }
        gState.shift = false;
        _refreshLCD2(); return;
      }
      if (key == '<') {
        // Toggle défilement arrière
        if (explScrollBwd) {
          explScrollBwd = false; // STOP
        } else {
          explScrollBwd = true;
          explScrollFwd = false;
          lastExplScroll = millis();
        }
        gState.shift = false;
        _refreshLCD2(); return;
      }
      // Shift + autre touche → désactiver shift et continuer normalement
      gState.shift = false;
    }

    // ---- Touches de contrôle standard ----
    switch (key) {
      case 'S':  // Activer shift
        gState.shift = !gState.shift;
        gState.hyp   = false;
        _refreshLCD2(); return;

      case 'h':  // HYP
        gState.hyp   = !gState.hyp;
        gState.shift = false;
        _refreshLCD2(); return;

      case '>':  // Étape suivante MANUELLE (sans shift)
        explScrollFwd = explScrollBwd = false;
        explIdx = (explIdx + 1) % explCount;
        { char tmp[STEP_MAX];
          PGM_P p = (PGM_P)pgm_read_word(&explLines[explIdx]);
          strncpy_P(tmp, p, STEP_MAX-1);
          if (tmp[0] == '\0') explIdx = 0; }
        _refreshLCD2(); return;

      case '<':  // Étape précédente MANUELLE (sans shift)
        explScrollFwd = explScrollBwd = false;
        if (explIdx == 0) explIdx = explCount - 2;
        else              explIdx--;
        _refreshLCD2(); return;

      case 'B':  // Backspace
        if (iaStep == 0 && fLen > 0) {
          fExpr[--fLen] = '\0';
          exprScrollOff    = 0;
          exprScrollActive = (fLen > 16);
          resultBuf[0]     = '\0';
          _refreshLCD1();
        } else if (iaStep > 0 && numLen > 0) {
          numBuf[--numLen] = '\0'; _showParamEdit();
        }
        return;

      case 'A':  // Insérer 'x' dans l'expression
        if (iaStep == 0 && fLen < EXPR_MAX - 2) {
          fExpr[fLen++] = 'x'; fExpr[fLen] = '\0';
          exprScrollActive = (fLen > 16);
          resultBuf[0] = '\0';
          _refreshLCD1();
        }
        return;

      case '.':
        if (iaStep == 0 && fLen < EXPR_MAX - 2) {
          fExpr[fLen++] = '.'; fExpr[fLen] = '\0'; _refreshLCD1();
        } else if (iaStep > 0 && numLen < 15) {
          numBuf[numLen++] = '.'; numBuf[numLen] = '\0'; _showParamEdit();
        }
        return;

      case '(':
      case ')':
      case '^':
        if (iaStep == 0 && fLen < EXPR_MAX - 2) {
          fExpr[fLen++] = key; fExpr[fLen] = '\0'; _refreshLCD1();
        }
        return;
    }

    // ---- Touches fonctions → chargement explication LCD2 ----
    if (iaStep == 0) {
      switch (key) {
        case 's': _loadExpl("SIN",   EXP_SIN,  7); break;
        case 'd': _loadExpl("COS",   EXP_COS,  7); break;
        case 't': _loadExpl("TAN",   EXP_TAN,  6); break;
        case 'n': _loadExpl("LOG10", EXP_LOG,  6); break;
        case 'l': _loadExpl("LN",    EXP_LN,   7); break;
        case 'Q': _loadExpl("SQRT",  EXP_SQRT, 6); break;
        default: return;
      }
      explIdx = 0;
      _refreshLCD2();
    }
  }

private:
  // ================================================================
  //  CALCULS
  // ================================================================
  void _computeLimit() {
    double xT = paramA;
    double eps[] = {0.1, 0.01, 0.001, 0.0001, 0.00001};
    double lv = 0, rv = 0;
    bool err = false;
    for (int8_t i = 4; i >= 0; i--) {
      lv = evalAt(fExpr, xT - eps[i], err); err = false;
      rv = evalAt(fExpr, xT + eps[i], err); err = false;
    }
    bool converge = (abs(lv - rv) < 1e-5 && !isnan(lv) && !isinf(lv));
    double lim = (lv + rv) / 2.0;

    if (converge) {
      formatNum(lim, resultBuf);
      gState.ans = lim;
    } else {
      strncpy(resultBuf, "indefinie", RES_MAX);
    }
    exprScrollOff = 0; exprScrollActive = (fLen > 16);
    _refreshLCD1();

    // LCD2 : valeurs G/D
    char lb[8], rb[8];
    formatNum(lv, lb, 8); formatNum(rv, rb, 8);
    lcd2.clear();
    lcd2.setCursor(0, 0);
    lcd2.print(F("G: "));
    for(int i=0;i<6&&lb[i];i++) lcd2.print(lb[i]);
    lcd2.print(F("  D: "));
    for(int i=0;i<4&&rb[i];i++) lcd2.print(rb[i]);
    lcd2.setCursor(0, 1);
    lcd2.print(converge ? F("lim = ") : F("Pas de limite!"));
    if (converge)
      for(int i=0;i<10&&resultBuf[i];i++) lcd2.print(resultBuf[i]);
    gState.newEntry = true;
  }

  void _computeDerivative() {
    double x = paramA, h = 1e-5;
    bool err = false;
    double fp = evalAt(fExpr, x + h, err); err = false;
    double fm = evalAt(fExpr, x - h, err); err = false;
    double fx = evalAt(fExpr, x,     err); err = false;
    double d  = (fp - fm) / (2.0 * h);

    formatNum(d, resultBuf);
    gState.ans = d;
    exprScrollOff = 0; exprScrollActive = (fLen > 16);
    _refreshLCD1();

    char fxBuf[10]; formatNum(fx, fxBuf, 10);
    lcd2.clear();
    lcd2.setCursor(0, 0);
    lcd2.print(F("f(x) = "));
    for(int i=0;i<9&&fxBuf[i];i++) lcd2.print(fxBuf[i]);
    lcd2.setCursor(0, 1);
    lcd2.print(F("diff. centrale  "));
    gState.newEntry = true;
  }

  void _computeIntegral() {
    double a = paramA, b = paramB;
    const uint8_t n = 100;
    double h = (b - a) / n;
    double sum = 0.0;
    bool err = false;

    double fa = evalAt(fExpr, a, err); err = false;
    double fb2= evalAt(fExpr, b, err); err = false;
    sum = fa + fb2;

    for (uint8_t i = 1; i < n; i++) {
      double fi = evalAt(fExpr, a + i * h, err);
      if (err) { disp.showError("Err dans [a,b]! "); return; }
      sum += (i % 2 == 0) ? 2.0 * fi : 4.0 * fi;
    }
    double result = sum * h / 3.0;
    formatNum(result, resultBuf);
    gState.ans = result;
    exprScrollOff = 0; exprScrollActive = (fLen > 16);
    _refreshLCD1();

    char ab[17];
    snprintf(ab, 17, "[%s;%s]", _fmt(a), _fmt(b));
    lcd2.clear();
    lcd2.setCursor(0, 0); lcd2.print(F("Simpson n=100   "));
    lcd2.setCursor(0, 1);
    for(int i=0;i<16&&ab[i];i++) lcd2.print(ab[i]);
    gState.newEntry = true;
  }

  // ================================================================
  //  AFFICHAGE
  // ================================================================
  void _refreshLCD1() {
    disp.showIAExpr(fExpr, exprScrollOff,
                    resultBuf[0] ? resultBuf : nullptr);
  }

  void _refreshLCD2() {
    if (!explLines || explCount == 0) return;
    if (explIdx >= explCount) explIdx = 0;
    char lineBuf[STEP_MAX];
    PGM_P p = (PGM_P)pgm_read_word(&explLines[explIdx]);
    strncpy_P(lineBuf, p, STEP_MAX - 1);
    lineBuf[STEP_MAX - 1] = '\0';
    if (lineBuf[0] == '\0') { explIdx = 0; _refreshLCD2(); return; }
    // Compter lignes non vides pour affichage "x/total"
    uint8_t visCount = 0;
    for (uint8_t i = 0; i < explCount; i++) {
      char tb[2]; PGM_P tp = (PGM_P)pgm_read_word(&explLines[i]);
      strncpy_P(tb, tp, 1); tb[1]='\0';
      if (tb[0] != '\0') visCount++;
    }
    disp.showIAExpl(explTitle, lineBuf,
                    explIdx, visCount,
                    explScrollFwd, explScrollBwd);
  }

  void _showParamPrompt() {
    lcd1.clear();
    switch (gState.iaSubMode) {
      case IA_LIMIT:
        lcd1.setCursor(0,0); lcd1.print(F("lim x -> ?      "));
        break;
      case IA_DERIV:
        lcd1.setCursor(0,0); lcd1.print(F("f'(x) en x = ?  "));
        break;
      case IA_INTEGRAL:
        lcd1.setCursor(0,0);
        lcd1.print(iaStep == 1 ? F("Borne a = ?     ")
                               : F("Borne b = ?     "));
        break;
    }
    lcd1.setCursor(0,1); lcd1.print(F("> _             "));
  }

  void _showParamEdit() {
    lcd1.setCursor(2, 1);
    lcd1.print(numBuf); lcd1.print(F("_           "));
  }

  void _validateParam() {
    double val = atof(numBuf);
    numBuf[0] = '\0'; numLen = 0;

    switch (gState.iaSubMode) {
      case IA_LIMIT:
        paramA = val; _computeLimit(); iaStep = 0; break;
      case IA_DERIV:
        paramA = val; _computeDerivative(); iaStep = 0; break;
      case IA_INTEGRAL:
        if (iaStep == 1) {
          paramA = val; iaStep = 2; _showParamPrompt();
        } else {
          paramB = val; _computeIntegral(); iaStep = 0;
        }
        break;
      default:
        iaStep = 0; break;
    }
  }

  void _loadExpl(const char* title,
                 const char* const* lines, uint8_t count) {
    explLines = lines; explCount = count; explIdx = 0;
    strncpy(explTitle, title, 9); explTitle[9] = '\0';
    explScrollFwd = explScrollBwd = false;
  }

  char* _fmt(double v) {
    static char fb[8]; formatNum(v, fb, 8); return fb;
  }
};

extern IAMode iaMode;
