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
        SerialBT.println("Error - Low Voltage");
      }
      else{
        digitalWrite(GATE_SIGNAL, true);
        SerialBT.println("On");
      }
    }

    //turn board off
    else if(message == "FE"){
      Serial.println("Off");
      digitalWrite(GATE_SIGNAL, false);
      SerialBT.println("Off");
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
      SerialBT.println("Battery Safety Diabled");
    }

    //enable low battery cutoff ( is set to on by default)
     else if(message == "0C"){
      bat_safe = 1;
      SerialBT.println("Battery Safety Enabled");
    }

    //set color to white
    else if(message == "06"){
      SerialBT.println("Color - White");
      rand_LED = 0;
      media1 = 0;
      media2 = 0;
      media3 = 0;
      media4 = 0;
      media5 = 0;
      for(int i=0; i<(NUM_LEDS/2); i++){
        half1[i] = CRGB::White;
        half2[i+1] = CRGB::White;
      }
      for(int i=0; i<TOP_LEDS; i++){
      }
    }

    //set color to Green
    else if(message == "07"){
      //SerialBT.println("Color - Green");
      rand_LED = 0;
      media1 = 0;
      media2 = 0;
      media3 = 0;
      media4 = 0;
      media5 = 0;
      for(int i=0; i<(NUM_LEDS/2); i++){
        half1[i] = CRGB::Green;
        half2[i+1] = CRGB::Green;
      }
      for(int i=0; i<TOP_LEDS; i++){
      }
    }

    //set color to Red
    else if(message == "08"){
      SerialBT.println("Color - Red");
      rand_LED = 0;
      media1 = 0;
      media2 = 0;
      media3 = 0;
      media4 = 0;
      media5 = 0;
      for(int i=0; i<(NUM_LEDS/2); i++){
        half1[i] = CRGB::Red;
        half2[i+1] = CRGB::Red;
      }
      for(int i=0; i<TOP_LEDS; i++){
      }
    }

    //set color to Blue
    else if(message == "09"){
      SerialBT.println("Color - Blue");
      rand_LED = 0;
      media1 = 0;
      media2 = 0;
      media3 = 0;
      media4 = 0;
      media5 = 0;
      for(int i=0; i<(NUM_LEDS/2); i++){
        half1[i] = CRGB::Blue;
        half2[i+1] = CRGB::Blue;
      }
      for(int i=0; i<TOP_LEDS; i++){
      }
    }

    //set color to Random
    else if(message == "0A"){
      SerialBT.println("Color - Random");
      rand_LED = 1;
      media1 = 0;
      media2 = 0;
      media3 = 0;
      media4 = 0;
      media5 = 0;
    }

    //set brightness to 5%
    else if(message == "00"){
      SerialBT.println("Brightness - 5%");
      FastLED.setBrightness(5);
    }

    //set brightness to 10%
    else if(message == "01"){
      SerialBT.println("Brightness - 10%");
      FastLED.setBrightness(10);
    }

    //set brightness to 25%
    else if(message == "02"){
      SerialBT.println("Brightness - 25%");
      FastLED.setBrightness(25);
    }

    //set brightness to 50%
    else if(message == "03"){
      SerialBT.println("Brightness - 50%");
      FastLED.setBrightness(50);
    }

    //set brightness to 75%
    else if(message == "04"){
      SerialBT.println("Brightness - 75%");
      FastLED.setBrightness(75);
    }

    //set brightness to 100%
    else if(message == "05"){
      SerialBT.println("Brightness - 100%");
      FastLED.setBrightness(100);
    }

    //assign media 1 to board
    else if(message == "10"){
      SerialBT.println("media1");
      frame_number = 0;
      rand_LED = 0;
      media1 = 1;
      media2 = 0;
      media3 = 0;
      media4 = 0;
      media5 = 0;
    }

    //assign media 2 image to board
    else if(message == "11"){
      SerialBT.println("media2");
      frame_number = 0;
      rand_LED = 0;
      media1 = 0;
      media2 = 1;
      media3 = 0;
      media4 = 0;
      media5 = 0; 
    }

    //assign media 3 to the board
    else if(message == "12"){
      SerialBT.println("media3");
      frame_number = 0;
      rand_LED = 0;
      media1 = 0;
      media2 = 0;
      media3 = 1;
      media4 = 0;
      media5 = 0;
    }
    //assign media4 to the board
    else if(message == "13"){
      SerialBT.println("media4");
      frame_number = 0;
      rand_LED = 0;
      media1 = 0;
      media2 = 0;
      media3 = 0;
      media4 = 1;
      media5 = 0;
    }
    //assign media 5 board to the board
    else if(message == "14"){
      SerialBT.println("media5");
      frame_number = 0;
      rand_LED = 0;
      media1 = 0;
      media2 = 0;
      media3 = 0;
      media4 = 0;
      media5 = 1;
    }
  //debugging function
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

  if (media1 == 1){
    assign_media1();
  }

  if (media2 == 1){
    assign_media2();
  }

  if (media3 == 1){
    assign_media3();
  }

  if (media4 == 1){
    assign_media4();
  }

  if (media5 == 1){
    assign_media5();
  }
}
