
#include <NeoPixelBus.h>
#include <NeoPixelBusLg.h> // instead of NeoPixelBus.h

const uint16_t PixelCount = 260; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 12;  // make sure to set this to the correct pin, ignored for Esp8266

#define colorSaturation 75



NeoPixelBusLg<NeoGrbWs2816Feature, NeoWs2816Method> strip(PixelCount, PixelPin);

RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

HslColor hslRed(red);
HslColor hslGreen(green);
HslColor hslBlue(blue);
HslColor hslWhite(white);
HslColor hslBlack(black);


void setup()
{
    Serial.begin(115200);
    while (!Serial); // wait for serial attach

    Serial.println();
    Serial.println("Initializing...");
    Serial.flush();

    // this resets all the neopixels to an off state
    strip.Begin();
    strip.Show();


    Serial.println();
    Serial.println("Running...");
}


void loop(){
  for(int i=0; i<PixelCount; i++){
    strip.SetPixelColor(i,white);
  }
  strip.Show();

}

