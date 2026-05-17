#pragma once
// ============================================================
//  CALCU IA — ia_brain.h
//  Moteur d'explication intelligent (Rule-Based AI)
//
//  Ce moteur analyse le contexte de calcul et génère
//  des explications contextuelles, éducatives et interactives.
//  Tout le contenu textuel est stocké en PROGMEM (~12 KB Flash)
//  pour préserver les 8 KB de SRAM de l'Arduino Mega.
//
//  FONCTIONNALITÉS :
//   1. Analyse de l'expression/résultat → explication auto
//   2. Navigateur de 20 concepts mathématiques
//   3. Guide interactif étape par étape (5 parcours)
//   4. Détection des propriétés du résultat (entier, premier, etc.)
// ============================================================
#include "config.h"
#include "display_mgr.h"
#include "math_engine.h"

// ============================================================
//  BASE DE CONNAISSANCES EN PROGMEM
//  Format : séries de lignes de 16 chars max, sentinelle ""
// ============================================================

// ---------- 20 CONCEPTS ----------
// Chaque concept : titre (10c) + 6 lignes explicatives

// 0 - SIN
const char C0_T[] PROGMEM = "SINUS           ";
const char C0_0[] PROGMEM = "sin(x): oscille ";
const char C0_1[] PROGMEM = "entre -1 et +1  ";
const char C0_2[] PROGMEM = "sin(0)=0        ";
const char C0_3[] PROGMEM = "sin(30deg)=0.5  ";
const char C0_4[] PROGMEM = "sin(90deg)=1    ";
const char C0_5[] PROGMEM = "Derivee: cos(x) ";
const char C0_6[] PROGMEM = "";

// 1 - COS
const char C1_T[] PROGMEM = "COSINUS         ";
const char C1_0[] PROGMEM = "cos(x): oscille ";
const char C1_1[] PROGMEM = "entre -1 et +1  ";
const char C1_2[] PROGMEM = "cos(0)=1        ";
const char C1_3[] PROGMEM = "cos(90deg)=0    ";
const char C1_4[] PROGMEM = "cos(PI)=-1      ";
const char C1_5[] PROGMEM = "Deriv:-sin(x)   ";
const char C1_6[] PROGMEM = "";

// 2 - TAN
const char C2_T[] PROGMEM = "TANGENTE        ";
const char C2_0[] PROGMEM = "tan=sin(x)/cos  ";
const char C2_1[] PROGMEM = "Indéfini: cos=0 ";
const char C2_2[] PROGMEM = "tan(45deg)=1    ";
const char C2_3[] PROGMEM = "tan(0)=0        ";
const char C2_4[] PROGMEM = "Periode: PI     ";
const char C2_5[] PROGMEM = "Deriv:1/cos^2   ";
const char C2_6[] PROGMEM = "";

// 3 - LN
const char C3_T[] PROGMEM = "LOGARITHME LN   ";
const char C3_0[] PROGMEM = "ln(x): x > 0   ";
const char C3_1[] PROGMEM = "ln(1)=0         ";
const char C3_2[] PROGMEM = "ln(e)=1 (e=2.71)";
const char C3_3[] PROGMEM = "ln(ab)=lna+lnb  ";
const char C3_4[] PROGMEM = "ln(a^n)=n*ln(a) ";
const char C3_5[] PROGMEM = "Derivee: 1/x    ";
const char C3_6[] PROGMEM = "";

// 4 - LOG10
const char C4_T[] PROGMEM = "LOGARITHME LOG10";
const char C4_0[] PROGMEM = "Base 10         ";
const char C4_1[] PROGMEM = "log(1)=0        ";
const char C4_2[] PROGMEM = "log(10)=1       ";
const char C4_3[] PROGMEM = "log(100)=2      ";
const char C4_4[] PROGMEM = "log=ln(x)/ln(10)";
const char C4_5[] PROGMEM = "Deriv:1/(x*ln10)";
const char C4_6[] PROGMEM = "";

// 5 - SQRT
const char C5_T[] PROGMEM = "RACINE CARREE   ";
const char C5_0[] PROGMEM = "sqrt(x): x>=0   ";
const char C5_1[] PROGMEM = "sqrt(4)=2       ";
const char C5_2[] PROGMEM = "sqrt(x)=x^(1/2) ";
const char C5_3[] PROGMEM = "sqrt(a*b)=sqrta ";
const char C5_4[] PROGMEM = "          *sqrtb";
const char C5_5[] PROGMEM = "Deriv:1/(2sqrtx)";
const char C5_6[] PROGMEM = "";

