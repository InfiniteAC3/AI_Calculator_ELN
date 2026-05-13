#pragma once
// ============================================================
//  SANDERS IA — math_engine.h
//  Parser récursif, fonctions scientifiques, substitution x
// ============================================================
#include <math.h>
#include "config.h"

// --- AJOUT DES FONCTIONS HYPERBOLIQUES INVERSES POUR AVR ---
#ifndef asinh
#define asinh(x) (log((x) + sqrt(square(x) + 1.0)))
#endif

#ifndef acosh
#define acosh(x) (log((x) + sqrt(square(x) - 1.0)))
#endif

#ifndef atanh
#define atanh(x) (0.5 * log((1.0 + (x)) / (1.0 - (x))))
#endif
// -----------------------------------------------------------

// ============================================================
//  FORMATAGE NOMBRE → CHAR ARRAY
// ============================================================
void formatNum(double v, char* buf, uint8_t bufSize = RES_MAX) {
  if (isnan(v))  { strncpy(buf, "NaN",  bufSize); return; }
  if (isinf(v))  { strncpy(buf, v > 0 ? "+Inf" : "-Inf", bufSize); return; }
  if (v == 0.0)  { strncpy(buf, "0", bufSize); return; }

  // Entier exact dans plage raisonnable
  long lv = (long)v;
  if ((double)lv == v && abs(v) < 1e9) {
    snprintf(buf, bufSize, "%ld", lv);
    return;
  }
  // Très grand ou très petit → notation xxx.xxe+yy
  if (abs(v) >= 1e10 || (abs(v) < 1e-4 && v != 0)) {
    dtostrf(v, 10, 4, buf);
    return;
  }
  // Standard 6 décimales, supprimer les zéros trailing
  dtostrf(v, 1, 6, buf);
  // Supprimer zéros finaux
  int dot = -1;
  for (int i = 0; buf[i]; i++) if (buf[i] == '.') { dot = i; break; }
  if (dot >= 0) {
    int end = strlen(buf) - 1;
    while (end > dot && buf[end] == '0') buf[end--] = '\0';
    if (buf[end] == '.') buf[end] = '\0';
  }
}

// ============================================================
//  PARSER RÉCURSIF (interne)
// ============================================================
namespace MathParser {
  const char* src;
  uint8_t     pos;
  bool        err;
  char        errMsg[STEP_MAX];

  void init(const char* s) {
    src = s; pos = 0; err = false; errMsg[0] = '\0';
  }

  void setError(const char* msg) {
    err = true;
    strncpy(errMsg, msg, STEP_MAX - 1);
    errMsg[STEP_MAX - 1] = '\0';
  }

  inline char peek()    { return src[pos]; }
  inline char consume() { return src[pos++]; }
  inline void skip()    { pos++; }

  void skipSpaces() { while (src[pos] == ' ') pos++; }

  double parseExpr();
  double parseTerm();
  double parsePow();
  double parseUnary();
  double parsePrimary();

  double parsePrimary() {
    skipSpaces();
    if (peek() == '(') {
      skip();
      double v = parseExpr();
      skipSpaces();
      if (peek() == ')') skip();
      else setError("Parenthese manq.");
      return v;
    }
    // Nombre
    bool hasDot = false, hasDigit = false;
    double result = 0.0, dec = 0.1;
    if (peek() == '\0') { setError("Expr incomplete"); return 0; }
    while (true) {
      char c = peek();
      if (c >= '0' && c <= '9') {
        hasDigit = true;
        if (hasDot) { result += (c - '0') * dec; dec *= 0.1; }
        else        result = result * 10.0 + (c - '0');
        skip();
      } else if (c == '.' && !hasDot) {
        hasDot = true; skip();
      } else break;
    }
    if (!hasDigit) setError("Nombre attendu");
    return result;
  }

  double parseUnary() {
    skipSpaces();
    if (peek() == '-') { skip(); return -parsePow(); }
    if (peek() == '+') { skip(); return  parsePow(); }
    return parsePow();
  }

  double parsePow() {
    double base = parsePrimary();
    skipSpaces();
    if (peek() == '^') {
      skip();
      double exp = parseUnary(); // associativité droite
      if (base < 0 && exp != (int)exp) { setError("Base neg puiss frac"); return 0; }
      base = pow(base, exp);
    }
    return base;
  }

  double parseTerm() {
    double r = parseUnary();
    while (!err) {
      skipSpaces();
      char op = peek();
      if (op == '*') { skip(); r *= parseUnary(); }
      else if (op == '/') {
        skip();
        double d = parseUnary();
        if (abs(d) < 1e-15) { setError("Division par zero"); return 0; }
        r /= d;
      } else break;
    }
    return r;
  }

  double parseExpr() {
    double r = parseTerm();
    while (!err) {
      skipSpaces();
      char op = peek();
      if      (op == '+') { skip(); r += parseTerm(); }
      else if (op == '-') { skip(); r -= parseTerm(); }
      else break;
    }
    return r;
  }
}

