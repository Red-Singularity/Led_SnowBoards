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
#define NUM_LEDS 1270 // 1270 sets the amount of pixels being controlled 15x91 max 

enum power{Off, On};
enum modes{White, Red, Green, Blue, Rand, Clock, None};
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
bool pmFlag;

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
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(Board, NUM_LEDS);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(10);
  FastLED.show();

  state = On;
  color = Clock;
  digitalWrite(ledPower, HIGH);
  digitalWrite(statusLight, HIGH);
}


void loop(){
  batData = analogRead(batPin);
  //Serial.print("BatData: ");
  //Serial.println(batData);
  //Serial.println(Danger);
  if((millis() > 5000) && (Danger == false) && (batData < cutOff)){
    state = Off;
    //Serial.println("trig");
  }
  
  Lights();
  Bluetooth();
}

void Bluetooth(){
  byte InData;
  int InNumber;

  
  while(SerialBT.available()){
    //Serial.write(SerialBT.read());
    InData = SerialBT.read();
    Serial.println(InData);
    if(InData == 0xFE){
      state = Off;
    }
    else if(InData == 0xFF){
      state = On;
    }
    else if(InData == 0x00){ //5% brightness
      FastLED.setBrightness(5);
    }
    else if(InData == 0x01){ //10% brightness
      FastLED.setBrightness(10);
    }
    else if(InData == 0x02){ //25% brightness
      FastLED.setBrightness(25);
    }
    else if(InData == 0x03){ //50% brightness
      FastLED.setBrightness(50);
    }
    else if(InData == 0x04){ //75% brightness
      FastLED.setBrightness(75);
    }
    else if(InData == 0x05){ //100% brightness
      FastLED.setBrightness(100);
    }
    else if(InData == 0x06){ //Red
      color = Red;
    }
    else if(InData == 0x07){ //Green
      color = Green;
    }
    else if(InData == 0x08){ //Blue
      color = Blue;
    }
    else if(InData == 0x09){ //Rand
      color = Rand;
    }
    else if(InData == 0x0B){ //White
      color = White;
    }
    else if(InData == 0x0C){ //Ignore Battery Protection
      Danger = true;
    }
    else if(InData == 0x0E){ //Ignore Battery Protection
      Danger = false;
    }
    else if(InData == 0x0F){ //send battery voltage data over terminal
      inputVoltage = (batData * (3.3/4096));
      batVoltage = inputVoltage / vDiv;
      SerialBT.print("Battery Voltage: ");
      SerialBT.print(batVoltage);
      Serial.print("constant: ");
      Serial.println(vDiv);
    }
    else if(InData == 0x10){ //set to clock mode
      color = Clock;
    }
    else if(InData == 0x11){ //set time
      SerialBT.print("Enter Hour: ");
      while(SerialBT.available()){SerialBT.read();} //flush all previous received and transmitted data
      while(SerialBT.available() == 0){} // wait for new data from bluetooth line
      InNumber = SerialBT.parseInt();
      Serial.println(InNumber);
      myRTC.setHour(InNumber);

      SerialBT.print("Enter Minute: ");
      while(SerialBT.available()){SerialBT.read();} //flush all previous received and transmitted data
      while(SerialBT.available() == 0){} // wait for new data from bluetooth line
      InNumber = SerialBT.parseInt();
      Serial.println(InNumber);
      myRTC.setMinute(InNumber);
    }
  }
}

