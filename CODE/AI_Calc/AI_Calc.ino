#include <Wire.h>
#include "DFRobot_LCD.h"

// Déclaration de deux objets distincts
// Assure-toi d'avoir installé la bibliothèque DFRobot_LCD
DFRobot_LCD lcdCalcul(16, 2); 
DFRobot_LCD lcdIA(16, 2);

void setup() {
    // Initialisation du premier écran (Calculs)
    lcdCalcul.init();
    lcdCalcul.setRGB(255, 255, 255); // Fond blanc
    lcdCalcul.setCursor(0, 0);
    lcdCalcul.print("Cenderson Calc");
    
    // Initialisation du deuxième écran (Explications)
    lcdIA.init();
    lcdIA.setRGB(0, 100, 255); // Fond bleu
    lcdIA.clear();
    lcdIA.setCursor(0, 0);
    lcdIA.print("IA Cenderson:");
    lcdIA.setCursor(0, 1);
    lcdIA.print("Systeme Pret.");

    delay(2000);
}

void loop() {
    // Test d'affichage
    lcdCalcul.setCursor(0, 1);
    lcdCalcul.print("Calcul: 15 + 25");

    lcdIA.setCursor(0, 1);
    lcdIA.print("Addition simple ");
    
    delay(500);
}