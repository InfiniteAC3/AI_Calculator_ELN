#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// On définit les deux adresses différentes
LiquidCrystal_I2C lcd1(0x27, 16, 2); 
LiquidCrystal_I2C lcd2(0x21, 16, 2); 

void setup() {
  Wire.begin();
  delay(8000); // On attend 2 secondes que Proteus stabilise le CPU
  
  lcd1.init();
  delay(1000); // Très important : laisse le temps au modèle LCD de traiter l'init
  lcd1.backlight();
  
  lcd1.setCursor(0, 0);
  lcd1.print("TEST CPU"); 
}

void loop() {}