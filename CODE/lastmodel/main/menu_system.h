#pragma once
// ============================================================
//  CALCU IA — menu_system.h
//  Menu graphique interactif sur 2 LCDs
//
//  Clavier SCI :
//    [<]  = item précédent
//    [>]  = item suivant
//    [=]  = confirmer (clavier CALC)
//    [M]  = retour / monter d'un niveau
//    [S]  = basculer DEG/RAD depuis le menu
//
//  LCD1 : titre + item sélectionné animé
//  LCD2 : description de l'item + navigation
// ============================================================
#include "config.h"
#include "display_mgr.h"

// ---- Caractères animation sélection ----
// Défile : [ item ] → flèche clignotante
#define ANIM_TICK_MS 500u

class MenuSystem {
public:
  bool     animTick;           // pour le clignotement du curseur
  unsigned long lastAnim;

  void begin() {
    animTick = false;
    lastAnim = 0;
    _drawMenu();
  }

  // ================================================================
  //  Appelé depuis loop() — clavier SCI
  // ================================================================
  void onSciKey(char key) {
    switch (key) {

      case '<':   // Précédent
        _navigate(-1);
        break;

      case '>':   // Suivant
        _navigate(+1);
        break;

      case 'M':   // Retour / monter
        _goBack();
        break;

      case 'S':   // Basculer DEG/RAD
        gState.angleMode = (gState.angleMode == ANG_DEG) ? ANG_RAD : ANG_DEG;
        _drawMenu();
        break;
    }
  }

  // ================================================================
  //  Clavier CALC : seule la touche [=] confirme
  // ================================================================
  void onCalcKey(char key) {
    if (key == '=') _confirm();
  }

  // ================================================================
  //  Tick animation (clignotement curseur)
  // ================================================================
  void update() {
    unsigned long now = millis();
    if (now - lastAnim >= ANIM_TICK_MS) {
      lastAnim = now;
      animTick = !animTick;
      _drawSelectedItem(); // re-draw uniquement l'item sélectionné
    }
  }

  // ================================================================
  //  Entrer dans le menu depuis n'importe quel mode
  // ================================================================
  void enter() {
    gState.appState  = APP_MENU;
    gState.menuState = MENU_ROOT;
    gState.menuSel   = 0;
    gState.shift     = false;
    gState.hyp       = false;
    gState.hasError  = false;
    _drawMenu();
  }

private:

  // ================================================================
  //  NAVIGATION
  // ================================================================
  void _navigate(int dir) {
    uint8_t maxSel = _maxSel();
    if (dir > 0) {
      gState.menuSel = (gState.menuSel + 1) % maxSel;
    } else {
      gState.menuSel = (gState.menuSel == 0) ? maxSel - 1 : gState.menuSel - 1;
    }
    _drawMenu();
  }

  uint8_t _maxSel() {
    switch (gState.menuState) {
      case MENU_ROOT:   return 2;             // Normal | IA
      case MENU_NORMAL: return NORMAL_COUNT;
      case MENU_IA:     return IA_COUNT;
    }
    return 2;
  }

  // ================================================================
  //  CONFIRMATION [=]
  // ================================================================
  void _confirm() {
    switch (gState.menuState) {

      case MENU_ROOT:
        // 0 = Mode Normal, 1 = Mode IA
        gState.menuState = (gState.menuSel == 0) ? MENU_NORMAL : MENU_IA;
        gState.menuSel   = 0;
        _drawMenu();
        break;

      case MENU_NORMAL:
        // Lancer le mode normal sélectionné
        gState.mode     = gState.menuSel; // COMP / MATRIX / EQUATION
        gState.appState = APP_RUN;
        _showLaunchAnim(false);
        break;

      case MENU_IA:
        // Lancer le mode IA sélectionné
        gState.mode      = MODE_IA;
        gState.iaSubMode = gState.menuSel;
        gState.appState  = APP_RUN;
        _showLaunchAnim(true);
        break;
    }
  }

  // ================================================================
  //  RETOUR
  // ================================================================
  void _goBack() {
    switch (gState.menuState) {
      case MENU_ROOT:
        // Déjà au root → rien
        break;
      case MENU_NORMAL:
      case MENU_IA:
        gState.menuState = MENU_ROOT;
        gState.menuSel   = 0;
        _drawMenu();
        break;
    }
  }

