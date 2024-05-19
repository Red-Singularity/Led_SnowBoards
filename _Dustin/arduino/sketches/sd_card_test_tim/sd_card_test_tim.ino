//Define compile debug variables:
#define RAM_DEBUG true

//Include libraries to handle peripherals:
#include <SPI.h>

//Define pins on the left side (EN button side) of ESP32:
#define CELL1         36
#define CELL2         39
#define CELL3         34
#define ANALOG_SENSOR 35
#define GATE          32
#define ENCODER_SW    33
#define ENCODER_DT    25
#define ENCODER_CLK   26
#define SD_SW         27
#define LED3          14
#define LED1          12
#define LED2          13

//Define pins on the right side (BOOT button side) of ESP32:
#define MOSI    23
#define SCL     22
#define SDA     21
#define MISO    19
#define SCLK    18
#define CS_SD   5
#define TX      17
#define RX      16
#define INT     4
#define CS_RAM  2

//RAM setup:
SPISettings RAM_SETTINGS(1000000,MSBFIRST,SPI_MODE1);

void setup(){
  Serial.begin(115200);
  while(!Serial){}
  pinMode(CS_RAM,OUTPUT);
  pinMode(CS_SD,OUTPUT);
  digitalWrite(CS_RAM,HIGH);
  digitalWrite(CS_SD,HIGH);
  SPI.begin();
}

uint8_t write_buff[] = {0x02, 0x00, 0x00, 0x00, 0x69};

void loop(){
  //write
  digitalWrite(CS_RAM,LOW);
  SPI.beginTransaction(RAM_SETTINGS);
  SPI.transfer(write_buff, sizeof(write_buff));
  SPI.endTransaction();
  digitalWrite(CS_RAM,HIGH);

  delay(500);

  //read
  uint8_t read_buff[] = {0x03, 0x00, 0x00, 0x00, 0xFF};
  uint8_t read_buff_size = sizeof(read_buff);
  digitalWrite(CS_RAM,LOW);
  SPI.beginTransaction(RAM_SETTINGS);
  SPI.transfer(read_buff,read_buff_size);
  SPI.endTransaction();
  Serial.print("DATA: "); Serial.println(read_buff[4]);
  digitalWrite(CS_RAM,HIGH);
  delay(500);
}