void Lights(){
  //batData = analogRead(batPin);
  //Serial.println(state);
  switch(state){
    case On:
      digitalWrite(ledPower, HIGH);
      digitalWrite(statusLight, HIGH);
      switch(color){
        case White:
          for(int i = 0; i <= NUM_LEDS; i++){
            //pixels.setPixelColor(i, pixels.Color(Brightness,Brightness,Brightness));
            Board[i] = CRGB::White;
          }
          break;
        case Red:
          for(int i = 0; i <= NUM_LEDS; i++){
            //pixels.setPixelColor(i, pixels.Color(0,Brightness,0));
            Board[i] = CRGB::Red;
          }
          break;
          
        case Green:
          for(int i = 0; i <= NUM_LEDS; i++){
            //pixels.setPixelColor(i, pixels.Color(Brightness,0,0));
            Board[i] = CRGB::Green;
          }
          break;
          
        case Blue:
          for(int i = 0; i <= NUM_LEDS; i++){
            //pixels.setPixelColor(i, pixels.Color(0,0,Brightness));
            Board[i] = CRGB::Blue;
          }
          break;
          
        case Rand:
          for(int i = 0; i <= NUM_LEDS; i++){
            //pixels.setPixelColor(i, pixels.Color(random(Brightness),random(Brightness),random(Brightness)));
            Board[i] = random(0, 0xFFFFFF);
          }
          break;

        case Clock:
          clockMode();
          break;

        case None:
          for(int i = 0; i <= NUM_LEDS; i++){
            //pixels.setPixelColor(i, pixels.Color(0,0,0));
            Board[i] = CRGB::Black;
          }
          break;
      }
        
      //pixels.show();
      FastLED.show();
      break;

   case Off:
      //pixels.clear();
      digitalWrite(ledPower, LOW);
      digitalWrite(statusLight, LOW);
      break;
  }

}

