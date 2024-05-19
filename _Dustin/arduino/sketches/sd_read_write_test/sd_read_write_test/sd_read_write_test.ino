//Stuff from LED setup that's needed here:
int const screen_y_size = 13;                                                   //y-axis is on the width of the board
int const tip_x_size[screen_y_size] = {17,18,18,19,19,19,19,19,19,19,18,18,17}; //x-axis is on the length of the board
int const tip_x_max = 19;
int const middle_x_size = 21;
int const tail_x_size[screen_y_size] = {17,18,18,19,19,19,19,19,19,19,18,18,17};
int const tail_x_max = 19;
int const screen_x_size = tip_x_max + middle_x_size + tail_x_max;

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
#define SCLK_SPEED 10*1000000       //limited by esp32 and/or PCB design
uint8_t const RAM_MFID_PASS = 0x0D;
uint8_t const RAM_KGD_PASS = 0x5D;

uint8_t const write_command = 0x02;
uint8_t const fast_read_command = 0x03;
uint8_t const read_id_command = 0x9F;

uint8_t first_empty_address[3] = {0x00, 0x00, 0x00};
SPISettings RAM_SETTINGS(SCLK_SPEED,MSBFIRST,SPI_MODE0);

//Structs:
struct writtenData{
  uint8_t address[3];       //address of the first byte of data
  unsigned long data_size;  //size of the data written in bytes
};

struct readData{
  uint8_t data_array[screen_x_size*screen_y_size];  //largest piece of data I'll need to pull is one frame, which is this big
  unsigned long data_size;                          //size of the data read in bytes
};

//RAM functions:
void InitializeRAM(){
  pinMode(CS_RAM,OUTPUT);
  digitalWrite(CS_RAM,HIGH);
  SPI.begin();
  
  uint8_t read_IDbuff[] = {read_id_command, 0x00, 0x00, 0x00, 0x00, 0x00};
  digitalWrite(CS_RAM,LOW);
  SPI.beginTransaction(RAM_SETTINGS);
  SPI.transfer(read_IDbuff,sizeof(read_IDbuff));
  SPI.endTransaction();
  digitalWrite(CS_RAM,HIGH);

  uint8_t mfid = read_IDbuff[4];
  uint8_t kgd = read_IDbuff[5];

  #if RAM_DEBUG
    Serial.print("MF ID: "); Serial.println(mfid);
    Serial.print("KGD: "); Serial.println(kgd);
  #endif
  
  if((mfid != RAM_MFID_PASS) or (kgd != RAM_KGD_PASS)){
    //TO DO: throw RAM error
    
    #if RAM_DEBUG
      Serial.println("RAM ID read failed");
    #endif
  }
  else{
    #if RAM_DEBUG
      Serial.println("RAM ID read passed");
    #endif
  }
}

writtenData WriteRAM(uint8_t* array_pointer, unsigned long array_size){
  #if RAM_DEBUG
    Serial.print("Writing "); Serial.print(array_size);
    Serial.print(" byte array at "); Serial.print(first_empty_address[0]);
    Serial.print(", "); Serial.print(first_empty_address[1]);
    Serial.print(", "); Serial.println(first_empty_address[2]);
  #endif
  
  uint8_t write_buff[4 + array_size];
  unsigned long buff_index = 0;
  
  write_buff[buff_index] = write_command;
  buff_index++;
  
  for(int i = 0; i < 3; i++){
    write_buff[buff_index] = first_empty_address[i];
    buff_index++;
  }
  
  for(int i = 0; i < array_size; i++){
    write_buff[buff_index] = *(array_pointer + i);
    buff_index++;
  }

  digitalWrite(CS_RAM,LOW);
  SPI.beginTransaction(RAM_SETTINGS);
  SPI.transfer(write_buff, buff_index);
  SPI.endTransaction();
  digitalWrite(CS_RAM,HIGH);

  writtenData wdata;
  wdata.data_size = array_size;
  for(int i  = 0; i < 3; i++){
    wdata.address[i] = first_empty_address[i]; 
  }

  uint32_t concat_address = (uint32_t)first_empty_address[0] << 16 | (uint32_t)first_empty_address[1] << 8 | (uint32_t)first_empty_address[2];
  concat_address += array_size;

  first_empty_address[0] = (concat_address >> 16) & 0xFF;
  first_empty_address[1] = (concat_address >> 8) & 0xFF;
  first_empty_address[2] = concat_address & 0xFF;
  
  #if RAM_DEBUG
    Serial.print("Write done. Next free address at "); Serial.print(first_empty_address[0]);
    Serial.print(", "); Serial.print(first_empty_address[1]);
    Serial.print(", "); Serial.println(first_empty_address[2]);
  #endif
  
  return wdata;
}

readData ReadRAM(writtenData to_read){
  #if RAM_DEBUG
    Serial.print("Reading "); Serial.print(to_read.data_size);
    Serial.print(" byte array at "); Serial.print(to_read.address[0]);
    Serial.print(", "); Serial.print(to_read.address[1]);
    Serial.print(", "); Serial.println(to_read.address[2]);
  #endif
  
  uint8_t read_buff[4 + to_read.data_size];
  unsigned long buff_index = 0;
  
  read_buff[0] = fast_read_command;
  buff_index++;
  
  for(int i = 0; i < 3; i++){
    read_buff[buff_index] = to_read.address[i];
    buff_index++;
  }

  digitalWrite(CS_RAM,LOW);
  SPI.beginTransaction(RAM_SETTINGS);
  SPI.transfer(read_buff, 4 + to_read.data_size);
  SPI.endTransaction();
  digitalWrite(CS_RAM,HIGH);

  #if RAM_DEBUG
    Serial.print("Read done. Read data: ");
  #endif

  readData rdata;
  rdata.data_size = to_read.data_size;
  for(int i = 0; i < to_read.data_size; i++){
    rdata.data_array[i] = read_buff[buff_index];
    buff_index++;
    
    #if RAM_DEBUG
      if(i < to_read.data_size - 1){
        Serial.print(rdata.data_array[i]); Serial.print(", ");
      }
      else{
        Serial.println(rdata.data_array[i]);
      }
    #endif
  }
  
  return rdata;
}

void setup(){
  Serial.begin(115200);
  while(!Serial){}
  
  pinMode(CS_SD,OUTPUT);
  digitalWrite(CS_SD,HIGH);

  InitializeRAM();
  uint8_t data_to_write[2] = {0x02,0x0F};
  writtenData written_data = WriteRAM(&data_to_write[0],2);
  readData read_data = ReadRAM(written_data);
}

void loop(){
  
}