// 6 - PUISSANCE
const char C6_T[] PROGMEM = "PUISSANCE       ";
const char C6_0[] PROGMEM = "a^n: a*a*...*a  ";
const char C6_1[] PROGMEM = "  (n fois)      ";
const char C6_2[] PROGMEM = "a^0 = 1         ";
const char C6_3[] PROGMEM = "a^(-n)=1/(a^n)  ";
const char C6_4[] PROGMEM = "2^10=1024       ";
const char C6_5[] PROGMEM = "Deriv:n*x^(n-1) ";
const char C6_6[] PROGMEM = "";

// 7 - LIMITE
const char C7_T[] PROGMEM = "LIMITE          ";
const char C7_0[] PROGMEM = "lim x->a f(x)=L ";
const char C7_1[] PROGMEM = "Gauche: x->a-   ";
const char C7_2[] PROGMEM = "Droite: x->a+   ";
const char C7_3[] PROGMEM = "Exist si G==D   ";
const char C7_4[] PROGMEM = "ex: lim x->0 x/x";
const char C7_5[] PROGMEM = "=1 (form.indet.)";
const char C7_6[] PROGMEM = "";

// 8 - DERIVEE
const char C8_T[] PROGMEM = "DERIVEE         ";
const char C8_0[] PROGMEM = "f'(x0): pente   ";
const char C8_1[] PROGMEM = "de la tangente  ";
const char C8_2[] PROGMEM = "f'~[f(x+h)-     ";
const char C8_3[] PROGMEM = " f(x-h)]/(2h)   ";
const char C8_4[] PROGMEM = "(x^n)'=n*x^(n-1)";
const char C8_5[] PROGMEM = "(uv)'=u'v+uv'   ";
const char C8_6[] PROGMEM = "";

// 9 - INTEGRALE
const char C9_T[] PROGMEM = "INTEGRALE       ";
const char C9_0[] PROGMEM = "Aire sous f(x)  ";
const char C9_1[] PROGMEM = "entre a et b    ";
const char C9_2[] PROGMEM = "Methode Simpson ";
const char C9_3[] PROGMEM = "n=100 tranches  ";
const char C9_4[] PROGMEM = "Primitiv F: F'=f";
const char C9_5[] PROGMEM = "Int=F(b)-F(a)   ";
const char C9_6[] PROGMEM = "";

// 10 - MATRICE
const char C10_T[] PROGMEM = "MATRICE         ";
const char C10_0[] PROGMEM = "Tableau n x m   ";
const char C10_1[] PROGMEM = "de nombres      ";
const char C10_2[] PROGMEM = "det(A): scalaire";
const char C10_3[] PROGMEM = "det=0: non inv. ";
const char C10_4[] PROGMEM = "A*A^-1 = I      ";
const char C10_5[] PROGMEM = "(I: identite)   ";
const char C10_6[] PROGMEM = "";

// 11 - EQUATION
const char C11_T[] PROGMEM = "EQUATION        ";
const char C11_0[] PROGMEM = "Trouver x tel   ";
const char C11_1[] PROGMEM = "que f(x)=0      ";
const char C11_2[] PROGMEM = "Lineaire:ax+b=0 ";
const char C11_3[] PROGMEM = "-> x=-b/a       ";
const char C11_4[] PROGMEM = "Quadrat: D=b2-4c";
const char C11_5[] PROGMEM = "x=(-b+-sqrtD)/2a";
const char C11_6[] PROGMEM = "";

// 12 - CONTINUITE
const char C12_T[] PROGMEM = "CONTINUITE      ";
const char C12_0[] PROGMEM = "f continue en a ";
const char C12_1[] PROGMEM = "si lim x->a f=f(a)";
const char C12_2[] PROGMEM = "Pas de saut     ";
const char C12_3[] PROGMEM = "ni de trou      ";
const char C12_4[] PROGMEM = "Polynomes:tjrs  ";
const char C12_5[] PROGMEM = "continus        ";
const char C12_6[] PROGMEM = "";

