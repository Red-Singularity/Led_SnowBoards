
#include <NeoPixelAnimator.h>
#include <NeoPixelBrightnessBus.h>
#include <NeoPixelBus.h>
#include <NeoPixelBusLg.h>
#include <NeoPixelSegmentBus.h>


#define DATA_1 12 // sets pin data is being sent from
#define DATA_2 14 // sets pin data is being sent from
#define NUM_LEDS 2090 //2090 sets the amount of pixels being controlled

//initialize led strips
// The WS2816 uses GRB color order
NeoPixelBus<NeoGrbFeature, NeoWs2816Method> strip1(NUM_LEDS, DATA_1);
NeoPixelBus<NeoGrbFeature, NeoWs2816Method> strip2(NUM_LEDS, DATA_2);

RgbColor red(255, 0, 0);
RgbColor green(0, 255, 0);
RgbColor blue(0, 0, 255);
RgbColor white(255);
RgbColor black(0);

HslColor hslRed(red);
HslColor hslGreen(green);
HslColor hslBlue(blue);
HslColor hslWhite(white);
HslColor hslBlack(black);


void setup() {

  //setup serial output
  Serial.begin(115200);

  //set pin modes
  pinMode(DATA_1, OUTPUT); // set led data pins to output
  pinMode(DATA_2, OUTPUT); 

  //setup led strips
  // this resets all the neopixels to an off state
  strip1.Begin();
  strip2.Begin();
  strip1.Show();
  strip2.Show();


}

void loop() {

    // set the color red
    for(int i=0; i<NUM_LEDS; i++){
      strip1.SetPixelColor(i, red);
      strip2.SetPixelColor(i, red);
    }
    strip1.Show();
    strip2.Show();
    delay(3000);

    // set color to black
    for(int i=0; i<NUM_LEDS; i++){
      strip1.SetPixelColor(i, 0);
      strip2.SetPixelColor(i, 0);
    }

    // set the color green 
    for(int i=0; i<NUM_LEDS; i++){
      strip1.SetPixelColor(i, green);
      strip2.SetPixelColor(i, green);
    }
    strip1.Show();
    strip2.Show();
    delay(3000);

    // set color to black
    for(int i=0; i<NUM_LEDS; i++){
      strip1.SetPixelColor(i, 0);
      strip2.SetPixelColor(i, 0);
    }

    // set the colors, 
    for(int i=0; i<NUM_LEDS; i++){
      strip1.SetPixelColor(i, blue);
      strip2.SetPixelColor(i, blue);
    }
    strip1.Show();
    strip2.Show();
    delay(3000);

    // set color to black
    for(int i=0; i<NUM_LEDS; i++){
      strip1.SetPixelColor(i, 0);
      strip2.SetPixelColor(i, 0);
    }

}
