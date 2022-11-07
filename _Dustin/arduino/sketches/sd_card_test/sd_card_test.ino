/*
Dustin Fisher

Test for reading and writing files to an sd card with esp32
*/

#include "FS.h" // handles files
#include "SD.h" // interfaces with sd card
#include "SPI.h" // handles SPI comms

// pin numbers refer to esp32 gpio number
#define led0 12
#define led1 27
#define led2 25
#define led3 32

//Global variables
int frameData[2090]; // array of data for single frame

void setup() {
  //setup serial output
  Serial.begin(500000);

  //set SPI speed
  SPI.setClockDivider(10);
  
  // set up status leds
  pinMode(led0, OUTPUT);
  digitalWrite(led0, LOW);
  pinMode(led1, OUTPUT);
  digitalWrite(led1, LOW);
  pinMode(led2, OUTPUT);
  digitalWrite(led2, LOW);
  pinMode(led3, OUTPUT);
  digitalWrite(led3, LOW);

  //setup sd card
  if(!SD.begin(5)){
    Serial.println("Card Mount Failed");
    digitalWrite(led0, HIGH);
    while(1);
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    digitalWrite(led0, HIGH);
    while(1);
    return;
  }

    Serial.print("\nSD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  //show sd card 
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));

}

void loop() {
  char currentChar;
  String pixelValue = ""; // string of integer values 
  int frameDataCount = 0;
  int pixelData = 0;
  int timer1 = 0;
  int timer2 = 0;
  int readTimeMicros = 0;
  int readTimeMillis = 0;
  File file = SD.open("/test/image0.dat");

  timer1 = micros(); // start timer1
  timer2 = millis(); // start timer2
  
  
  //while(file.available()){
  for(int i=0; i<2090; i++){
    currentChar = file.read(); // read next character in file
    if(currentChar == ','){ // ',' marks where data is seperated
      pixelData = pixelValue.toInt(); // assign file data to array
      frameData[frameDataCount] = pixelData; // assign file data to array
      pixelValue = ""; // reset pixelValue string to nothing
      frameDataCount++; // incriment frame data location
    }
    else{
      pixelValue = pixelValue + currentChar; // concatonate character to string
    }
  }
  file.close();
  readTimeMicros = micros() - timer1; // end timer
  readTimeMillis = (millis() - timer2); // end timer
  Serial.print("\n\nRead Time Millis: ");
  Serial.print(readTimeMillis);
  Serial.print("\n\nRead Time Micro: ");
  Serial.print(readTimeMicros);
  Serial.print("\n\n");
  for(int i=0; i<2090; i++){
    //Serial.print(frameData[i]);
    //Serial.print("\n");
  }
  while(1); // end loop
  
}
