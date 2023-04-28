void setup() {
  Serial.begin(115200);
  pinMode(11, OUTPUT);

}

void loop() {
  for(int i = 0; i < 255; i++){
    analogWrite(11, i);
    delay(1);
    //Serial.print("Value: ");
    //Serial.println(i);
  }

  for(int g = 255; g >= 0; g--){
    analogWrite(11, g);
    delay(1);
    //Serial.print("Value: ");
    //Serial.println(g);
  }

}
