void setup() {
  for(int i=1; i<=13; i++)  pinMode(i, OUTPUT);
  delay(100);
  Serial.begin(115200);
  delay(1000);

}

void loop() {
  // rgbLedWrite(WS_RGB, 10, 0, 0); 
  for(int i=1; i<=13; i++)  digitalWrite(i, HIGH);
  delay(20);
  // rgbLedWrite(WS_RGB, 0, 00, 0); 

  for(int i=1; i<=13; i++)  digitalWrite(i, LOW); 
  delay(20);
  // Serial.println("LOOP");
  

}
