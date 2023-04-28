/*
Dustin Fisher

Test for reading and writing files to an sd card with esp32
*/

#include "FS.h" // handles files
#include "SdFat.h" // interfaces with sd card
#include "SPI.h" // handles SPI comms
#include "sdios.h"

// pin numbers refer to esp32 gpio number
#define led0 12
#define led1 27
#define led2 25
#define led3 32

// Test with reduced SPI speed for breadboards.  SD_SCK_MHZ(4) will select
// the highest speed supported by the board that is not over 4 MHz.
// Change SPI_SPEED to SD_SCK_MHZ(50) for best performance.
#define SPI_SPEED SD_SCK_MHZ(4)

// SD card chip select
const uint8_t SD_CS_PIN = 5;

SdFat sd; // initialize sd library as sd
File file;
File root;

//Global variables
int frameData[2090]; // array of data for single frame

// Serial streams
ArduinoOutStream cout(Serial);

void setup() {
  //setup serial output
  Serial.begin(500000);

  // Wait for USB Serial
  while (!Serial) {
    yield();
  }
delay(1000);
  cout << F("Type any character to start\n");
  while (!Serial.available()) {
    yield();
  }

  // Initialize the SD card.
  if (!sd.begin(SD_CONFIG)) {
    sd.initErrorHalt(&Serial);
  }
  if (sd.exists("Folder1")
    || sd.exists("Folder1/file1.txt")
    || sd.exists("Folder1/File2.txt")) {
    error("Please remove existing Folder1, file1.txt, and File2.txt");
  }

  int rootFileCount = 0;
  if (!root.open("/")) {
    error("open root");
  }
  while (file.openNext(&root, O_RDONLY)) {
    if (!file.isHidden()) {
      rootFileCount++;
    }
    file.close();
    if (rootFileCount > 10) {
      error("Too many files in root. Please use an empty SD.");
    }
  }
  if (rootFileCount) {
    cout << F("\nPlease use an empty SD for best results.\n\n");
    delay(1000);
  }
  // Create a new folder.
  if (!sd.mkdir("Folder1")) {
    error("Create Folder1 failed");
  }
  cout << F("Created Folder1\n");

  // Create a file in Folder1 using a path.
  if (!file.open("Folder1/file1.txt", O_WRONLY | O_CREAT)) {
    error("create Folder1/file1.txt failed");
  }
  file.close();
  cout << F("Created Folder1/file1.txt\n");

  // Change volume working directory to Folder1.
  if (!sd.chdir("Folder1")) {
    error("chdir failed for Folder1.\n");
  }
  cout << F("chdir to Folder1\n");

  // Create File2.txt in current directory.
  if (!file.open("File2.txt", O_WRONLY | O_CREAT)) {
    error("create File2.txt failed");
  }
  file.close();
  cout << F("Created File2.txt in current directory\n");

  cout << F("\nList of files on the SD.\n");
  sd.ls("/", LS_R);

  // Remove files from current directory.
  if (!sd.remove("file1.txt") || !sd.remove("File2.txt")) {
    error("remove failed");
  }
  cout << F("\nfile1.txt and File2.txt removed.\n");

  // Change current directory to root.
  if (!sd.chdir()) {
    error("chdir to root failed.\n");
  }

  cout << F("\nList of files on the SD.\n");
  sd.ls(LS_R);

  // Remove Folder1.
  if (!sd.rmdir("Folder1")) {
    error("rmdir for Folder1 failed\n");
  }
  cout << F("\nFolder1 removed.\n");
  cout << F("\nList of files on the SD.\n");
  sd.ls(LS_R);
  cout << F("Done!\n");

}

void loop() {
  char currentChar;
  String pixelValue = ""; // string of integer values 
  int frameDataCount = 0;
  int pixelData = 0;
  float timer = 0;
  float readTime = 0;
  file.open("/test/image0.dat");

  //timer = micros(); // start timer1
  
  
  //while(file.available()){
  for(int i=0; i<2090; i++){
    timer = micros(); // start timer1
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
    readTime = (micros() - timer)/1000; // end timer
  }
  file.close();
  //readTime = (micros() - timer)/1000; // end timer
  Serial.printf("\n\nRead Time: %f\n\n", readTime);
  for(int i=0; i<2090; i++){
    //Serial.print(frameData[i]);
    //Serial.print("\n");
  }
  while(1); // end loop
  
}
