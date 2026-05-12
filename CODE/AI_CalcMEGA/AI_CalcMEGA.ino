/*
 * ============================================================
 *  CENDERS IA - Calculatrice Scientifique v2.0
 *  Arduino MEGA | 2 Claviers | 2 Écrans LCD I2C DFRobot
 * ============================================================
 *  Clavier 1 (CALC) : Chiffres + Opérateurs de base
 *  Clavier 2 (SCI)  : Fonctions scientifiques avancées
 *
 *  FONCTIONS DISPONIBLES :
 *   - Addition, Soustraction, Multiplication, Division
 *   - Puissance (^), Parenthèses, Virgule décimale
 *   - SQRT (racine), x² (Shift + SQRT)
 *   - SIN, COS, TAN (et inverses avec Shift)
 *   - SINH, COSH, TANH (et inverses avec Shift + Hyp)
 *   - LOG base 10, 10^x (Shift + LOG)
 *   - LN, e^x (Shift + LN)
 *   - ANS (mémorisation dernier résultat)
 *   - Mode DEG / RAD
 *   - Shift, Hyp
 *   - Clear (C) et gestion des erreurs
 * ============================================================
 */

#include <Wire.h>
#include <Keypad.h>
#include <math.h>
#include "DFRobot_LCD.h"

// ============================================================
// CONSTANTES
// ============================================================
#define MODE_DEG 0
#define MODE_RAD 1
#define MAX_EXPR 64   // Longueur max de l'expression
// Définition manuelle des fonctions hyperboliques inverses (si non supportées)
double asinh(double x) { return log(x + sqrt(x * x + 1.0)); }
double acosh(double x) { return log(x + sqrt(x * x - 1.0)); }
double atanh(double x) { return 0.5 * log((1.0 + x) / (1.0 - x)); }
// ============================================================
// CLAVIER 1 — CALCUL DE BASE
// Broches : Lignes 22-25 | Colonnes 26-29
// ============================================================
const byte ROWS_C = 4;
const byte COLS_C = 4;
char keysCalcul[ROWS_C][COLS_C] = {
  {'7', '8', '9', '/'},
  {'4', '5', '6', '*'},
  {'1', '2', '3', '-'},
  {'C', '0', '=', '+'}
};
byte rowPinsCalc[ROWS_C] = {22, 23, 24, 25};
byte colPinsCalc[COLS_C] = {26, 27, 28, 29};
Keypad keypadCalc = Keypad(makeKeymap(keysCalcul), rowPinsCalc, colPinsCalc, ROWS_C, COLS_C);

// ============================================================
// CLAVIER 2 — SCIENTIFIQUE / IA
// Broches : Lignes 30-33 | Colonnes 34-37
// ============================================================
const byte ROWS_I = 4;
const byte COLS_I = 4;
// Q=sqrt  ^=puissance  (=parenthèse ouverte  )=espace→parenthèse fermée
// n=ln    l=log        .=décimal              A=ANS
// h=hyp   s=sin        d=cos                  t=tan
// M=mode  S=shift      <=retour arrière        >=effacer tout
char keysIA[ROWS_I][COLS_I] = {
  {'Q', '^', '(', ')'},
  {'n', 'l', '.', 'A'},
  {'h', 's', 'd', 't'},
  {'M', 'S', '<', '>'}
};
byte rowPinsIA[ROWS_I] = {30, 31, 32, 33};
byte colPinsIA[COLS_I] = {34, 35, 36, 37};
Keypad keypadIA = Keypad(makeKeymap(keysIA), rowPinsIA, colPinsIA, ROWS_I, COLS_I);

// ============================================================
// ÉCRANS LCD I2C
// lcd1 (0x7C) : Affichage expression/résultat
// lcd2 (0x27) : Affichage état/info fonctions
// ============================================================
DFRobot_LCD lcd1(0x27, 16, 2);
DFRobot_LCD lcd2(0x21, 16, 2);

// ============================================================
// VARIABLES GLOBALES
// ============================================================
String expression  = "";      // Expression en cours de saisie
double lastAnswer  = 0.0;     // Mémoire ANS (dernier résultat)
int    angleMode   = MODE_DEG;// Mode angulaire
bool   shiftActive = false;   // Touche Shift activée
bool   hypActive   = false;   // Touche Hyp activée
bool   newEntry    = false;   // Après un résultat : prochaine saisie = nouveau calcul
bool   errorState  = false;   // Erreur en cours

