void setup() {
  pinMode(13, OUTPUT);
  Serial.begin();
  delay(1000);

}

void loop() {
  Serial.print(".");
  digitalWrite(13, HIGH);
  delay(2000);
  digitalWrite(13, LOW);
  delay(500);

}
