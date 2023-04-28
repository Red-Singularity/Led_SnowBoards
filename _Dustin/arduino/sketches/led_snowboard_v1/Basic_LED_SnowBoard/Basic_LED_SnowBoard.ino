#include <Adafruit_NeoPixel.h>
#include "BluetoothSerial.h"

//by Dustin Fisher
//minimum led length is 13 across board up 17
//voltage divider resisitors: 220k and 68k

#define USE_ADC2      true

#define DATA_PIN 13 //sets pin data is being sent from
#define ledPower 15 //controls the mosfet for led strip power
#define statusLight 14 //green led light on board
#define batPin 33 //voltage divider data
#define NUM_LEDS 2090 // 1270 sets the amount of pixels being controlled

enum power{Off, On};
enum modes{White, Red, Green, Blue, Rand, None};
power state = On;
modes color = None;

const double vDiv = .2361; //68k/(68k+220k)
int loop_time = 0;
int batData = 4095;
int cutOff = 2500;
int Brightness;
float inputVoltage = 0;
float batVoltage = 0;

bool Danger = true;

Adafruit_NeoPixel pixels(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);
BluetoothSerial SerialBT;

void setup(){
  Serial.begin(57600);
  SerialBT.begin("LED_SnowBoard"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");

  analogReadResolution(12); // Default of 12 is not very linear. Recommended to use 10 or 11 depending on needed resolution.
  //analogSetAttenuation(ADC_6db); // Default is 11db which is very noisy. Recommended to use 2.5 or 6.
  
  pinMode(DATA_PIN, OUTPUT);
  pinMode(ledPower, OUTPUT);
  pinMode(statusLight, OUTPUT);
  pinMode(batPin, INPUT);
  
  pixels.begin();
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
      Brightness = 13;
    }
    else if(InData == 0x01){ //10% brightness
      Brightness = 26;
    }
    else if(InData == 0x02){ //25% brightness
      Brightness = 64;
    }
    else if(InData == 0x03){ //50% brightness
      Brightness = 127;
    }
    else if(InData == 0x04){ //75% brightness
      Brightness = 191;
    }
    else if(InData == 0x05){ //100% brightness
      Brightness = 255;
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
            pixels.setPixelColor(i, pixels.Color(Brightness,Brightness,Brightness));
          }
          break;
        case Red:
          for(int i = 0; i <= NUM_LEDS; i++){
            pixels.setPixelColor(i, pixels.Color(0,Brightness,0));
          }
          break;
          
        case Green:
          for(int i = 0; i <= NUM_LEDS; i++){
            pixels.setPixelColor(i, pixels.Color(Brightness,0,0));
          }
          break;
          
        case Blue:
          for(int i = 0; i <= NUM_LEDS; i++){
            pixels.setPixelColor(i, pixels.Color(0,0,Brightness));
          }
          break;
          
        case Rand:
          for(int i = 0; i <= NUM_LEDS; i++){
            pixels.setPixelColor(i, pixels.Color(random(Brightness),random(Brightness),random(Brightness)));
          }
          break;

        case None:
          for(int i = 0; i <= NUM_LEDS; i++){
            pixels.setPixelColor(i, pixels.Color(0,0,0));
          }
          break;
      }
        
      pixels.show();
      break;

   case Off:
      pixels.clear();
      digitalWrite(ledPower, LOW);
      digitalWrite(statusLight, LOW);
      break;
  }

}