void clockMode(){
  int Day = myRTC.getDate();
  int Hour = myRTC.getHour(h12Flag, pmFlag);
  int Minute = myRTC.getMinute();
  int Second = myRTC.getSecond();

  int hoursTens = Hour / 10;
  int hoursOnes = Hour % 10; // tens digit is remainder when divided by 10

  int minutesTens = Minute / 10;
  int minutesOnes = Minute % 10; // tens digit is remainder when divided by 10

  for(int i=0; i<NUM_LEDS; i++){
    Board[i] = CRGB::Black;
  }

  displaySevenSeg(hoursTens, 0); // hours tens. pass digit to be displayed and position. 
  displaySevenSeg(hoursOnes, 1); // hours ones. pass digit to be displayed and position. 
  displaySevenSeg(minutesTens, 2); // minutes tens. pass digit to be displayed and position. 
  displaySevenSeg(minutesOnes, 3); // minutes ones. pass digit to be displayed and position. 

  Board[633] = CRGB::Indigo; // location of center "colon" of clock
  Board[627] = CRGB::Indigo; // location of center "colon" of clock

  if(pmFlag == false){ //display "A" in AM if its morning
    Board[287] = CRGB::Indigo;
    Board[288] = CRGB::Indigo;
    Board[289] = CRGB::Indigo;
    Board[290] = CRGB::Indigo;
    Board[291] = CRGB::Indigo;
    Board[280] = CRGB::Indigo;
    Board[283] = CRGB::Indigo;
    Board[256] = CRGB::Indigo;
    Board[259] = CRGB::Indigo;
    Board[248] = CRGB::Indigo;
    Board[249] = CRGB::Indigo;
    Board[250] = CRGB::Indigo;
    Board[251] = CRGB::Indigo;
    Board[252] = CRGB::Indigo;
  }

  else if(pmFlag == true){ //display "P" in PM if its afternoon
    Board[287] = CRGB::Indigo;
    Board[288] = CRGB::Indigo;
    Board[289] = CRGB::Indigo;
    Board[290] = CRGB::Indigo;
    Board[291] = CRGB::Indigo;
    Board[280] = CRGB::Indigo;
    Board[283] = CRGB::Indigo;
    Board[256] = CRGB::Indigo;
    Board[259] = CRGB::Indigo;
    Board[251] = CRGB::Indigo;
    Board[252] = CRGB::Indigo;
  }

  //display "M" for AM and PM
  Board[223] = CRGB::Indigo;
  Board[222] = CRGB::Indigo;
  Board[221] = CRGB::Indigo;
  Board[220] = CRGB::Indigo;
  Board[219] = CRGB::Indigo;
  Board[218] = CRGB::Indigo;
  Board[198] = CRGB::Indigo;
  Board[197] = CRGB::Indigo;
  Board[192] = CRGB::Indigo;
  Board[191] = CRGB::Indigo;
  Board[169] = CRGB::Indigo;
  Board[168] = CRGB::Indigo;
  Board[165] = CRGB::Indigo;
  Board[164] = CRGB::Indigo;
  Board[163] = CRGB::Indigo;
  Board[162] = CRGB::Indigo;
  Board[161] = CRGB::Indigo;
  Board[160] = CRGB::Indigo;


  //check engine light
  Board[1226] = CRGB::Orange;
  Board[1225] = CRGB::Orange;
  Board[1224] = CRGB::Orange;
  Board[1223] = CRGB::Orange;
  Board[1222] = CRGB::Orange;
  Board[1221] = CRGB::Orange;
  Board[1220] = CRGB::Orange;
  Board[1208] = CRGB::Orange;
  Board[1196] = CRGB::Orange;
  Board[1195] = CRGB::Orange;
  Board[1194] = CRGB::Orange;
  Board[1193] = CRGB::Orange;
  Board[1192] = CRGB::Orange;
  Board[1191] = CRGB::Orange;
  Board[1190] = CRGB::Orange;
  Board[1181] = CRGB::Orange;
  Board[1175] = CRGB::Orange;
  Board[1167] = CRGB::Orange;
  Board[1166] = CRGB::Orange;
  Board[1160] = CRGB::Orange;
  Board[1151] = CRGB::Orange;
  Board[1152] = CRGB::Orange;
  Board[1144] = CRGB::Orange;
  Board[1142] = CRGB::Orange;
  Board[1139] = CRGB::Orange;
  Board[1137] = CRGB::Orange;
  Board[1129] = CRGB::Orange;
  Board[1128] = CRGB::Orange;
  Board[1123] = CRGB::Orange;
  Board[1114] = CRGB::Orange;
  Board[1113] = CRGB::Orange;
  Board[1112] = CRGB::Orange;
  Board[1109] = CRGB::Orange;
  Board[1108] = CRGB::Orange;
  Board[1107] = CRGB::Orange;
  Board[1098] = CRGB::Orange;
  Board[1093] = CRGB::Orange;
  Board[1084] = CRGB::Orange;
  Board[1083] = CRGB::Orange;
  Board[1082] = CRGB::Orange;
  Board[1079] = CRGB::Orange;
  Board[1077] = CRGB::Orange;
  Board[1068] = CRGB::Orange;
  Board[1063] = CRGB::Orange;
  Board[1054] = CRGB::Orange;
  Board[1052] = CRGB::Orange;
  Board[1047] = CRGB::Orange;
  Board[1046] = CRGB::Orange;
  Board[1038] = CRGB::Orange;
  Board[1033] = CRGB::Orange;
  Board[1026] = CRGB::Orange;
  Board[1025] = CRGB::Orange;
  Board[1015] = CRGB::Orange;
  Board[1010] = CRGB::Orange;
  Board[1009] = CRGB::Orange;
  Board[1008] = CRGB::Orange;
  Board[1001] = CRGB::Orange;
  Board[996] = CRGB::Orange;
  Board[986] = CRGB::Orange;
  Board[985] = CRGB::Orange;
  Board[984] = CRGB::Orange;
  Board[980] = CRGB::Orange;
  Board[979] = CRGB::Orange;
  Board[972] = CRGB::Orange;
  Board[965] = CRGB::Orange;
  Board[956] = CRGB::Orange;
  Board[955] = CRGB::Orange;
  Board[950] = CRGB::Orange;
  Board[949] = CRGB::Orange;
  Board[941] = CRGB::Orange;
  Board[940] = CRGB::Orange;
  Board[939] = CRGB::Orange;
  Board[938] = CRGB::Orange;
  Board[937] = CRGB::Orange;
  Board[936] = CRGB::Orange;

  //display battery
  Board[275] = CRGB::SeaGreen;
  Board[274] = CRGB::SeaGreen;
  Board[273] = CRGB::SeaGreen;
  Board[272] = CRGB::SeaGreen;

  Serial.println(Hour);
  Serial.println(pmFlag);
  Board[267] = CRGB::SeaGreen;
  if((Hour == 12 && pmFlag == false) || (Hour < 5 && pmFlag == false)){ // if between midnight and 5 am set lowest batt to blink
    if(Second % 2 == 1){
      Board[266] = CRGB::Red; // lowest batt
      Board[265] = CRGB::Red; // lowest batt
    }

    else{
      Board[266] = CRGB::Black; // lowest batt
      Board[265] = CRGB::Black; // lowest batt
    }
  }

  else{
    Board[266] = CRGB::Red; // lowest batt
    Board[265] = CRGB::Red; // lowest batt
  }
  Board[264] = CRGB::SeaGreen;

  Board[245] = CRGB::SeaGreen;
  Board[244] = CRGB::SeaGreen;
  Board[243] = CRGB::SeaGreen;
  Board[242] = CRGB::SeaGreen;

  Board[237] = CRGB::SeaGreen;
  if((Hour == 12 && pmFlag == false) || (Hour > 9 && pmFlag == true) || (Hour < 5 && pmFlag == false)){
    Board[236] = CRGB::Black; // mid batt
    Board[235] = CRGB::Black; // mid batt
  }

  else{
    Board[236] = CRGB::Yellow; // mid batt
    Board[235] = CRGB::Yellow; // mid batt
  }
  Board[234] = CRGB::SeaGreen;

  Board[215] = CRGB::SeaGreen;
  Board[214] = CRGB::SeaGreen;
  Board[213] = CRGB::SeaGreen;
  Board[212] = CRGB::SeaGreen;

  Board[207] = CRGB::SeaGreen;
  if((Hour == 12 && pmFlag == false) || (Hour > 4 && pmFlag == true) || (Hour < 5 && pmFlag == false)){
    Board[206] = CRGB::Black; // full batt
    Board[205] = CRGB::Black; // full batt
  }

  else{
    Board[206] = CRGB::Green; // full batt
    Board[205] = CRGB::Green; // full batt
  }
  Board[204] = CRGB::SeaGreen;

  Board[186] = CRGB::SeaGreen;
  Board[185] = CRGB::SeaGreen;
  Board[184] = CRGB::SeaGreen;
  Board[183] = CRGB::SeaGreen;

  Board[177] = CRGB::SeaGreen;
  Board[176] = CRGB::SeaGreen;

  FastLED.show(); // need to call multiple times due to faulty leds
  FastLED.show();
  FastLED.show();
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
      Board[x] = CRGB::Green;
    }
  }

  if(digit==0 || digit==2 || digit==6 || digit==8){
    for(int i=0; i<5; i++){ // Bottom left segment
      x = bottomLeft[i];
      Board[x] = CRGB::Green;
    }
  }

  if(digit==0 || digit==1 || digit==2 || digit==3 || digit==4 || digit==7 || digit==8|| digit==9){
    for(int i=0; i<5; i++){ // top right segment
      x = topRight[i];
      Board[x] = CRGB::Green;
    }
  }

  if(digit==0 || digit==1 || digit==3 || digit==4 || digit==5 || digit==6 || digit==7 || digit==8 || digit==9){
    for(int i=0; i<5; i++){ // Bottom right segment
      x = bottomRight[i];
      Board[x] = CRGB::Green;
    }
  }

  if(digit==0 || digit==2 || digit==3 || digit==5 || digit==6 || digit==8 || digit==9){
    for(int i=0; i<5; i++){ // bottom segment
      x = bottom[i];
      Board[x] = CRGB::Green;
    }
  }

  if(digit==2 || digit==3 || digit==4 || digit==5 || digit==6 || digit==8 || digit==9){
    for(int i=0; i<5; i++){ // middle segment
      x = middle[i];
      Board[x] = CRGB::Green;
    }
  }

  if(digit==0 || digit==2 || digit==3 || digit==5 || digit==6 || digit==7 || digit==8 || digit==9){
    for(int i=0; i<5; i++){ // top segment
      x = top[i];
      Board[x] = CRGB::Green;
    }
  }

}
