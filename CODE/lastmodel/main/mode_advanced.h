#pragma once
// ============================================================
// CALCU IA — mode_advanced.h
//  MODE MATRIX : Addition, Soustraction, Multiplication,
//                Déterminant, Inverse (2x2 et 3x3)
//  MODE EQUATION : Linéaire (ax+b=0) et Quadratique (ax²+bx+c=0)
// ============================================================
#include "config.h"
#include "display_mgr.h"
#include "math_engine.h"

// ============================================================
//  MODE MATRICE
// ============================================================
#define MAT_SIZE_MAX 3
#define MAT_OP_ADD   0
#define MAT_OP_SUB   1
#define MAT_OP_MUL   2
#define MAT_OP_DET   3
#define MAT_OP_INV   4
#define MAT_OP_TRANS 5
#define MAT_OP_COUNT 6

const char* const MAT_OP_NAMES[MAT_OP_COUNT] PROGMEM = {
  "ADD", "SUB", "MUL", "DET", "INV", "TRA"
};

class MatrixMode {
public:
  double A[MAT_SIZE_MAX][MAT_SIZE_MAX];
  double B[MAT_SIZE_MAX][MAT_SIZE_MAX];
  double R[MAT_SIZE_MAX][MAT_SIZE_MAX];
  uint8_t matSize;   // 2 ou 3
  uint8_t matOp;     // MAT_OP_xxx
  uint8_t entryRow, entryCol;
  bool    enteringB; // false=A true=B
  char    numBuf[12];
  uint8_t numLen;

  void reset() {
    matSize = 2; matOp = MAT_OP_ADD;
    memset(A, 0, sizeof(A));
    memset(B, 0, sizeof(B));
    entryRow = 0; entryCol = 0;
    enteringB = false;
    numBuf[0] = '\0'; numLen = 0;
    _showHeader();
  }

  // ---- Clavier 1 ----
  void onCalcKey(char key) {
    if (key == 'C') { reset(); return; }

    // Saisie des chiffres et décimal/moins
    if ((key >= '0' && key <= '9') || key == '.') {
      if (numLen < 11) { numBuf[numLen++] = key; numBuf[numLen] = '\0'; }
      disp.showMatrixEntry(entryRow, entryCol, matSize, numBuf);
      return;
    }

    // Moins (pas disponible sur clavier 1 facilement, on utilise signe -)
    if (key == '-' && numLen == 0) {
      numBuf[numLen++] = '-'; numBuf[numLen] = '\0';
      disp.showMatrixEntry(entryRow, entryCol, matSize, numBuf);
      return;
    }

    // = → valider l'entrée courante
    if (key == '=') {
      _validateEntry();
      return;
    }

    // + → changer d'opération
    if (key == '+') {
      matOp = (matOp + 1) % MAT_OP_COUNT;
      _showHeader();
      return;
    }

    // / → changer taille matrice 2↔3
    if (key == '/') {
      matSize = (matSize == 2) ? 3 : 2;
      entryRow = 0; entryCol = 0; enteringB = false;
      memset(A, 0, sizeof(A)); memset(B, 0, sizeof(B));
      numBuf[0] = '\0'; numLen = 0;
      _showHeader();
      return;
    }
  }

  // ---- Clavier 2 ----
  void onSciKey(char key) {
    if (key == 'M') return; // géré dans main.ino
    if (key == 'B' || key == '<') { // backspace
      if (numLen > 0) { numBuf[--numLen] = '\0'; }
      disp.showMatrixEntry(entryRow, entryCol, matSize, numBuf);
    }
    if (key == '>') { reset(); }
    // Navigation < > entre cellules
    if (key == '<') { _navPrev(); }
  }

private:
  // ---- Valider la cellule courante et passer à la suivante ----
  void _validateEntry() {
    double val = atof(numBuf);
    if (enteringB) B[entryRow][entryCol] = val;
    else           A[entryRow][entryCol] = val;

    numBuf[0] = '\0'; numLen = 0;

    // Avancer
    entryCol++;
    if (entryCol >= matSize) { entryCol = 0; entryRow++; }
    if (entryRow >= matSize) {
      if (!enteringB && (matOp==MAT_OP_ADD || matOp==MAT_OP_SUB || matOp==MAT_OP_MUL)) {
        // Passer à la matrice B
        enteringB = true;
        entryRow = 0; entryCol = 0;
        lcd1.clear(); lcd1.setCursor(0,0); lcd1.print(F("Saisir Matrice B"));
        disp.showModeInfo();
      } else {
        // Calculer
        _compute();
        return;
      }
    }
    disp.showMatrixEntry(entryRow, entryCol, matSize, numBuf);
  }