// ============================================================
// PROTOTYPES FONCTIONS
// ============================================================
double evalExpression(String expr);
String formatDouble(double val);
void   displayExpression();
void   displayResult(double result, String exprDisplay);
void   displayError(String msg);
void   displayInfo(String line1, String line2);
void   handleCalcKey(char key);
void   handleIAKey(char key);
double applyFunction(char func, double operand, bool isShift, bool isHyp, bool& valid, String& funcLabel);
double toRad(double angle);
double toDeg(double rad);

// ============================================================
// PARSER RÉCURSIF (Recursive Descent Parser)
// Gère : +  -  *  /  ^  ()
// ============================================================
namespace Parser {
  String  src;
  int     pos;
  bool    hasError;

  double parseExpr();
  double parseTerm();
  double parsePower();
  double parseUnary();
  double parsePrimary();

  void init(String s) {
    src      = s;
    pos      = 0;
    hasError = false;
  }

  char peek()  { return (pos < src.length()) ? src.charAt(pos) : '\0'; }
  char consume() { return src.charAt(pos++); }
  void skip()  { pos++; }

  double parsePrimary() {
    // Parenthèses
    if (peek() == '(') {
      skip(); // '('
      double val = parseExpr();
      if (peek() == ')') skip(); // ')'
      return val;
    }
    // Nombre
    double result = 0;
    bool   hasDot = false;
    double dec    = 0.1;
    bool   hasDigit = false;

    while (pos < src.length()) {
      char c = peek();
      if (c >= '0' && c <= '9') {
        hasDigit = true;
        if (hasDot) { result += (c - '0') * dec; dec *= 0.1; }
        else          result = result * 10.0 + (c - '0');
        skip();
      } else if (c == '.' && !hasDot) {
        hasDot = true;
        skip();
      } else {
        break;
      }
    }
    if (!hasDigit) hasError = true;
    return result;
  }

  double parseUnary() {
    if (peek() == '-') { skip(); return -parsePrimary(); }
    if (peek() == '+') { skip(); return  parsePrimary(); }
    return parsePrimary();
  }

  double parsePower() {
    double base = parseUnary();
    if (peek() == '^') {
      skip();
      double exp = parsePower(); // droite-associatif
      base = pow(base, exp);
    }
    return base;
  }

  double parseTerm() {
    double result = parsePower();
    while (pos < src.length()) {
      char op = peek();
      if (op == '*') { skip(); result *= parsePower(); }
      else if (op == '/') {
        skip();
        double d = parsePower();
        if (d == 0.0) { hasError = true; return 0; }
        result /= d;
      } else break;
    }
    return result;
  }

  double parseExpr() {
    double result = parseTerm();
    while (pos < src.length()) {
      char op = peek();
      if      (op == '+') { skip(); result += parseTerm(); }
      else if (op == '-') { skip(); result -= parseTerm(); }
      else break;
    }
    return result;
  }
}

double evalExpression(String expr) {
  // Nettoyer les espaces
  String clean = "";
  for (int i = 0; i < expr.length(); i++) {
    if (expr.charAt(i) != ' ') clean += expr.charAt(i);
  }
  Parser::init(clean);
  double result = Parser::parseExpr();
  if (Parser::hasError) { errorState = true; return 0; }
  return result;
}

// ============================================================
// SETUP
// ============================================================
void setup() {
  Wire.begin();
  delay(500);

  lcd1.init();
  lcd1.setRGB(255, 255, 255);
  lcd2.init();
  lcd2.setRGB(0, 150, 255);

  // Splash screen
  lcd1.clear();
  lcd1.setCursor(0, 0); lcd1.print("  CENDERS  IA  ");
  lcd1.setCursor(0, 1); lcd1.print(" Calculatrice ");

  lcd2.clear();
  lcd2.setCursor(0, 0); lcd2.print("  Version 2.0  ");
  lcd2.setCursor(0, 1); lcd2.print(" Mode: DEG     ");

  delay(2500);
  lcd1.clear();
  lcd2.clear();

  displayExpression();
  displayInfo("Pret", "Mode: DEG | [C]=Eff");
}