  // ================================================================
  //  AFFICHAGE COMPLET DU MENU
  // ================================================================
  void _drawMenu() {
    lcd1.clear();
    lcd2.clear();

    switch (gState.menuState) {

      // ------ ROOT : choisir Normal ou IA ------
      case MENU_ROOT:
        // LCD1 ligne 0 : titre
        lcd1.setCursor(0, 0);
        lcd1.print(F("=== CALCU  IA ==="));
        // LCD1 ligne 1 : item sélectionné
        _drawSelectedItem();

        // LCD2 : description + navigation
        lcd2.setCursor(0, 0);
        if (gState.menuSel == 0) {
          lcd2.print(F("Calculs classiques"));
          lcd2.setCursor(0, 1);
          lcd2.print(F("+ fonctions sci."));
        } else {
          lcd2.print(F("Limites, derivees"));
          lcd2.setCursor(0, 1);
          lcd2.print(F("Integrales,Explic."));
        }
        break;

      // ------ NORMAL submenu ------
      case MENU_NORMAL:
        lcd1.setCursor(0, 0);
        lcd1.print(F("MODE NORMAL :   "));
        _drawSelectedItem();

        lcd2.setCursor(0, 0);
        lcd2.print(NORMAL_DESC[gState.menuSel]);
        lcd2.setCursor(0, 1);
        _drawNavHint();
        break;

      // ------ IA submenu ------
      case MENU_IA:
        lcd1.setCursor(0, 0);
        lcd1.print(F("MODE IA :       "));
        _drawSelectedItem();

        lcd2.setCursor(0, 0);
        lcd2.print(IA_DESC[gState.menuSel]);
        lcd2.setCursor(0, 1);
        _drawNavHint();
        break;
    }
  }

  // ----------------------------------------------------------------
  //  Ligne 1 de LCD1 : "< NOM ITEM >" avec curseur animé
  // ----------------------------------------------------------------
  void _drawSelectedItem() {
    lcd1.setCursor(0, 1);
    // Flèche gauche animée
    lcd1.print(animTick ? '<' : ' ');
    lcd1.print(' ');

    // Nom de l'item centré sur 10 chars
    const char* name = _currentItemName();
    uint8_t nlen = strlen(name);
    uint8_t pad  = (10 - nlen) / 2;
    for (uint8_t i = 0; i < pad; i++) lcd1.print(' ');
    lcd1.print(name);
    uint8_t rpad = 10 - nlen - pad;
    for (uint8_t i = 0; i < rpad; i++) lcd1.print(' ');

    // Flèche droite animée
    lcd1.print(' ');
    lcd1.print(animTick ? '>' : ' ');

    // Angle mode affiché à droite
    lcd1.print(gState.angleMode == ANG_DEG ? 'D' : 'R');
  }

  const char* _currentItemName() {
    switch (gState.menuState) {
      case MENU_ROOT:
        return (gState.menuSel == 0) ? "NORMAL" : "IA";
      case MENU_NORMAL:
        return NORMAL_NAMES[gState.menuSel];
      case MENU_IA:
        return IA_NAMES[gState.menuSel];
    }
    return "?";
  }

  // ----------------------------------------------------------------
  //  LCD2 ligne 1 : hint de navigation
  // ----------------------------------------------------------------
  void _drawNavHint() {
    uint8_t cur  = gState.menuSel + 1;
    uint8_t tot  = _maxSel();
    // "[=]OK  <> nav  (2/3)"
    lcd2.print(F("[=]OK <> ("));
    lcd2.print(cur);
    lcd2.print('/');
    lcd2.print(tot);
    lcd2.print(F(") [M]ret"));
  }

  // ----------------------------------------------------------------
  //  Animation de lancement du mode
  // ----------------------------------------------------------------
  void _showLaunchAnim(bool isIA) {
    lcd1.clear(); lcd2.clear();
    lcd1.setCursor(0, 0);
    lcd1.print(F("Chargement...   "));
    lcd1.setCursor(0, 1);
    if (isIA) {
      lcd1.print(F("MODE IA : "));
      lcd1.print(IA_NAMES[gState.iaSubMode]);
    } else {
      lcd1.print(F("MODE : "));
      lcd1.print(NORMAL_NAMES[gState.mode]);
    }
    lcd2.setCursor(0, 0);
    for (uint8_t i = 0; i < 16; i++) {
      lcd2.print('=');
      delay(40);
    }
    lcd2.setCursor(0, 1);
    lcd2.print(F("[M] = retour menu"));
    delay(500);
  }
};

extern MenuSystem menuSys;