  void _navPrev() {
    if (entryCol > 0) entryCol--;
    else if (entryRow > 0) { entryRow--; entryCol = matSize - 1; }
    disp.showMatrixEntry(entryRow, entryCol, matSize, numBuf);
  }

  // ---- Calcul selon l'opération ----
  void _compute() {
    switch (matOp) {
      case MAT_OP_ADD:   _addSub(1);    break;
      case MAT_OP_SUB:   _addSub(-1);   break;
      case MAT_OP_MUL:   _multiply();   break;
      case MAT_OP_DET:   _determinant(); break;
      case MAT_OP_INV:   _inverse();    break;
      case MAT_OP_TRANS: _transpose();  break;
    }
  }

  void _addSub(int sign) {
    for (uint8_t i = 0; i < matSize; i++)
      for (uint8_t j = 0; j < matSize; j++)
        R[i][j] = A[i][j] + sign * B[i][j];
    _showResult();
  }

  void _multiply() {
    for (uint8_t i = 0; i < matSize; i++)
      for (uint8_t j = 0; j < matSize; j++) {
        R[i][j] = 0;
        for (uint8_t k = 0; k < matSize; k++)
          R[i][j] += A[i][k] * B[k][j];
      }
    _showResult();
  }

  void _determinant() {
    double det = 0;
    if (matSize == 2) {
      det = A[0][0]*A[1][1] - A[0][1]*A[1][0];
    } else {
      det = A[0][0]*(A[1][1]*A[2][2]-A[1][2]*A[2][1])
           -A[0][1]*(A[1][0]*A[2][2]-A[1][2]*A[2][0])
           +A[0][2]*(A[1][0]*A[2][1]-A[1][1]*A[2][0]);
    }
    gState.ans = det;
    char buf[RES_MAX]; formatNum(det, buf);
    lcd1.clear();
    lcd1.setCursor(0,0); lcd1.print(F("det(A) ="));
    lcd1.setCursor(0,1); lcd1.print(buf);
    disp.showIA(F("Determinant calc"), buf);
  }

  void _inverse() {
    if (matSize == 2) {
      double det = A[0][0]*A[1][1] - A[0][1]*A[1][0];
      if (abs(det) < 1e-12) { disp.showError("det=0 : pas inv!"); return; }
      R[0][0] =  A[1][1]/det; R[0][1] = -A[0][1]/det;
      R[1][0] = -A[1][0]/det; R[1][1] =  A[0][0]/det;
      _showResult();
    } else {
      // Inverse 3x3 via cofacteurs
      double det = A[0][0]*(A[1][1]*A[2][2]-A[1][2]*A[2][1])
                  -A[0][1]*(A[1][0]*A[2][2]-A[1][2]*A[2][0])
                  +A[0][2]*(A[1][0]*A[2][1]-A[1][1]*A[2][0]);
      if (abs(det) < 1e-12) { disp.showError("det=0 : pas inv!"); return; }
      double inv[3][3];
      inv[0][0]= (A[1][1]*A[2][2]-A[1][2]*A[2][1])/det;
      inv[0][1]=-(A[0][1]*A[2][2]-A[0][2]*A[2][1])/det;
      inv[0][2]= (A[0][1]*A[1][2]-A[0][2]*A[1][1])/det;
      inv[1][0]=-(A[1][0]*A[2][2]-A[1][2]*A[2][0])/det;
      inv[1][1]= (A[0][0]*A[2][2]-A[0][2]*A[2][0])/det;
      inv[1][2]=-(A[0][0]*A[1][2]-A[0][2]*A[1][0])/det;
      inv[2][0]= (A[1][0]*A[2][1]-A[1][1]*A[2][0])/det;
      inv[2][1]=-(A[0][0]*A[2][1]-A[0][1]*A[2][0])/det;
      inv[2][2]= (A[0][0]*A[1][1]-A[0][1]*A[1][0])/det;
      memcpy(R, inv, sizeof(inv));
      _showResult();
    }
  }

