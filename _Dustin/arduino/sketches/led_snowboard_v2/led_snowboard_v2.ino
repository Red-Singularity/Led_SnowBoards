/*
Dustin Fisher

main code for the led matrix snowboard
This code controls my version of the led matrix snowboard. this includes an indivdually addressable
array fo 2090 leds on the base of the board which is actually split between two data pins on the esp32.
This board also has a set of leds on its top for miscellaious purposes which uses a single data pin.
This design utelizes a GPS for determining speed and a mpu-6050 for acceleromrter / gyroscope data. The
data for the bottom matrix will be stored on an sd card which is read frame by frame then displayed. the
rate at which it is displayed and various effects can be applied based on data collected from the mpu and GPS
*/

#include "SdFat.h"
#include "sdios.h"
#include <Adafruit_NeoPixel.h>

//pin defines
#define DATA_1 12 //sets pin data is being sent from
#define DATA_2 13 //sets pin data is being sent from
#define DATA_3 14 //sets pin data is being sent from
#define GATE_SIGNAL 32 //controls the mosfet for led strip power
#define DIVIDER 25 // voltage divider pin
#define LIGHTS_STATUS 15 // board status led

// #define SD_CS_PIN 5 //SPI CS pin

//other defines
#define NUM_LEDS 2090 //2090 sets the amount of pixels being controlled
#define TOP_LEDS 50

//spi speed
#define SPI_CLOCK SD_SCK_MHZ(19)

// Try to select the best SD card configuration.
#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif  ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#else  // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
#endif  // HAS_SDIO_CLASS


//initialize sd card class as sd
SdFat SD;

//initialize File class as file
File file;

//initialize led strips
Adafruit_NeoPixel half1(NUM_LEDS/2, DATA_1, NEO_GRB + NEO_KHZ800); // first half of bottom matrix
Adafruit_NeoPixel half2(NUM_LEDS/2, DATA_2, NEO_GRB + NEO_KHZ800); // second half of bottom matrix
Adafruit_NeoPixel top(TOP_LEDS, DATA_3, NEO_GRB + NEO_KHZ800); // top set of leds

// Create a Serial output stream.
ArduinoOutStream cout(Serial);

//Global variables
int frameData[2090]; // array of data for single frame
int chipSelect = 5;

void setup() {
  //setup serial output
  Serial.begin(500000);

  //set pin modes
  pinMode(DATA_1, OUTPUT); // set led data pins to output
  pinMode(DATA_2, OUTPUT); 
  pinMode(DATA_3, OUTPUT);
  pinMode(GATE_SIGNAL, OUTPUT); // turns on 12v to leds
  pinMode(LIGHTS_STATUS, OUTPUT); // board status led
  pinMode(DIVIDER, INPUT); // voltage divider

  //setup sd card
  sd_setup();

  //setup GPS

  //setup led strips and initalize to 0
  
}

void loop() {
  //get board status

  //comms over bluetooth

  //get data from sd card and send to leds
  sd_image_read();

  //get gps data

  //get MPU data

  //if going slow soft glow. if faster turns red and goes faster. gradiant from front to back. flame effect?
  //stop effect

}