// 13 - ASYMPTOTE
const char C13_T[] PROGMEM = "ASYMPTOTE       ";
const char C13_0[] PROGMEM = "Droite limite   ";
const char C13_1[] PROGMEM = "Verticale: x=a  ";
const char C13_2[] PROGMEM = "si f->inf en a  ";
const char C13_3[] PROGMEM = "Horiz: y=L si   ";
const char C13_4[] PROGMEM = "f(x)->L x->inf  ";
const char C13_5[] PROGMEM = "ex: 1/x -> y=0  ";
const char C13_6[] PROGMEM = "";

// 14 - VECTEUR
const char C14_T[] PROGMEM = "VECTEUR         ";
const char C14_0[] PROGMEM = "Quantite: dir.  ";
const char C14_1[] PROGMEM = "+ sens + norme  ";
const char C14_2[] PROGMEM = "||v||=sqrt(x2+..)";
const char C14_3[] PROGMEM = "Prod scal:u.v=  ";
const char C14_4[] PROGMEM = "||u||*||v||*cos ";
const char C14_5[] PROGMEM = "Ortho si u.v=0  ";
const char C14_6[] PROGMEM = "";

// 15 - PROBABILITE
const char C15_T[] PROGMEM = "PROBABILITE     ";
const char C15_0[] PROGMEM = "P(A): 0 <= P<=1 ";
const char C15_1[] PROGMEM = "P(A ou B)=P(A)+ ";
const char C15_2[] PROGMEM = " P(B)-P(AnB)    ";
const char C15_3[] PROGMEM = "P(A|B)=P(AnB)/  ";
const char C15_4[] PROGMEM = "       P(B)     ";
const char C15_5[] PROGMEM = "Indep: P(A|B)=  ";
const char C15_6[] PROGMEM = "       P(A)     ";

// 16 - SUITE
const char C16_T[] PROGMEM = "SUITES          ";
const char C16_0[] PROGMEM = "Arith.: u(n+1)= ";
const char C16_1[] PROGMEM = "u(n)+r          ";
const char C16_2[] PROGMEM = "Geo.: u(n+1)=   ";
const char C16_3[] PROGMEM = "u(n)*q          ";
const char C16_4[] PROGMEM = "Somme arith.:   ";
const char C16_5[] PROGMEM = "n*(u0+un)/2     ";
const char C16_6[] PROGMEM = "";

// 17 - COMPLEXES
const char C17_T[] PROGMEM = "NB COMPLEXES    ";
const char C17_0[] PROGMEM = "z = a + b*i     ";
const char C17_1[] PROGMEM = "i^2 = -1        ";
const char C17_2[] PROGMEM = "|z|=sqrt(a2+b2) ";
const char C17_3[] PROGMEM = "Arg: theta=     ";
const char C17_4[] PROGMEM = "  atan(b/a)     ";
const char C17_5[] PROGMEM = "z=|z|*e^(i*tht) ";
const char C17_6[] PROGMEM = "";

// 18 - TRIGONOMETRIE
const char C18_T[] PROGMEM = "TRIGO (cercle)  ";
const char C18_0[] PROGMEM = "sin2+cos2=1     ";
const char C18_1[] PROGMEM = "sin(a+b)=sin(a)*";
const char C18_2[] PROGMEM = "cos(b)+cos(a)*  ";
const char C18_3[] PROGMEM = "sin(b)          ";
const char C18_4[] PROGMEM = "sin(2a)=        ";
const char C18_5[] PROGMEM = "2*sin(a)*cos(a) ";
const char C18_6[] PROGMEM = "";

// 19 - EXPONENTIELLE
const char C19_T[] PROGMEM = "EXPONENTIELLE   ";
const char C19_0[] PROGMEM = "e^x : toujours+ ";
const char C19_1[] PROGMEM = "e^0=1  e^1=2.71 ";
const char C19_2[] PROGMEM = "e^(a+b)=e^a*e^b ";
const char C19_3[] PROGMEM = "Derivee: e^x    ";
const char C19_4[] PROGMEM = "(elle-meme!)    ";
const char C19_5[] PROGMEM = "lim x->-inf = 0 ";
const char C19_6[] PROGMEM = "";