// ---- Interface publique du parser ----
double evalExpr(const char* expr, bool& hasErr, char* errOut = nullptr) {
  MathParser::init(expr);
  double result = MathParser::parseExpr();
  hasErr = MathParser::err;
  if (hasErr && errOut)
    strncpy(errOut, MathParser::errMsg, STEP_MAX - 1);
  return result;
}

// ---- Substitution de 'x' ----
double evalAt(const char* expr, double xVal, bool& hasErr, char* errOut = nullptr) {
  char subExpr[EXPR_MAX * 3]; 
  char xStr[12];
  dtostrf(xVal, 1, 8, xStr);

  uint8_t j = 0;
  for (uint8_t i = 0; expr[i] && j < sizeof(subExpr) - 12; i++) {
    if (expr[i] == 'x' || expr[i] == 'X') {
      subExpr[j++] = '(';
      uint8_t k = 0;
      while (xStr[k] && j < sizeof(subExpr) - 2) subExpr[j++] = xStr[k++];
      subExpr[j++] = ')';
    } else {
      subExpr[j++] = expr[i];
    }
  }
  subExpr[j] = '\0';
  return evalExpr(subExpr, hasErr, errOut);
}

// ============================================================
//  FONCTIONS SCIENTIFIQUES
// ============================================================
double toRad(double a) { return a * PI / 180.0; }
double toDeg(double r) { return r * 180.0 / PI;  }

double applyFunc(char key, double val,
                 bool shift, bool hyp,
                 bool& valid, char* labelOut) {
  valid = true;
  double result = 0.0;
  const char* lbl = "?";

  double rad = (gState.angleMode == ANG_DEG) ? toRad(val) : val;

  switch (key) {
    case 'Q':
      if (shift) { lbl = "x2"; result = val * val; }
      else {
        lbl = "sqrt";
        if (val < 0) { valid = false; strncpy(labelOut, "sqrt: x<0!", STEP_MAX); return 0; }
        result = sqrt(val);
      }
      break;

    case 's':
      if (hyp) {
        if (shift) { lbl = "asinh"; result = asinh(val); }
        else       { lbl = "sinh";  result = sinh(val);  }
      } else {
        if (shift) {
          lbl = "arcsin";
          if (val < -1 || val > 1) { valid = false; strncpy(labelOut, "arcsin:|x|<=1!", STEP_MAX); return 0; }
          result = asin(val);
          if (gState.angleMode == ANG_DEG) result = toDeg(result);
        } else {
          lbl = "sin"; result = sin(rad);
        }
      }
      break;

    case 'd':
      if (hyp) {
        if (shift) {
          lbl = "acosh";
          if (val < 1) { valid = false; strncpy(labelOut, "acosh: x>=1!", STEP_MAX); return 0; }
          result = acosh(val);
        } else { lbl = "cosh"; result = cosh(val); }
      } else {
        if (shift) {
          lbl = "arccos";
          if (val < -1 || val > 1) { valid = false; strncpy(labelOut, "arccos:|x|<=1!", STEP_MAX); return 0; }
          result = acos(val);
          if (gState.angleMode == ANG_DEG) result = toDeg(result);
        } else {
          lbl = "cos"; result = cos(rad);
        }
      }
      break;

    case 't':
      if (hyp) {
        if (shift) {
          lbl = "atanh";
          if (val <= -1 || val >= 1) { valid = false; strncpy(labelOut, "atanh:|x|<1!", STEP_MAX); return 0; }
          result = atanh(val);
        } else { lbl = "tanh"; result = tanh(val); }
      } else {
        if (shift) {
          lbl = "arctan";
          result = atan(val);
          if (gState.angleMode == ANG_DEG) result = toDeg(result);
        } else {
          double cosV = cos(rad);
          if (abs(cosV) < 1e-10) { valid = false; strncpy(labelOut, "tan: indefini!", STEP_MAX); return 0; }
          lbl = "tan"; result = sin(rad) / cosV;
        }
      }
      break;

    case 'n':
      if (shift) { lbl = "10^x"; result = pow(10.0, val); }
      else {
        lbl = "log10";
        if (val <= 0) { valid = false; strncpy(labelOut, "log: x>0!", STEP_MAX); return 0; }
        result = log10(val);
      }
      break;

    case 'l':
      if (shift) { lbl = "e^x"; result = exp(val); }
      else {
        lbl = "ln";
        if (val <= 0) { valid = false; strncpy(labelOut, "ln: x>0!", STEP_MAX); return 0; }
        result = log(val);
      }
      break;

    case '^':
      lbl = "^"; result = val;
      break;

    default:
      valid = false;
      return 0;
  }

  strncpy(labelOut, lbl, STEP_MAX - 1);
  labelOut[STEP_MAX - 1] = '\0';
  return result;
}