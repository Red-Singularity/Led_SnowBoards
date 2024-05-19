void bluetooth_setup(){
  //setup bluetooth connections
  SerialBT.begin("LED_Snowboard_V2"); //name given to esp32
  Serial.println("\n The device started, now you can pair it with bluetooth!");
}

void bluetooth_control(){
  // a set of if statements to read data from the terminal and control the board
  bool command = 0;
  char incomingChar;
  String message = "";

  while(SerialBT.available()){
    incomingChar = char(SerialBT.read()); // read what is in serial buffer
    Serial.print(incomingChar);
    if(incomingChar == '\n'|| incomingChar == '\0' || incomingChar == '\r'){ // see if terminating character appears
      command = 1; //allow command to be read by code
      break; // leave while loop
    }
    else{
      message += incomingChar; // add character to bluetooth command string
    }
  }

  if(command){
    command = 0;

    //turn board on
    if(message == "FF"){
      Serial.println("On");
      if ((bat_safe == 1) && (voltage < 9.5)){
        digitalWrite(GATE_SIGNAL, false);
      }
      else{
        digitalWrite(GATE_SIGNAL, true);
      }
    }

    //turn board off
    else if(message == "FE"){
      Serial.println("Off");
      digitalWrite(GATE_SIGNAL, false);
    }

    //read battery values
    else if(message == "0B"){
      SerialBT.print("Battery Voltage: ");
      SerialBT.println(voltage);
      SerialBT.print("Min Cell Voltage: ");
      SerialBT.println(minCell);
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
      pacman = 0;
      mario = 0;
      neon = 0;
      kirby = 0;
      for(int i=0; i<(NUM_LEDS/2); i++){
        half1[i] = CRGB::White;
        half2[i+1] = CRGB::White;
      }
      for(int i=0; i<TOP_LEDS; i++){
      }
    }

    //set color to Green
    else if(message == "07"){
      rand_LED = 0;
      nyan = 0;
      pacman = 0;
      mario = 0;
      neon = 0;
      kirby = 0;
      for(int i=0; i<(NUM_LEDS/2); i++){
        half1[i] = CRGB::Green;
        half2[i+1] = CRGB::Green;
      }
      for(int i=0; i<TOP_LEDS; i++){
      }
    }

    //set color to Red
    else if(message == "08"){
      rand_LED = 0;
      nyan = 0;
      pacman = 0;
      mario = 0;
      neon = 0;
      kirby = 0;
      for(int i=0; i<(NUM_LEDS/2); i++){
        half1[i] = CRGB::Red;
        half2[i+1] = CRGB::Red;
      }
      for(int i=0; i<TOP_LEDS; i++){
      }
    }

    //set color to Blue
    else if(message == "09"){
      rand_LED = 0;
      nyan = 0;
      pacman = 0;
      mario = 0;
      neon = 0;
      kirby = 0;
      for(int i=0; i<(NUM_LEDS/2); i++){
        half1[i] = CRGB::Blue;
        half2[i+1] = CRGB::Blue;
      }
      for(int i=0; i<TOP_LEDS; i++){
      }
    }

    //set color to Random
    else if(message == "0A"){
      rand_LED = 1;
      nyan = 0;
      pacman = 0;
      mario = 0;
      neon = 0;
      kirby = 0;
    }

    //set brightness to 5%
    else if(message == "00"){
      FastLED.setBrightness(5);
    }

    //set brightness to 10%
    else if(message == "01"){
      FastLED.setBrightness(10);
    }

    //set brightness to 25%
    else if(message == "02"){
      FastLED.setBrightness(25);
    }

    //set brightness to 50%
    else if(message == "03"){
      FastLED.setBrightness(50);
    }

    //set brightness to 75%
    else if(message == "04"){
      FastLED.setBrightness(75);
    }

    //set brightness to 100%
    else if(message == "05"){
      FastLED.setBrightness(100);
    }

    //assign nyan cat to board
    else if(message == "10"){
      SerialBT.println("nyan");
      frame_number = 0;
      rand_LED = 0;
      nyan = 1;
      pacman = 0;
      mario = 0;
      neon = 0;
      kirby = 0;
    }

    //assign mario image to board
    else if(message == "11"){
      SerialBT.println("mario");
      frame_number = 0;
      rand_LED = 0;
      nyan = 0;
      pacman = 0;
      mario = 1;
      neon = 0;
      kirby = 0; 
    }

    //assign pacman to the board
    else if(message == "12"){
      SerialBT.println("pacman");
      frame_number = 0;
      rand_LED = 0;
      nyan = 0;
      pacman = 1;
      mario = 0;
      neon = 0;
      kirby = 0;
    }
    //assign neon board to the board
    else if(message == "13"){
      SerialBT.println("neon");
      frame_number = 0;
      rand_LED = 0;
      nyan = 0;
      pacman = 0;
      mario = 0;
      neon = 1;
      kirby = 0;
    }
    //assign kirby board to the board
    else if(message == "14"){
      SerialBT.println("kirby");
      frame_number = 0;
      rand_LED = 0;
      nyan = 0;
      pacman = 0;
      mario = 0;
      neon = 0;
      kirby = 1;
    }

    else if(message == "list"){
      Serial.print("list");
    }
  
    else{}

    message = ""; // reset command string
  }

//-------------------------------------------------------------------------------

  if (rand_LED == 1){
    for(int i=0; i<(NUM_LEDS/2); i++){
      half1[i] = random(0, 0xFFFFFF);
      half2[i+1] = random(0, 0xFFFFFF);
    }
    for(int i=0; i<TOP_LEDS; i++){
    }
  }

  if (nyan == 1){
    assign_nyan();
  }

  if (pacman == 1){
    assign_pacman();
  }

  if (mario == 1){
    assign_mario();
  }

  if (neon == 1){
    assign_neon();
  }

  if (kirby == 1){
    assign_kirby();
  }
}