#define CONCEPT_COUNT 20
#define CONCEPT_LINES  7  // 6 lignes + sentinelle

// Tables de pointeurs PROGMEM
const char* const CONCEPT_TITLES[CONCEPT_COUNT] PROGMEM = {
  C0_T,C1_T,C2_T,C3_T,C4_T,C5_T,C6_T,C7_T,C8_T,C9_T,
  C10_T,C11_T,C12_T,C13_T,C14_T,C15_T,C16_T,C17_T,C18_T,C19_T
};
const char* const CONCEPT_LINES_0[CONCEPT_COUNT] PROGMEM = {
  C0_0,C1_0,C2_0,C3_0,C4_0,C5_0,C6_0,C7_0,C8_0,C9_0,
  C10_0,C11_0,C12_0,C13_0,C14_0,C15_0,C16_0,C17_0,C18_0,C19_0
};
const char* const CONCEPT_LINES_1[CONCEPT_COUNT] PROGMEM = {
  C0_1,C1_1,C2_1,C3_1,C4_1,C5_1,C6_1,C7_1,C8_1,C9_1,
  C10_1,C11_1,C12_1,C13_1,C14_1,C15_1,C16_1,C17_1,C18_1,C19_1
};
const char* const CONCEPT_LINES_2[CONCEPT_COUNT] PROGMEM = {
  C0_2,C1_2,C2_2,C3_2,C4_2,C5_2,C6_2,C7_2,C8_2,C9_2,
  C10_2,C11_2,C12_2,C13_2,C14_2,C15_2,C16_2,C17_2,C18_2,C19_2
};
const char* const CONCEPT_LINES_3[CONCEPT_COUNT] PROGMEM = {
  C0_3,C1_3,C2_3,C3_3,C4_3,C5_3,C6_3,C7_3,C8_3,C9_3,
  C10_3,C11_3,C12_3,C13_3,C14_3,C15_3,C16_3,C17_3,C18_3,C19_3
};
const char* const CONCEPT_LINES_4[CONCEPT_COUNT] PROGMEM = {
  C0_4,C1_4,C2_4,C3_4,C4_4,C5_4,C6_4,C7_4,C8_4,C9_4,
  C10_4,C11_4,C12_4,C13_4,C14_4,C15_4,C16_4,C17_4,C18_4,C19_4
};
const char* const CONCEPT_LINES_5[CONCEPT_COUNT] PROGMEM = {
  C0_5,C1_5,C2_5,C3_5,C4_5,C5_5,C6_5,C7_5,C8_5,C9_5,
  C10_5,C11_5,C12_5,C13_5,C14_5,C15_5,C16_5,C17_5,C18_5,C19_5
};

// ============================================================
//  GUIDES INTERACTIFS (5 parcours)
// ============================================================
// Guide 0 : Comprendre les limites
const char G0_0[] PROGMEM = "QU'EST-CE QU'UNE";
const char G0_1[] PROGMEM = "LIMITE ?        ";
const char G0_2[] PROGMEM = "Imaginons x->2  ";
const char G0_3[] PROGMEM = "dans f(x)=x*x   ";
const char G0_4[] PROGMEM = "x=1.9: f=3.61   ";
const char G0_5[] PROGMEM = "x=1.99: f=3.96  ";
const char G0_6[] PROGMEM = "x=1.999: f=3.996";
const char G0_7[] PROGMEM = "=> lim x->2 =4  ";
const char G0_8[] PROGMEM = "f(2)=4: continu!";
const char G0_9[] PROGMEM = "[>] pour suite  ";
const char G0_10[] PROGMEM= "";

// Guide 1 : Comprendre la dérivée
const char G1_0[] PROGMEM = "QU'EST-CE QU'UNE";
const char G1_1[] PROGMEM = "DERIVEE ?       ";
const char G1_2[] PROGMEM = "C'est la pente  ";
const char G1_3[] PROGMEM = "de la courbe    ";
const char G1_4[] PROGMEM = "f(x)=x^2: pente ";
const char G1_5[] PROGMEM = "en x=3: f'(3)=6 ";
const char G1_6[] PROGMEM = "Si f'(x)>0:     ";
const char G1_7[] PROGMEM = "f est croissante";
const char G1_8[] PROGMEM = "Si f'(x)=0: max ";
const char G1_9[] PROGMEM = "ou min local    ";
const char G1_10[] PROGMEM= "";

