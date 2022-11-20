void bluetooth_setup(){
  //setup bluetooth connections
  
  SerialBT.begin("LED_Snowboard"); //name given to esp32
  Serial.println("\n The device started, now you can pair it with bluetooth!");
  
}

void app(){
  // communications with android app. 
  int loops = 0;
  float timer = micros();
  
  while (Serial.available()) {
    SerialBT.write(Serial.read());
    loops = 1;
    counter = counter++;
  }
  
  while (SerialBT.available()){
    Serial.write(SerialBT.read());
    loops = 1;
    counter = counter++;
  }

  if(loops == 1){
    loops = 0;
    timer = micros() - timer;
    Serial.print("\n Bluetooth Time(s): "); Serial.println(timer/1000000);
  }
}
