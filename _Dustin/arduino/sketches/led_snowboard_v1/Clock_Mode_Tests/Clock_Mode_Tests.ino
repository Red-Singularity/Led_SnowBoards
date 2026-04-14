//#include <Adafruit_NeoPixel.h>
#include "BluetoothSerial.h"
#include <FastLED.h>
#include <DS3231.h> // include library for RTC module

//by Dustin Fisher
//minimum led length is 13 across board up 17
//voltage divider resisitors: 220k and 68k

#define USE_ADC2      true

#define DATA_PIN 13 //sets pin data is being sent from
#define ledPower 12 //controls the mosfet for led strip power
#define statusLight 14 //green led light on board
#define batPin 33 //voltage divider data
#define NUM_LEDS 1270 // 1270 sets the amount of pixels being controlled

enum power{Off, On};
enum modes{White, Red, Green, Blue, Rand, None};
power state = On;
modes color = None;

const double vDiv = .2361; //68k/(68k+220k)
int loop_time = 0;
int batData = 4095;
int cutOff = 2500;
int Brightness = 26;
float inputVoltage = 0;
float batVoltage = 0;
bool Danger = true;

// RTC variables
bool century = false;
bool h12Flag = true;
bool pmFlag = false;

//Adafruit_NeoPixel pixels(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);
CRGB Board[NUM_LEDS]; // initialize led strip
BluetoothSerial SerialBT;
DS3231 myRTC; // create instance of the real time clock

