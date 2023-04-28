
#define LED_CLOCK_PIN 3
#define LED_DATA_PIN 4

#define GREEN_BLUE_COLOR 0x001F1F
#define WHITE_COLOR 0xF4F3D0
#define GREEN_COLOR 0x6EDB55
#define RED_BLUE_COLOR 0xD237A5
#define RED_COLOR 0xFF0000
#define BLUE_COLOR 0x0000ff

void setup() {
  // put your setup code here, to run once:

}

void loop() {
   //display_color_on_RGB_led(RED_COLOR);
  // delay(2000);
   display_color_on_RGB_led(BLUE_COLOR);
   

}

void display_color_on_RGB_led(unsigned long color) {
  unsigned long bitmask = 0UL; // UL unsigned long literal (forces compiler to use long data type)
  unsigned long masked_color_result = 0UL;


  digitalWrite(LED_CLOCK_PIN, LOW); //start with clock low.


  for (int i = 23; i >= 0; i--) { // clock out one data bit at a time, starting with the MSB first
    bitmask = (1UL << i); // build bit mask. Note must use "1UL" unsigned long literal, not "1"
    masked_color_result = color & bitmask; // reveals just one bit of color at time
    boolean data_bit = !(masked_color_result == 0); // this is the bit of data to be clocked out.
    
    digitalWrite(LED_DATA_PIN, data_bit);
    digitalWrite(LED_CLOCK_PIN, HIGH);
    digitalWrite(LED_CLOCK_PIN, LOW);

  }

  digitalWrite(LED_CLOCK_PIN, HIGH);
  delay(1); // after writing data to LED driver, must hold clock line

  // high for 1 ms to latch color data in led shift register.
}//display_color_on_RGB_led()
