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
#include "BluetoothSerial.h"
#include <Adafruit_NeoPixel.h>
#include <Wire.h>


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
#define MPU_addr 0x68
//setting for mpu degrees per second
#define dps2000 0x18
#define dps1000 0x10
#define dps500 0x08
#define dps250 0x00
//setting for mpu degrees per second
#define g16 0x18
#define g8 0x10
#define g4 0x08
#define g2 0x00

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

//initialize BluetoothSerial class as SerialBT
BluetoothSerial SerialBT;

//initialize led strips
Adafruit_NeoPixel half1(NUM_LEDS/2, DATA_1, NEO_GRB + NEO_KHZ800); // first half of bottom matrix
Adafruit_NeoPixel half2(NUM_LEDS/2, DATA_2, NEO_GRB + NEO_KHZ800); // second half of bottom matrix
Adafruit_NeoPixel top(TOP_LEDS, DATA_3, NEO_GRB + NEO_KHZ800); // top set of leds

// Create a Serial output stream.
ArduinoOutStream cout(Serial);

//Global variables
int frameData[2090]; // array of data for single frame
int chipSelect = 5;
int cal = 50; // amount of loops to get gyro data for calibration
int offsetx, offsety, offsetz; // offset values for gyro
int counter = 0; // generic counter for testing

float ax, ay, az; // accelerometer data in Gs
float gx, gy, gz; // gyro data in degrees/second
float comp_x, comp_y = 0; // complimentary filter angles
float gyro_angle_x, gyro_angle_y, gyro_angle_z = 0; // angle calculated with gyro
float temp_c; // temperature sensor on mpu. in degrees c
float mpu_timer = 0; // timer to get data from gyro in mpu

String accel_set = "16g"; // 2g, 4g, 8g, 16g
String gyro_set = "2000dps"; // 250dps, 500dps, 1000dps, 2000dps


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
  

  //setup mpu
  mpu_setup();

  //setup bluetooth
  bluetooth_setup();
  
  //setup led strips and initalize to 0
  
}

void loop() {
  //get board status (battery info, current measurements, etc)

  //comms over bluetooth
  app();

  //get data from sd card and send to leds
  //sd_image_read();

  //get gps data

  //get MPU data
  //mpu();
  
  //if going slow soft glow breathing effect. if faster turns red and goes faster. gradiant from front to back.
  //stoping effect? flame effect?

}
