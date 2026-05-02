bool isON = true;
void setup() {
  pinMode(1,OUTPUT);

}

void loop() {
  isON = !isON;
  digitalWrite(1,isON);
  delay(300);
  

}