  void _transpose() {
    for (uint8_t i = 0; i < matSize; i++)
      for (uint8_t j = 0; j < matSize; j++)
        R[i][j] = A[j][i];
    _showResult();
  }

  void _showResult() {
    lcd1.clear(); lcd2.clear();
    lcd1.setCursor(0,0); lcd1.print(F("Resultat:"));
    // Afficher ligne par ligne en alternant sur LCD1 et LCD2
    char buf[STEP_MAX];
    for (uint8_t i = 0; i < matSize; i++) {
      String row = "[";
      for (uint8_t j = 0; j < matSize; j++) {
        char nb[8]; formatNum(R[i][j], nb, 8);
        row += nb;
        if (j < matSize-1) row += " ";
      }
      row += "]";
      if (i == 0) {
        lcd1.setCursor(0, 1); lcd1.print(row.substring(0,16));
      } else if (i == 1) {
        lcd2.setCursor(0, 0); lcd2.print(row.substring(0,16));
      } else {
        lcd2.setCursor(0, 1); lcd2.print(row.substring(0,16));
      }
    }
    gState.newEntry = true;
  }

  void _showHeader() {
    lcd1.clear();
    lcd1.setCursor(0,0); lcd1.print(F("MODE MATRICE "));
    lcd1.print(matSize); lcd1.print(F("x")); lcd1.print(matSize);
    lcd2.clear();
    lcd2.setCursor(0,0); lcd2.print(F("Op: "));
    lcd2.print(MAT_OP_NAMES[matOp]);
    lcd2.setCursor(0,1); lcd2.print(F("[+]op [/]taille"));
    disp.showMatrixEntry(entryRow, entryCol, matSize, numBuf);
  }
};

// ============================================================
//  MODE ÉQUATION
// ============================================================
#define EQ_LINEAR 0
#define EQ_QUAD   1

class EquationMode {
public:
  uint8_t eqType;  // EQ_LINEAR ou EQ_QUAD
  double  a, b, c;
  uint8_t step;    // 0=saisie a, 1=saisie b, 2=saisie c
  char    numBuf[12];
  uint8_t numLen;

  void reset() {
    eqType = EQ_LINEAR; a = b = c = 0;
    step = 0; numBuf[0]='\0'; numLen=0;
    _showPrompt();
  }

  void onCalcKey(char key) {
    if (key == 'C') { reset(); return; }

    if ((key>='0'&&key<='9') || key=='.') {
      if (numLen < 11) { numBuf[numLen++]=key; numBuf[numLen]='\0'; }
      disp.showEquation(eqType==EQ_QUAD?2:1, numBuf, step);
      return;
    }

    if (key == '-' && numLen == 0) {
      numBuf[numLen++]='-'; numBuf[numLen]='\0';
      disp.showEquation(eqType==EQ_QUAD?2:1, numBuf, step);
      return;
    }

    if (key == '=') { _validateStep(); return; }

    // '+' → changer type équation
    if (key == '+') {
      eqType = (eqType == EQ_LINEAR) ? EQ_QUAD : EQ_LINEAR;
      step = 0; numBuf[0]='\0'; numLen=0;
      _showPrompt();
    }
  }

  void onSciKey(char key) {
    if (key == 'M') return;
    if (key == 'B' || key == '<') {
      if (numLen > 0) { numBuf[--numLen]='\0'; }
      disp.showEquation(eqType==EQ_QUAD?2:1, numBuf, step);
    }
    if (key == '>') { reset(); }
  }

private:
  void _validateStep() {
    double val = atof(numBuf);
    if (step == 0) a = val;
    else if (step == 1) b = val;
    else c = val;
    numBuf[0]='\0'; numLen=0;
    step++;
    uint8_t maxStep = (eqType==EQ_LINEAR) ? 2 : 3;
    if (step >= maxStep) { _solve(); }
    else { disp.showEquation(eqType==EQ_QUAD?2:1, numBuf, step); }
  }

