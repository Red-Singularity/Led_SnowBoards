
#include <INA220.h>

/******************
 * Begin Configure
 ******************/
const uint8_t NUM_INA = 1; // 2 INA devices
const uint8_t MAX_CUR = 40; // 5 Amps
const uint16_t SHUNT_R = 1000; // 1 mOhm
uint8_t ina_addresses[NUM_INA] = {0x40}; // INA I2C addresses
/******************
 * End Configure
 ******************/

INA220 ina220;

void setup() {
  Serial.begin(115200);
  uint8_t availableDevices = ina220.begin(MAX_CUR, SHUNT_R, INA_ADC_MODE_128AVG, INA_ADC_MODE_128AVG, INA_MODE_CONTINUOUS_BOTH, ina_addresses, NUM_INA);
  Serial.print("Configured "); Serial.print(availableDevices); Serial.print(" of "); Serial.print(NUM_INA); Serial.println(" INA220 current sensors");
  delay(100);
}

void loop() {
  float vol = ina220.getBusMilliVolts(0) / 1000.0; // voltage in volts
  float cur = ina220.getBusMicroAmps(0) / 1000000.0; // current in amps
  float power = ina220.getBusMicroWatts(0) / 1000000.0; // power in watts
  Serial.print("INA at 0x"); Serial.print(ina_addresses[0], HEX); Serial.print(" measures "); Serial.print(vol); Serial.print(" V, ");
  Serial.print(cur); Serial.print(" A, and "); Serial.print(power); Serial.println(" mW");
}