// Guide 2 : Comprendre l'intégrale
const char G2_0[] PROGMEM = "QU'EST-CE QU'UNE";
const char G2_1[] PROGMEM = "INTEGRALE ?     ";
const char G2_2[] PROGMEM = "L'aire sous f(x)";
const char G2_3[] PROGMEM = "entre a et b    ";
const char G2_4[] PROGMEM = "ex: int(x^2,0,3)";
const char G2_5[] PROGMEM = "On decoupe en   ";
const char G2_6[] PROGMEM = "100 rectangles  ";
const char G2_7[] PROGMEM = "On somme l'aire ";
const char G2_8[] PROGMEM = "=> 9 (exact)    ";
const char G2_9[] PROGMEM = "Primitiv: x^3/3 ";
const char G2_10[] PROGMEM= "";

// Guide 3 : Les fonctions trig
const char G3_0[] PROGMEM = "TRIGO FACILE    ";
const char G3_1[] PROGMEM = "Cercle de r=1   ";
const char G3_2[] PROGMEM = "Point M: angle t";
const char G3_3[] PROGMEM = "M=(cos t, sin t)";
const char G3_4[] PROGMEM = "0deg: (1,0)     ";
const char G3_5[] PROGMEM = "90deg: (0,1)    ";
const char G3_6[] PROGMEM = "180deg: (-1,0)  ";
const char G3_7[] PROGMEM = "270deg: (0,-1)  ";
const char G3_8[] PROGMEM = "sin2+cos2=1     ";
const char G3_9[] PROGMEM = "TOUJOURS VRAI!  ";
const char G3_10[] PROGMEM= "";

// Guide 4 : Matrices
const char G4_0[] PROGMEM = "MATRICES FACILE ";
const char G4_1[] PROGMEM = "Tableau de nbres";
const char G4_2[] PROGMEM = "[a b]           ";
const char G4_3[] PROGMEM = "[c d]  = M 2x2  ";
const char G4_4[] PROGMEM = "det(M)=a*d-b*c  ";
const char G4_5[] PROGMEM = "Si det=0:       ";
const char G4_6[] PROGMEM = "pas d'inverse   ";
const char G4_7[] PROGMEM = "M*M^-1=Identite ";
const char G4_8[] PROGMEM = "[1 0]           ";
const char G4_9[] PROGMEM = "[0 1]           ";
const char G4_10[] PROGMEM= "";

#define GUIDE_COUNT  5
#define GUIDE_LINES  11

const char* const GUIDES[GUIDE_COUNT][GUIDE_LINES] PROGMEM = {
  {G0_0,G0_1,G0_2,G0_3,G0_4,G0_5,G0_6,G0_7,G0_8,G0_9,G0_10},
  {G1_0,G1_1,G1_2,G1_3,G1_4,G1_5,G1_6,G1_7,G1_8,G1_9,G1_10},
  {G2_0,G2_1,G2_2,G2_3,G2_4,G2_5,G2_6,G2_7,G2_8,G2_9,G2_10},
  {G3_0,G3_1,G3_2,G3_3,G3_4,G3_5,G3_6,G3_7,G3_8,G3_9,G3_10},
  {G4_0,G4_1,G4_2,G4_3,G4_4,G4_5,G4_6,G4_7,G4_8,G4_9,G4_10}
};
// Noms des guides en PROGMEM (F() interdit hors fonction)
const char GN_0[] PROGMEM = "LIMITES";
const char GN_1[] PROGMEM = "DERIVEES";
const char GN_2[] PROGMEM = "INTEGRALES";
const char GN_3[] PROGMEM = "TRIGONOMETRIE";
const char GN_4[] PROGMEM = "MATRICES";
const char* const GUIDE_NAMES[GUIDE_COUNT] PROGMEM = {
  GN_0, GN_1, GN_2, GN_3, GN_4
};

