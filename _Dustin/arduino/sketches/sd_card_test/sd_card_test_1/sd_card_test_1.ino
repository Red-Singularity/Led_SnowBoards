
#include "SdFat.h" // interfaces with sd card
#include "sdios.h"
#include "FS.h" // handles files

#define MOSI 23 // MOSI SPI pin
#define MISO 19 // MISO SPI pin
#define SCLK 18 // SPI clock pin
#define SD_CS_PIN 5 // SD card chip select
#define SPI_CLOCK SD_SCK_MHZ(1) // spi speed

//initialize sd card class as sd
SdFat SD;

//initialize File class as file
File file;

void setup() {
  // Wait for USB Serial
  Serial.begin(115200);
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
      while(1);
      return;
    }
  }
    
  Serial.println(" Card successfully initialized");

}

void loop() {
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