// ============================================================
// LOOP
// ============================================================
void loop() {
  char key1 = keypadCalc.getKey();
  char key2 = keypadIA.getKey();

  if (key1) handleCalcKey(key1);
  if (key2) handleIAKey(key2);
}

// ============================================================
// GESTION CLAVIER 1 — OPÉRATIONS DE BASE
// ============================================================
void handleCalcKey(char key) {
  // En état d'erreur : seul C est accepté
  if (errorState && key != 'C') {
    displayInfo("ERREUR!", "Appuyer [C] reset");
    return;
  }

  // --- CLEAR ---
  if (key == 'C') {
    expression = "";
    errorState = false;
    newEntry   = false;
    shiftActive = false;
    hypActive   = false;
    displayExpression();
    displayInfo("Efface", "Expression videe");
    return;
  }

  // --- EGAL ---
  if (key == '=') {
    if (expression.length() == 0) return;
    errorState = false;
    double result = evalExpression(expression);
    if (errorState) {
      displayError("Erreur calcul");
      return;
    }
    lastAnswer = result;
    displayResult(result, expression);
    newEntry = true;
    return;
  }

  // Après un résultat : si on appuie sur un opérateur → continuer avec ANS
  // Si chiffre → nouveau calcul
  if (newEntry) {
    bool isOperator = (key == '+' || key == '-' || key == '*' || key == '/');
    if (isOperator) {
      expression = formatDouble(lastAnswer);
    } else {
      expression = "";
    }
    newEntry = false;
  }

  // Limite de longueur
  if (expression.length() >= MAX_EXPR) {
    displayInfo("Trop long!", "Max " + String(MAX_EXPR) + " caracteres");
    return;
  }

  // Ajouter le caractère
  expression += key;
  displayExpression();
}

// ============================================================
// GESTION CLAVIER 2 — SCIENTIFIQUE
// ============================================================
void handleIAKey(char key) {

  // ---- COMMANDES DE CONTRÔLE ----

  // MODE (DEG ↔ RAD)
  if (key == 'M') {
    angleMode   = (angleMode == MODE_DEG) ? MODE_RAD : MODE_DEG;
    shiftActive = false;
    hypActive   = false;
    String modeStr = (angleMode == MODE_DEG) ? "DEG" : "RAD";
    displayInfo("Mode: " + modeStr, "Angles en " + modeStr);
    displayExpression();
    return;
  }

  // SHIFT
  if (key == 'S') {
    shiftActive = !shiftActive;
    if (shiftActive) hypActive = false;
    displayInfo(shiftActive ? "Shift: ACTIVE" : "Shift: OFF",
                shiftActive ? "Inverse actif" : "Normal");
    displayExpression();
    return;
  }

  // HYP
  if (key == 'h') {
    hypActive   = !hypActive;
    if (hypActive) shiftActive = false;
    displayInfo(hypActive ? "Hyp: ACTIVE" : "Hyp: OFF",
                hypActive ? "Fonctions hyp." : "Normal");
    displayExpression();
    return;
  }

  // ANS
  if (key == 'A') {
    if (newEntry) { expression = ""; newEntry = false; }
    expression += formatDouble(lastAnswer);
    displayExpression();
    displayInfo("ANS insere", "=" + formatDouble(lastAnswer));
    return;
  }

  // POINT DÉCIMAL
  if (key == '.') {
    if (newEntry) { expression = ""; newEntry = false; }
    if (expression.length() < MAX_EXPR) {
      expression += ".";
      displayExpression();
    }
    return;
  }

  // PARENTHÈSE OUVRANTE
  if (key == '(') {
    if (newEntry) { expression = ""; newEntry = false; }
    if (expression.length() < MAX_EXPR) {
      expression += "(";
      displayExpression();
      displayInfo("( insere", "Ouvrir parenthese");
    }
    return;
  }

  // PARENTHÈSE FERMANTE
  if (key == ')') {
    if (expression.length() < MAX_EXPR) {
      expression += ")";
      displayExpression();
      displayInfo(") insere", "Fermer parenthese");
    }
    return;
  }

  // PUISSANCE
  if (key == '^') {
    if (newEntry) { expression = ""; newEntry = false; }
    if (expression.length() < MAX_EXPR) {
      expression += "^";
      displayExpression();
      displayInfo("Puissance ^", "ex: 2^8 = 256");
    }
    return;
  }

  // RETOUR ARRIÈRE ('<')
  if (key == '<') {
    if (expression.length() > 0) {
      expression.remove(expression.length() - 1);
      displayExpression();
      displayInfo("Retour arriere", "Supprime 1 car.");
    }
    return;
  }

  // EFFACER TOUT ('>')
  if (key == '>') {
    expression  = "";
    errorState  = false;
    newEntry    = false;
    displayExpression();
    displayInfo("Tout efface", "");
    return;
  }

  // ---- FONCTIONS SCIENTIFIQUES ----
  // La fonction s'applique au dernier nombre de l'expression,
  // ou à lastAnswer si l'expression est vide / après un résultat.

  double operand = 0.0;

  if (newEntry || expression.length() == 0) {
    operand = lastAnswer;
  } else {
    // Extraire le dernier token numérique de l'expression
    int lastOp = -1;
    for (int i = expression.length() - 1; i >= 0; i--) {
      char c = expression.charAt(i);
      if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '(') {
        lastOp = i;
        break;
      }
    }
    String numStr = expression.substring(lastOp + 1);
    if (numStr.length() == 0) {
      displayInfo("Entrer un nombre", "avant la fonction");
      return;
    }
    operand = numStr.toDouble();
  }

  bool   valid     = true;
  String funcLabel = "";
  double result    = applyFunction(key, operand, shiftActive, hypActive, valid, funcLabel);

  if (!valid) return; // Message déjà affiché dans applyFunction

  // Mettre à jour l'expression et l'affichage
  lastAnswer  = result;
  newEntry    = true;
  shiftActive = false;
  hypActive   = false;

  // Remplacer l'expression par le résultat
  String resStr  = formatDouble(result);
  String exprSnap = expression;
  expression  = resStr;

  displayResult(result, funcLabel + "(" + formatDouble(operand) + ")");
  displayInfo(funcLabel + "=" + resStr,
              (angleMode == MODE_DEG ? "DEG" : "RAD"));
}