// ============================================================
//  ANALYSE DU RÉSULTAT (réponses contextuelles)
// ============================================================
// Détecte les propriétés du résultat et génère un commentaire
const char PROP_ZERO[]     PROGMEM = "Resultat = 0    ";
const char PROP_ONE[]      PROGMEM = "Resultat = 1    ";
const char PROP_NEG[]      PROGMEM = "Resultat negatif";
const char PROP_BIG[]      PROGMEM = "Grand nombre!   ";
const char PROP_SMALL[]    PROGMEM = "Tres petit (<1) ";
const char PROP_INT[]      PROGMEM = "Entier exact!   ";
const char PROP_APPROX[]   PROGMEM = "Valeur approchee";
const char PROP_PRIME[]    PROGMEM = "Nombre PREMIER! ";
const char PROP_SQRT2[]    PROGMEM = "~sqrt(2)=1.4142 ";
const char PROP_PI[]       PROGMEM = "~PI=3.14159...  ";
const char PROP_E[]        PROGMEM = "~e=2.71828...   ";
const char PROP_GOLDEN[]   PROGMEM = "~Nb d'or=1.618  ";

// ============================================================
//  CLASSE IABrain
// ============================================================
class IABrain {
public:
  // --- Navigateur de concepts ---
  uint8_t  conceptIdx;     // concept courant (0-19)
  uint8_t  conceptLine;    // ligne courante dans le concept
  bool     conceptScroll;  // défilement auto actif
  unsigned long lastScroll;
  #define BRAIN_SCROLL_MS 1500u

  // --- Guide interactif ---
  uint8_t  guideIdx;       // guide sélectionné (0-4)
  uint8_t  guideLine;      // ligne courante dans le guide
  bool     inGuideSelect;  // sélection du guide ou lecture?

  // --- Mode actif ---
  // IA_CONCEPT : navigateur de concepts
  // IA_GUIDE   : guide interactif

  void reset() {
    conceptIdx    = 0;
    conceptLine   = 0;
    conceptScroll = false;
    lastScroll    = 0;
    guideIdx      = 0;
    guideLine     = 0;
    inGuideSelect = true;
    gState.shift  = false;
    gState.hyp    = false;

    if (gState.iaSubMode == IA_CONCEPT) {
      _displayConcept();
    } else if (gState.iaSubMode == IA_GUIDE) {
      _displayGuideSelect();
    }
  }

  // ================================================================
  //  UPDATE — appelé depuis loop()
  // ================================================================
  void update() {
    if (!conceptScroll) return;
    unsigned long now = millis();
    if (now - lastScroll >= BRAIN_SCROLL_MS) {
      lastScroll = now;
      _nextConceptLine();
    }
  }

  // ================================================================
  //  Analyse d'un résultat de calcul → afficher propriétés sur LCD2
  // ================================================================
  void analyzeResult(const char* expr, double result) {
    strncpy(gState.lastExpr, expr, EXPR_MAX - 1);
    gState.lastResult = result;

    char propBuf[STEP_MAX];
    _detectProperty(result, propBuf);

    lcd2.clear();
    lcd2.setCursor(0, 0);
    lcd2.print(F("IA: "));
    lcd2.print(propBuf);
    lcd2.setCursor(0, 1);
    _printContextHint(expr, result);
  }

  // ================================================================
  //  CLAVIER 2 — MODE CONCEPT
  // ================================================================
  void onSciKeyConc(char key) {
    switch (key) {
      case 'M': return; // géré dans main.ino
      case '>':         // concept suivant
        conceptScroll = false;
        conceptIdx    = (conceptIdx + 1) % CONCEPT_COUNT;
        conceptLine   = 0;
        _displayConcept(); return;
      case '<':         // concept précédent
        conceptScroll = false;
        conceptIdx    = (conceptIdx == 0) ? CONCEPT_COUNT-1 : conceptIdx-1;
        conceptLine   = 0;
        _displayConcept(); return;
      case 'A':         // ligne suivante dans le concept
        _nextConceptLine(); return;
      case 'B':         // ligne précédente
        if (conceptLine > 0) conceptLine--;
        _displayConcept(); return;
      case 'S':         // toggle défilement auto
        conceptScroll = !conceptScroll;
        if (conceptScroll) lastScroll = millis();
        _displayConcept(); return;
    }
  }

