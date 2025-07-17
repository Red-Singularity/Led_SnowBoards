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

#include "SdFat.h" // interfaces with sd card
#include "sdios.h" // sd card comms
#include "SPI.h" // handles SPI comms
#include "BluetoothSerial.h" // Bluetooth comms
#include <Wire.h> // i2c communication
#include <INA220.h> // for power monitor https://github.com/nathancheek/INA220
#include <FastLED.h> // main led control

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
#define DATA_3 14 // sets pin data is being sent from
#define DATA_1 12 // sets pin data is being sent from
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
#define CS_RAM 2 // PSRAM chip select
#define BAT 15 // battery voltage voltage divider

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
#define SPI_CLOCK SD_SCK_MHZ(1)

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
CRGB half1[NUM_LEDS/2];
CRGB half2[NUM_LEDS/2];
CRGB top[TOP_LEDS];

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

bool bat_safe = 0; //flag for enabling or disabling low battery cutoff. 1 = safety enabled
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
float voltage, current, power; // ina chip values
float timer = 0; // generic timer
float minCell = 4.2; // minimum cell voltage reading

String accel_set = "16g"; // 2g, 4g, 8g, 16g
String gyro_set = "2000dps"; // 250dps, 500dps, 1000dps, 2000dps


void setup() {
  //setup serial output
  Serial.begin(115200);

  //set pin modes
  pinMode(DATA_1, OUTPUT); // set led data pins to output
  pinMode(DATA_2, OUTPUT); 
  pinMode(DATA_3, OUTPUT);
  pinMode(GATE_SIGNAL, OUTPUT); // turns on 12v to leds
  pinMode(BAT, INPUT);

  //device setups
  INA_setup(); //INA220 setup
  sd_setup(); //setup sd card
  bluetooth_setup(); //setup bluetooth
  //mpu_setup(); //setup mpu
  //setup GPS
  
  //setup led strips
  digitalWrite(GATE_SIGNAL, false);
  FastLED.addLeds<WS2812, DATA_1, BGR>(half1, NUM_LEDS);
  FastLED.addLeds<WS2812, DATA_2, BGR>(half2, NUM_LEDS);
  FastLED.addLeds<WS2812, DATA_3, BGR>(top, TOP_LEDS);
  FastLED.setCorrection(TypicalLEDStrip);
  //set brightness to 5%
  FastLED.setBrightness(5);
  FastLED.show();
  
}

void loop() {
  timer = millis();

  getInaValues(); //get board status (battery info, current measurements, etc)
  getBatteryData(); // get all battery data
  bluetooth_control(); // use data over bluetooth to control images
  //get MPU data
  //get GPS data
  //app();
  //get gps data

  FastLED.show();

  //Serial.println(millis()-timer);
}