// ============================================================
// APPLICATION DES FONCTIONS SCIENTIFIQUES
// ============================================================
double applyFunction(char func, double operand, bool isShift, bool isHyp,
                     bool& valid, String& label) {
  double rad    = toRad(operand); // conversion si mode DEG
  double result = 0.0;
  valid = true;

  switch (func) {

    // ---------- RACINE / CARRÉ ----------
    case 'Q':
      if (isShift) {
        label  = "x^2";
        result = operand * operand;
      } else {
        label  = "sqrt";
        if (operand < 0) { displayError("sqrt negatif!"); valid = false; return 0; }
        result = sqrt(operand);
      }
      break;

    // ---------- SIN ----------
    case 's':
      if (isHyp) {
        if (isShift) { label = "asinh"; result = asinh(operand); }
        else         { label = "sinh";  result = sinh(operand);  }
      } else {
        if (isShift) {
          label = "arcsin";
          if (operand < -1 || operand > 1) { displayError("arcsin hors [-1,1]"); valid = false; return 0; }
          result = asin(operand);
          if (angleMode == MODE_DEG) result = toDeg(result);
        } else {
          label  = "sin";
          result = sin(angleMode == MODE_DEG ? rad : operand);
        }
      }
      break;

    // ---------- COS ----------
    case 'd':
      if (isHyp) {
        if (isShift) {
          label = "acosh";
          if (operand < 1) { displayError("acosh: x>=1 requis"); valid = false; return 0; }
          result = acosh(operand);
        } else { label = "cosh"; result = cosh(operand); }
      } else {
        if (isShift) {
          label = "arccos";
          if (operand < -1 || operand > 1) { displayError("arccos hors [-1,1]"); valid = false; return 0; }
          result = acos(operand);
          if (angleMode == MODE_DEG) result = toDeg(result);
        } else {
          label  = "cos";
          result = cos(angleMode == MODE_DEG ? rad : operand);
        }
      }
      break;

    // ---------- TAN ----------
    case 't':
      if (isHyp) {
        if (isShift) {
          label = "atanh";
          if (operand <= -1 || operand >= 1) { displayError("atanh: |x|<1 requis"); valid = false; return 0; }
          result = atanh(operand);
        } else { label = "tanh"; result = tanh(operand); }
      } else {
        if (isShift) {
          label  = "arctan";
          result = atan(operand);
          if (angleMode == MODE_DEG) result = toDeg(result);
        } else {
          label  = "tan";
          result = tan(angleMode == MODE_DEG ? rad : operand);
          // Vérification dénominateur nul (cos=0)
          double cosVal = cos(angleMode == MODE_DEG ? rad : operand);
          if (abs(cosVal) < 1e-10) { displayError("tan indefini!"); valid = false; return 0; }
        }
      }
      break;

    // ---------- LOG base 10 ----------
    case 'l':
      if (isShift) {
        label  = "10^x";
        result = pow(10.0, operand);
      } else {
        label  = "log10";
        if (operand <= 0) { displayError("log: x>0 requis"); valid = false; return 0; }
        result = log10(operand);
      }
      break;

    // ---------- LN (log naturel) ----------
    case 'n':
      if (isShift) {
        label  = "e^x";
        result = exp(operand);
      } else {
        label  = "ln";
        if (operand <= 0) { displayError("ln: x>0 requis"); valid = false; return 0; }
        result = log(operand);
      }
      break;

    default:
      valid = false;
      break;
  }
  return result;
}

