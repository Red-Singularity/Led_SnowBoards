void bluetooth_setup(){
  //setup bluetooth connections
  SerialBT.begin("LED_Snowboard_V2"); //name given to esp32
  Serial.println("\n The device started, now you can pair it with bluetooth!");
}

void bluetooth_control(){
  // a set of if statements to read data from the terminal and control the board

  if(SerialBT.available()){

    //get incoming data as a a string
    incomingChar = SerialBT.read();
    if (incomingChar != '\n'){
      message += String(incomingChar);
    }
    else{
      message = "";
    }
    Serial.println(message); 


    //turn board on
    if(message == "FF"){
      if ((bat_safe == 1) && (voltage < 9.5)){
        digitalWrite(GATE_SIGNAL, false);
      }
      else{
        digitalWrite(GATE_SIGNAL, true);
      }
    }

    //turn board off
    else if(message == "FE"){
      digitalWrite(GATE_SIGNAL, false);
    }

    //read battery values
    else if(message == "0B"){
      SerialBT.print("Battery Voltage: ");
      SerialBT.println(voltage);
      SerialBT.print("Total Current: ");
      SerialBT.println(current);
      SerialBT.print("Total Power: ");
      SerialBT.println(power);
    }

    //disable low battery cut off
     else if(message == "0D"){
      bat_safe = 0;
    }

    //enable low battery cutoff ( is set to on by default)
     else if(message == "0C"){
      bat_safe = 1;
    }

    //set color to white
    else if(message == "06"){
      rand_LED = 0;
      nyan = 0;
      mario = 0;
      pacman = 0;
      for(int i=0; i<(NUM_LEDS/2); i++){
        half1.setPixelColor(i, 255, 255, 255);
        half2.setPixelColor(i, 255, 255, 255);
      }
      for(int i=0; i<TOP_LEDS; i++){
        //top.setPixelColor(i, 255, 255, 255);
      }
    }

    //set color to Green
    else if(message == "07"){
      rand_LED = 0;
      nyan = 0;
      mario = 0;
      pacman = 0;
      for(int i=0; i<(NUM_LEDS/2); i++){
        half1.setPixelColor(i, 255, 0, 0);
        half2.setPixelColor(i, 255, 0, 0);
      }
      for(int i=0; i<TOP_LEDS; i++){
        //top.setPixelColor(i, 0, 255, 0);
      }
    }

    //set color to Red
    else if(message == "08"){
      rand_LED = 0;
      nyan = 0;
      mario = 0;
      pacman = 0;
      for(int i=0; i<(NUM_LEDS/2); i++){
        half1.setPixelColor(i, 0, 255, 0);
        half2.setPixelColor(i, 0, 255, 0);
      }
      for(int i=0; i<TOP_LEDS; i++){
        //top.setPixelColor(i, 255, 0, 0);
      }
    }

    //set color to Blue
    else if(message == "09"){
      rand_LED = 0;
      nyan = 0;
      mario = 0;
      pacman = 0;
      for(int i=0; i<(NUM_LEDS/2); i++){
        half1.setPixelColor(i, 0, 0, 255);
        half2.setPixelColor(i, 0, 0, 255);
      }
      for(int i=0; i<TOP_LEDS; i++){
        //top.setPixelColor(i, 0, 0, 255);
      }
    }

    //set color to Random
    else if(message == "0A"){
      nyan = 0;
      rand_LED = 1;
      mario = 0;
      pacman = 0;
    }

    //set brightness to 5%
    else if(message == "00"){
      half1.setBrightness(13);
      half2.setBrightness(13);
      top.setBrightness(13);
    }

    //set brightness to 10%
    else if(message == "01"){
      half1.setBrightness(26);
      half2.setBrightness(26);
      top.setBrightness(26);
    }

    //set brightness to 25%
    else if(message == "02"){
      half1.setBrightness(64);
      half2.setBrightness(64);
      top.setBrightness(64);
    }

    //set brightness to 50%
    else if(message == "03"){
      half1.setBrightness(128);
      half2.setBrightness(128);
      top.setBrightness(128);
    }

    //set brightness to 75%
    else if(message == "04"){
      half1.setBrightness(191);
      half2.setBrightness(191);
      top.setBrightness(191);
    }

    //set brightness to 100%
    else if(message == "05"){
      half1.setBrightness(255);
      half2.setBrightness(255);
      top.setBrightness(255);
    }

    //assign nyan cat to board
    else if(message == "10"){
      rand_LED = 0;
      nyan = 1;
      pacman = 0;
      mario = 0;
      neon = 0;
    }

    //assign mario image to board
    else if(message == "11"){
      rand_LED = 0;
      nyan = 0;
      pacman = 0;
      mario = 1;
      neon = 0; 
    }

    //assign pacman to the board
    else if(message == "12"){
      rand_LED = 0;
      nyan = 0;
      pacman = 1;
      mario = 0;
      neon = 0;
    }
    //assign neon board to the board
    else if(message == "13"){
      rand_LED = 0;
      nyan = 0;
      pacman = 0;
      mario = 0;
      neon = 1;
    }
  
    else{}

  }

  if (rand_LED == 1){
    for(int i=0; i<(NUM_LEDS/2); i++){
      half1.setPixelColor(i, random(0, 0xFFFFFF));
      half2.setPixelColor(i, random(0, 0xFFFFFF));
    }
    for(int i=0; i<TOP_LEDS; i++){
      //top.setPixelColor(i, random(0, 0xFFFFFF));
    }
  }

  if (nyan == 1){
    Serial.println("nyan");
    assign_nyan();
  }

  if (pacman == 1){
    Serial.println("pacman");
    assign_pacman();
  }

  if (mario == 1){
    Serial.println("mario");
    assign_mario();
  }

  if (neon == 1){
    Serial.println("neon");
    assign_neon();
  }
}


/*void app(){
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
}*/