void setup(){
  Serial.begin(115200);
  SerialBT.begin("LED_SnowBoard"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");

  Wire.begin(); // start I2C interface

  analogReadResolution(12); // Default of 12 is not very linear. Recommended to use 10 or 11 depending on needed resolution.
  //analogSetAttenuation(ADC_6db); // Default is 11db which is very noisy. Recommended to use 2.5 or 6.
  
  pinMode(DATA_PIN, OUTPUT);
  pinMode(ledPower, OUTPUT);
  pinMode(statusLight, OUTPUT);
  pinMode(batPin, INPUT);
  
  //pixels.begin();
  FastLED.addLeds<WS2812, DATA_PIN, BGR>(Board, NUM_LEDS);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(10);
  FastLED.show();

  state = On;
  color = Rand;
  digitalWrite(ledPower, HIGH);
  digitalWrite(statusLight, HIGH);

  myRTC.setMinute(35);
  myRTC.setHour(12);
}

void loop(){
  int topLeft[5] = {865, 866, 867, 868, 869}; // top left segment array
  int bottomLeft[5] = {859, 860, 861, 862, 863}; // bottom left segment array
  int topRight[5] = {787, 788, 789, 790, 791}; // top right segment array
  int bottomRight[5] = {781, 782, 783, 784, 785}; // bottom right segment array
  int bottom[5] = {857, 832, 831, 806, 805}; // bottom segment array
  int middle[5] = {851, 838, 825, 812, 799} ; // middle segment array
  int top[5] = {845, 844, 819, 818, 793}; // top segment array

  int x;

  int Day = myRTC.getDate();
  int Hour = myRTC.getHour(h12Flag, pmFlag);
  int Minute = myRTC.getMinute();
  int Second = myRTC.getSecond();

  int hoursTens = Minute / 10;
  int hoursOnes = Minute % 10; // tens digit is remainder when divided by 10

  int minutesTens = Second / 10;
  int minutesOnes = Second % 10; // tens digit is remainder when divided by 10

  for(int i=0; i<NUM_LEDS; i++){
    Board[i] = CRGB::Black;
  }
  Serial.print(Hour);
  Serial.print(" ");
  Serial.print(Minute);
  Serial.print(" ");
  Serial.print(hoursTens);
  Serial.print(" ");
  Serial.print(hoursOnes);
  Serial.print(";");
  Serial.print(minutesTens);
  Serial.print(" ");
  Serial.println(minutesOnes);

  displaySevenSeg(hoursTens, 0); // hours tens. pass digit to be displayed and position. 
  displaySevenSeg(hoursOnes, 1); // hours ones. pass digit to be displayed and position. 
  displaySevenSeg(minutesTens, 2); // minutes tens. pass digit to be displayed and position. 
  displaySevenSeg(minutesOnes, 3); // minutes ones. pass digit to be displayed and position. 
  Board[633] = CRGB::Red; // location of center "colon" of clock
  Board[627] = CRGB::Red; // location of center "colon" of clock
  FastLED.show(); // need to call multiple times due to faulty leds
  FastLED.show();
  FastLED.show();
  delay(1000);


}


void displaySevenSeg(int digit, int position){
  int topLeft[5] = {865, 866, 867, 868, 869}; // top left segment array
  int bottomLeft[5] = {859, 860, 861, 862, 863}; // bottom left segment array
  int topRight[5] = {787, 788, 789, 790, 791}; // top right segment array
  int bottomRight[5] = {781, 782, 783, 784, 785}; // bottom right segment array
  int bottom[5] = {857, 832, 831, 806, 805}; // bottom segment array
  int middle[5] = {851, 838, 825, 812, 799} ; // middle segment array
  int top[5] = {845, 844, 819, 818, 793}; // top segment array

  int x = 0;
  int offset;

  if(position == 1){ // hours ones position pixel location define
    offset = 10;
  }

  else if(position == 2){ // minutes tens position pixel location define
    offset = 20;
  }

  else if(position == 3){ // minutes ones position pixel location define
    offset = 30;
  }

  else{
    offset = 0;
  }

  for(int i=0; i<5; i++){
    topLeft[i] = topLeft[i]-(13*offset); // top left segment array
    bottomLeft[i] = bottomLeft[i]-(13*offset); // bottom left segment array
    topRight[i] = topRight[i]-(13*offset); // top right segment array
    bottomRight[i] = bottomRight[i]-(13*offset); // bottom right segment array
    bottom[i] = bottom[i]-(13*offset); // bottom segment array
    middle[i] = middle[i]-(13*offset); // middle segment array
    top[i] = top[i]-(13*offset); // top segment array
  }

  if(digit==0 || digit==4 || digit==5 || digit==6 || digit==8 || digit==9){
    for(int i=0; i<5; i++){ // top left segment
      x = topLeft[i];
      Board[x] = CRGB::Blue;
    }
  }

  if(digit==0 || digit==2 || digit==6 || digit==8){
    for(int i=0; i<5; i++){ // Bottom left segment
      x = bottomLeft[i];
      Board[x] = CRGB::Blue;
    }
  }

  if(digit==0 || digit==1 || digit==2 || digit==3 || digit==4 || digit==7 || digit==8|| digit==9){
    for(int i=0; i<5; i++){ // top right segment
      x = topRight[i];
      Board[x] = CRGB::Blue;
    }
  }

  if(digit==0 || digit==1 || digit==3 || digit==4 || digit==5 || digit==6 || digit==7 || digit==8 || digit==9){
    for(int i=0; i<5; i++){ // Bottom right segment
      x = bottomRight[i];
      Board[x] = CRGB::Blue;
    }
  }

  if(digit==0 || digit==2 || digit==3 || digit==5 || digit==6 || digit==8 || digit==9){
    for(int i=0; i<5; i++){ // bottom segment
      x = bottom[i];
      Board[x] = CRGB::Blue;
    }
  }

  if(digit==2 || digit==3 || digit==4 || digit==5 || digit==6 || digit==8 || digit==9){
    for(int i=0; i<5; i++){ // middle segment
      x = middle[i];
      Board[x] = CRGB::Blue;
    }
  }

  if(digit==0 || digit==2 || digit==3 || digit==5 || digit==6 || digit==7 || digit==8 || digit==9){
    for(int i=0; i<5; i++){ // top segment
      x = top[i];
      Board[x] = CRGB::Blue;
    }
  }

}

