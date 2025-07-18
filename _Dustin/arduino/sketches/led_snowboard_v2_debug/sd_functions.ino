
void sd_setup(){
  // Wait for USB Serial
  while (!Serial) {
  }

  Serial.println("\n\n SPI pins:");
  Serial.print(" MISO: "); Serial.println(int(MISO));
  Serial.print(" MOSI: "); Serial.println(int(MOSI));
  Serial.print(" SCK:  "); Serial.println(int(SCLK));
  Serial.print(" CS:   "); Serial.println(int(SD_CS_PIN));

  Serial.println("\n Initializing SD card...");

  if(!SD.begin(SD_CS_PIN, SPI_CLOCK)) {
    if(SD.card()->errorCode()) {
      Serial.println(
             "\n SD initialization failed.\n"
             "Do not reformat the card!\n"
             "Is the card correctly inserted?\n"
             "Is chipSelect set to the correct value?\n"
             "Does another SPI device need to be disabled?\n"
             "Is there a wiring/soldering problem?\n");
       return;
    }
  }
    
  Serial.println(" Card successfully initialized");

  Serial.println("Available Images: ");
  SD.ls("/", LS_R);
  
}


void sd_image_read(){
  //read image data from file then save to global array
  
  String pixelValue = ""; // string of integer values 
  char currentChar;
  int frameDataCount = 0;
  int pixelData = 0;
  float timer = 0;
  float readTime = 0;
  file = SD.open("/test_gif/image0.dat");

  timer = micros(); // start timer1
  while(file.available()){
    //timer = micros(); // start timer1
    currentChar = file.read(); // read next character in file
    //readTime = (micros() - timer)/1000; // end timer
    //Serial.println(currentChar);
    if((currentChar == ',') or (!file.available())){ // ',' marks where data is seperated
      if(!file.available()){ // make sure to gather last digit in file
        pixelValue = pixelValue + currentChar; // concatonate character to string
      }
      pixelData = pixelValue.toInt(); // assign file data to array
      // frameData[frameDataCount] = pixelData; // assign file data to array
      pixelValue = ""; // reset pixelValue string to nothing
      frameDataCount++; // incriment frame data location
      //Serial.println(frameDataCount);
    }
    else{
      pixelValue = pixelValue + currentChar; // concatonate character to string
    }
  }

  file.close();
  readTime = (micros() - timer)/1000; // end timer
  Serial.printf("\n\n Read Time: %fmS\n\n", readTime);
  delay(1000);
  
}

void readFrame(String filename){
  //display frame data from sd card. takes in filename
  String Path = "/" + filename + "/image" + frame_number + ".dat";
  file = SD.open(Path);
  //Serial.println(Path);
  String pixelValue = ""; // string of integer values 
  char currentChar;
  int frameDataCount = 0;
  int pixelData = 0;

  if(!file.available()){
    frame_number = 0;
    String Path = "/" + filename + "/image" + frame_number + ".dat";
    file = SD.open(Path);
  }

  //assign data from dat file to int array
  while(file.available()){
    currentChar = file.read(); // read next character in file
    if((currentChar == ',') or (!file.available())){ // ',' marks where data is seperated
      if(!file.available()){ // make sure to gather last digit in file
        pixelValue = pixelValue + currentChar; // concatonate character to string
      }
      pixelData = pixelValue.toInt(); // assign file data to array
      //Serial.println(pixelData);
      frameData[frameDataCount] = pixelData; // assign file data to array
      pixelValue = ""; // reset pixelValue string to nothing
      frameDataCount++; // incriment frame data location
      //Serial.println(frameDataCount);
    }
    else{
      pixelValue = pixelValue + currentChar; // concatonate character to string
    }
  }

  frame_number = frame_number + 1;

  for(int i=0; i<(NUM_LEDS/2); i++){
    half1[i] = CRGB(frameData[i]);
    half2[i+1] = CRGB(frameData[i+NUM_LEDS/2]);
    //delay(50); // fake the frame rate
  }
}