  void _solve() {
    lcd1.clear(); lcd2.clear();
    if (eqType == EQ_LINEAR) {
      // ax + b = 0 → x = -b/a
      if (abs(a) < 1e-12) {
        disp.showError("a=0: pas eq!   ");
        return;
      }
      double x = -b / a;
      char xs[RES_MAX]; formatNum(x, xs);
      lcd1.setCursor(0,0); lcd1.print(F("ax+b=0 Solution:"));
      lcd1.setCursor(0,1); lcd1.print(F("x="));
      lcd1.print(xs);
      disp.showIA(F("Lineaire ok"), xs);
      // LCD2 : étapes
      _showLinearSteps(x);
    } else {
      // ax² + bx + c = 0 → discriminant
      if (abs(a) < 1e-12) {
        disp.showError("a=0: lineaire!  ");
        return;
      }
      double disc = b*b - 4*a*c;
      lcd1.setCursor(0,0); lcd1.print(F("ax2+bx+c=0      "));
      char ds[RES_MAX]; formatNum(disc, ds);
      if (disc < 0) {
        lcd1.setCursor(0,1); lcd1.print(F("D<0: pas solution"));
        lcd2.setCursor(0,0); lcd2.print(F("Discriminant D="));
        lcd2.setCursor(0,1); lcd2.print(ds);
      } else if (disc == 0) {
        double x0 = -b / (2*a);
        char xs[RES_MAX]; formatNum(x0, xs);
        lcd1.setCursor(0,1); lcd1.print(F("x1=x2= "));
        lcd1.print(xs);
        lcd2.setCursor(0,0); lcd2.print(F("D=0 double racine"));
        lcd2.setCursor(0,1); lcd2.print(xs);
      } else {
        double sqrtD = sqrt(disc);
        double x1 = (-b + sqrtD)/(2*a);
        double x2 = (-b - sqrtD)/(2*a);
        char x1s[RES_MAX], x2s[RES_MAX];
        formatNum(x1, x1s); formatNum(x2, x2s);
        lcd1.setCursor(0,0); lcd1.print(F("x1="));
        for(int i=0;i<13&&x1s[i];i++) lcd1.print(x1s[i]);
        lcd1.setCursor(0,1); lcd1.print(F("x2="));
        for(int i=0;i<13&&x2s[i];i++) lcd1.print(x2s[i]);
        lcd2.setCursor(0,0);
        lcd2.print(F("D="));
        lcd2.print(ds);
        lcd2.setCursor(0,1);
        lcd2.print(F("2 solutions OK"));
      }
    }
    gState.newEntry = true;
  }

  void _showLinearSteps(double x) {
    // Affichage sur LCD2 des étapes de résolution
    lcd2.clear();
    lcd2.setCursor(0,0); lcd2.print(F("ax+b=0 -> x=-b/a"));
    char buf[STEP_MAX];
    snprintf(buf, STEP_MAX, "=-%s/%s", _fmt(b), _fmt(a));
    lcd2.setCursor(0,1);
    for(int i=0;i<16&&buf[i];i++) lcd2.print(buf[i]);
  }

  char* _fmt(double v) {
    static char fb[8]; formatNum(v, fb, 8); return fb;
  }

  void _showPrompt() {
    lcd1.clear();
    lcd1.setCursor(0,0);
    lcd1.print(eqType==EQ_LINEAR ? F("Eq. ax+b=0    ") : F("Eq. ax2+bx+c=0"));
    lcd1.setCursor(0,1); lcd1.print(F("Saisir a=?     "));
    lcd2.clear();
    lcd2.setCursor(0,0); lcd2.print(F("[=]:confirmer  "));
    lcd2.setCursor(0,1); lcd2.print(F("[+]:changer type"));
  }
};

extern MatrixMode  matrixMode;
extern EquationMode equMode;