// ============================================================
// UTILITAIRES MATHÉMATIQUES
// ============================================================
double toRad(double angle) { return angle * PI / 180.0; }
double toDeg(double rad)   { return rad   * 180.0 / PI; }

String formatDouble(double val) {
  if (isnan(val))  return "NaN";
  if (isinf(val))  return (val > 0) ? "+Inf" : "-Inf";

  // Si valeur entière dans plage raisonnable
  if (val == (long)val && abs(val) < 1e10) {
    return String((long)val);
  }
  // Arrondir à 6 chiffres significatifs
  if (abs(val) >= 1e6 || (abs(val) < 1e-3 && val != 0)) {
    // Notation scientifique approximative
    return String(val, 4) + "~";
  }
  return String(val, 6);
}

// ============================================================
// AFFICHAGE
// ============================================================
void displayExpression() {
  lcd1.clear();

  // Ligne 0 : expression (16 derniers caractères si trop long)
  lcd1.setCursor(0, 0);
  String disp = expression.length() <= 16
                ? expression
                : expression.substring(expression.length() - 16);
  lcd1.print(disp);

  // Ligne 1 : indicateurs d'état
  lcd1.setCursor(0, 1);
  String status = "[";
  status += (angleMode == MODE_DEG) ? "D" : "R";
  status += "]";
  if (shiftActive) status += " SH";
  if (hypActive)   status += " HY";
  if (!shiftActive && !hypActive) status += "       ";
  lcd1.print(status);
}

void displayResult(double result, String exprStr) {
  lcd1.clear();
  // Ligne 0 : expression tronquée
  lcd1.setCursor(0, 0);
  lcd1.print(exprStr.length() <= 16
             ? exprStr
             : exprStr.substring(exprStr.length() - 16));
  // Ligne 1 : résultat
  lcd1.setCursor(0, 1);
  String resStr = "=" + formatDouble(result);
  lcd1.print(resStr.substring(0, 16));

  // lcd2 : résultat détaillé
  lcd2.clear();
  lcd2.setCursor(0, 0); lcd2.print(">> Resultat:");
  lcd2.setCursor(0, 1);
  lcd2.print(formatDouble(result).substring(0, 16));
}

void displayError(String msg) {
  errorState  = true;
  expression  = "";
  shiftActive = false;
  hypActive   = false;

  lcd1.clear();
  lcd1.setRGB(255, 50, 50);
  lcd1.setCursor(0, 0); lcd1.print("!!! ERREUR !!!");
  lcd1.setCursor(0, 1); lcd1.print(msg.substring(0, 16));

  lcd2.clear();
  lcd2.setCursor(0, 0); lcd2.print(msg.substring(0, 16));
  lcd2.setCursor(0, 1); lcd2.print("Appuyer [C]");

  delay(500);
  lcd1.setRGB(255, 255, 255); // Remettre blanc
}

void displayInfo(String line1, String line2) {
  lcd2.clear();
  lcd2.setCursor(0, 0);
  lcd2.print(line1.substring(0, 16));
  if (line2.length() > 0) {
    lcd2.setCursor(0, 1);
    lcd2.print(line2.substring(0, 16));
  }
}