  // ================================================================
  //  CLAVIER 2 — MODE GUIDE
  // ================================================================
  void onSciKeyGuide(char key) {
    if (key == 'M') return;

    if (inGuideSelect) {
      if (key == '<') { guideIdx = (guideIdx==0)?GUIDE_COUNT-1:guideIdx-1; _displayGuideSelect(); return; }
      if (key == '>') { guideIdx = (guideIdx+1)%GUIDE_COUNT; _displayGuideSelect(); return; }
    } else {
      if (key == '>') { _nextGuideLine(); return; }
      if (key == '<') { if (guideLine>0) guideLine--; _displayGuideLine(); return; }
      if (key == 'B') { inGuideSelect=true; guideLine=0; _displayGuideSelect(); return; }
    }
  }

  void onCalcKeyGuide(char key) {
    if (key == '=') {
      if (inGuideSelect) {
        inGuideSelect = false;
        guideLine     = 0;
        _displayGuideLine();
      } else {
        _nextGuideLine();
      }
    }
    if (key == 'C') { inGuideSelect=true; guideLine=0; _displayGuideSelect(); }
  }

private:
  // ================================================================
  //  AFFICHAGE CONCEPT
  // ================================================================
  void _displayConcept() {
    // LCD1 : titre du concept + indicateurs
    char titleBuf[STEP_MAX];
    PGM_P tp = (PGM_P)pgm_read_word(&CONCEPT_TITLES[conceptIdx]);
    strncpy_P(titleBuf, tp, STEP_MAX-1);
    titleBuf[STEP_MAX-1] = '\0';

    lcd1.clear();
    lcd1.setCursor(0, 0);
    lcd1.print(titleBuf);
    lcd1.setCursor(0, 1);
    lcd1.print(F("("));
    lcd1.print(conceptIdx+1);
    lcd1.print(F("/"));
    lcd1.print(CONCEPT_COUNT);
    lcd1.print(F(") "));
    lcd1.print(F("ligne:"));
    lcd1.print(conceptLine+1);
    if (conceptScroll) lcd1.print(F(" >>"));

    // LCD2 : ligne du concept
    char lineBuf[STEP_MAX];
    _readConceptLine(conceptIdx, conceptLine, lineBuf);

    lcd2.clear();
    lcd2.setCursor(0, 0);
    lcd2.print(F("[<][>]:concept  "));
    lcd2.setCursor(0, 1);
    if (lineBuf[0] == '\0') {
      // Retour au début
      conceptLine = 0;
      _readConceptLine(conceptIdx, 0, lineBuf);
    }
    lcd2.print(lineBuf);
  }

  void _nextConceptLine() {
    char tmp[STEP_MAX];
    _readConceptLine(conceptIdx, conceptLine+1, tmp);
    if (tmp[0] == '\0') conceptLine = 0;
    else                conceptLine++;
    _displayConcept();
  }

  void _readConceptLine(uint8_t cIdx, uint8_t line, char* out) {
    const char* const* table;
    switch (line) {
      case 0: table = CONCEPT_LINES_0; break;
      case 1: table = CONCEPT_LINES_1; break;
      case 2: table = CONCEPT_LINES_2; break;
      case 3: table = CONCEPT_LINES_3; break;
      case 4: table = CONCEPT_LINES_4; break;
      case 5: table = CONCEPT_LINES_5; break;
      default: out[0]='\0'; return;
    }
    PGM_P p = (PGM_P)pgm_read_word(&table[cIdx]);
    strncpy_P(out, p, STEP_MAX-1);
    out[STEP_MAX-1] = '\0';
  }

  // ================================================================
  //  AFFICHAGE GUIDE
  // ================================================================
  void _displayGuideSelect() {
    lcd1.clear();
    lcd1.setCursor(0, 0); lcd1.print(F("CHOISIR GUIDE:  "));
    lcd1.setCursor(0, 1);
    lcd1.print(F("< "));
    // Nom du guide depuis PROGMEM
    char nameBuf[STEP_MAX];
    PGM_P gn = (PGM_P)pgm_read_word(&GUIDE_NAMES[guideIdx]);
    strncpy_P(nameBuf, gn, STEP_MAX-1);
    nameBuf[STEP_MAX-1] = '\0';
    lcd1.print(nameBuf);
    lcd1.print(F(" >"));

    lcd2.clear();
    lcd2.setCursor(0, 0);
    lcd2.print(F("Guide "));
    lcd2.print(guideIdx+1);
    lcd2.print(F("/"));
    lcd2.print(GUIDE_COUNT);
    lcd2.print(F("        "));
    lcd2.setCursor(0, 1);
    lcd2.print(F("[=] commencer   "));
  }

