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

//#include "FS.h" // handles files
#include "SdFat.h" // interfaces with sd card
#include "sdios.h"
#include "SPI.h" // handles SPI comms
#include "BluetoothSerial.h"
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <INA220.h>

//#include "pacman.h"
//#include "mario.h"
//#include "nyan.h"


//pin defines
#define CELL_1 36 // analog cell 1 data
#define CELL_2 39 // analog cell 2 data
#define CELL_3 34 // analog cell 3 data
#define SENSOR 35 // analog read sensor data
#define GATE_SIGNAL 32 //controls the mosfet for led strip power
#define SW 33 // Encoder switch
#define DT 25 // Encoder DT pin
#define CLK 26 // Encoder clock pin
#define SD_SW 27 // SD detect switch
#define DATA_3 12 // sets pin data is being sent from
#define DATA_1 14 // sets pin data is being sent from
#define DATA_2 13 // sets pin data is being sent from
#define MOSI 23 // MOSI SPI pin
#define SCL 22 // I2C clock pin
#define SDA 21 // I2C data pin
#define MISO 19 // MISO SPI pin
#define SCLK 18 // SPI clock pin
#define SD_CS_PIN 5 // SD card chip select
#define TX 17 // UART transmit pin
#define RX 16 // UART receive pin
#define INT 26 // mpu6050 interrupt
#define CS_RAM // PSRAM chip select

//other defines
#define NUM_LEDS 2090 //2090 sets the amount of pixels being controlled
#define TOP_LEDS 60
#define MPU_addr 0x68 //i2c address of mpu
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
#define SPI_CLOCK SD_SCK_MHZ(15)

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

//INA220 library initialization as ina220
INA220 ina220;

//initialize led strips
Adafruit_NeoPixel half1(NUM_LEDS/2, DATA_1, NEO_BGR + NEO_KHZ800); // first half of bottom matrix
Adafruit_NeoPixel half2(NUM_LEDS/2, DATA_2, NEO_BGR + NEO_KHZ800); // second half of bottom matrix
Adafruit_NeoPixel top(TOP_LEDS, DATA_3, NEO_BGR + NEO_KHZ800); // top set of leds

// Create a Serial output stream.
//ArduinoOutStream cout(Serial);

//Global variables
int frameData[NUM_LEDS]; // array of data for single frame
int cal = 50; // amount of loops to get gyro data for calibration
int offsetx, offsety, offsetz; // offset values for gyro
int counter = 0; // generic counter for testing
int frame_number = 0; // sets the frame being displayed for the current gif

const uint8_t NUM_INA = 1; // 2 INA devices
const uint8_t MAX_CUR = 40; // 5 Amps
const uint16_t SHUNT_R = 1000; // 1 mOhm

uint8_t ina_addresses[NUM_INA] = {0x40}; // INA I2C addresses

bool bat_safe = 1; //flag for enabling or disabling low battery cutoff. 1 = safety enabled
bool rand_LED = 0; // flag for random sequence animation
bool nyan = 0; // flag for setting nyan cat animation
bool freefall = 0; // flag for freefall detection
bool pacman = 0;
bool kirby = 0;
bool mario = 0;
bool neon = 0;

float ax, ay, az; // accelerometer data in Gs
float gx, gy, gz; // gyro data in degrees/second
float comp_x, comp_y = 0; // complimentary filter angles
float gyro_angle_x, gyro_angle_y, gyro_angle_z = 0; // angle calculated with gyro
float temp_c; // temperature sensor on mpu. in degrees c
float mpu_timer = 0; // timer to get data from gyro in mpu
float voltage; // main battery voltage
float current; // total current draw
float power; // total power draw
float timer = 0; // generic timer

String accel_set = "16g"; // 2g, 4g, 8g, 16g
String gyro_set = "2000dps"; // 250dps, 500dps, 1000dps, 2000dps

String message = "";
char incomingChar;


void setup() {
  //setup serial output
  Serial.begin(115200);

  //set pin modes
  pinMode(DATA_1, OUTPUT); // set led data pins to output
  pinMode(DATA_2, OUTPUT); 
  pinMode(DATA_3, OUTPUT);
  pinMode(GATE_SIGNAL, OUTPUT); // turns on 12v to leds

  //device setups
  INA_setup(); //INA220 setup
  sd_setup(); //setup sd card
  bluetooth_setup(); //setup bluetooth
  //setup GPS
  //mpu_setup(); //setup mpu
  
  //setup led strips and initalize to 0
  digitalWrite(GATE_SIGNAL, false);
  half1.begin();
  half2.begin();
  top.begin();
  for(int i=0; i<(NUM_LEDS/2); i++){
    half1.setPixelColor(i, half1.Color(0,0,0));
    half2.setPixelColor(i, half2.Color(0,0,0));
  }
  for(int i=0; i<TOP_LEDS; i++){
    top.setPixelColor(i, top.Color(0,0,0));
  }

  half1.show();
  half2.show();
  top.show();
  
}

void loop() {
  timer = millis();

  GetInaValues(); //get board status (battery info, current measurements, etc)
  bluetooth_control(); // use data over bluetooth to control images
  //sd_image_read(); //get data from sd card and send to leds
  //app();
  //get gps data
  //get MPU data

  half1.show();
  half2.show();
  top.show();

  //Serial.println(millis()-timer);
}