  void _displayGuideLine() {
    char lineBuf[STEP_MAX];
    PGM_P p = (PGM_P)pgm_read_word(&GUIDES[guideIdx][guideLine]);
    strncpy_P(lineBuf, p, STEP_MAX-1);
    lineBuf[STEP_MAX-1] = '\0';

    if (lineBuf[0] == '\0') {
      // Fin du guide
      lcd1.clear(); lcd2.clear();
      lcd1.setCursor(0,0); lcd1.print(F("FIN DU GUIDE!   "));
      lcd1.setCursor(0,1); lcd1.print(F("[C] recommencer "));
      lcd2.setCursor(0,0); lcd2.print(F("Bravo!          "));
      lcd2.setCursor(0,1); lcd2.print(F("[B] changer guide"));
      return;
    }

    // LCD1 : ligne de contenu
    lcd1.clear();
    lcd1.setCursor(0, 0);
    lcd1.print(lineBuf);
    // Ligne suivante si disponible
    char nextBuf[STEP_MAX];
    PGM_P pn = (PGM_P)pgm_read_word(&GUIDES[guideIdx][guideLine+1]);
    strncpy_P(nextBuf, pn, STEP_MAX-1);
    if (nextBuf[0] != '\0') {
      lcd1.setCursor(0, 1);
      lcd1.print(nextBuf);
    }

    // LCD2 : navigation
    lcd2.clear();
    lcd2.setCursor(0, 0);
    lcd2.print(F("Etape "));
    lcd2.print(guideLine/2 + 1);
    lcd2.print(F("         "));
    lcd2.setCursor(0, 1);
    lcd2.print(F("[>] ou [=]:suite"));
  }

  void _nextGuideLine() {
    guideLine += 2; // on avance de 2 lignes (affichées 2 par 2)
    _displayGuideLine();
  }

  // ================================================================
  //  ANALYSE RÉSULTAT
  // ================================================================
  void _detectProperty(double v, char* out) {
    char pgmBuf[STEP_MAX];
    PGM_P src = nullptr;

    if (v == 0.0)              src = PROP_ZERO;
    else if (v == 1.0)         src = PROP_ONE;
    else if (v < 0)            src = PROP_NEG;
    else if (abs(v) >= 1e6)    src = PROP_BIG;
    else if (abs(v) < 1.0 && v!=0) src = PROP_SMALL;
    else if (v == (long)v)     {
      // Entier → tester si premier
      src = _isPrime((long)v) ? PROP_PRIME : PROP_INT;
    }
    else if (abs(v - 3.14159) < 0.001)  src = PROP_PI;
    else if (abs(v - 2.71828) < 0.001)  src = PROP_E;
    else if (abs(v - 1.41421) < 0.001)  src = PROP_SQRT2;
    else if (abs(v - 1.61803) < 0.001)  src = PROP_GOLDEN;
    else                                 src = PROP_APPROX;

    strncpy_P(out, src, STEP_MAX-1);
    out[STEP_MAX-1] = '\0';
  }

  bool _isPrime(long n) {
    if (n < 2 || n > 1000) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (long i = 3; i*i <= n; i += 2)
      if (n % i == 0) return false;
    return true;
  }

  void _printContextHint(const char* expr, double result) {
    // Détecte le type d'opération dans l'expression
    bool hasTrig = (strchr(expr,'s') || strchr(expr,'d') || strchr(expr,'t'));
    bool hasLog  = (strchr(expr,'n') || strchr(expr,'l'));
    bool hasPow  = (strchr(expr,'^'));
    bool hasSqrt = (strchr(expr,'Q'));

    if (hasTrig)      lcd2.print(F("[CONCEPT:SIN 0/1]"));
    else if (hasLog)  lcd2.print(F("[CONCEPT:LN  3/4]"));
    else if (hasSqrt) lcd2.print(F("[CONCEPT:SQRT 5 ]"));
    else if (hasPow)  lcd2.print(F("[CONCEPT:PUIS 6 ]"));
    else              lcd2.print(F("Mode IA:CONCEPT "));
  }
};

extern IABrain iaBrain;
