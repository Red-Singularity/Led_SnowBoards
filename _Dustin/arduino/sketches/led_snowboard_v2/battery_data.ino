void INA_setup() {
  uint8_t availableDevices = ina220.begin(MAX_CUR, SHUNT_R, INA_ADC_MODE_128AVG, INA_ADC_MODE_128AVG, INA_MODE_CONTINUOUS_BOTH, ina_addresses, NUM_INA);
  Serial.print("Configured "); Serial.print(availableDevices); Serial.print(" of "); Serial.print(NUM_INA); Serial.println(" INA220 current sensors");
  delay(100);
}

void getInaValues() {
  int bat_indicator; // for use with top led color

  voltage = ina220.getBusMilliVolts(0) / 1000.0; // voltage in volts
  current = ina220.getBusMicroAmps(0) / 1000000.0; // current in amps
  power = ina220.getBusMicroWatts(0) / 1000000.0; // power in watts
  //Serial.print("INA at 0x"); Serial.print(ina_addresses[0], HEX); Serial.print(" measures "); Serial.print(voltage); Serial.print(" V, ");
  //Serial.print(current); Serial.print(" A, and "); Serial.print(power); Serial.println(" mW");
}

void getCellValues(){
  cell1 = analogRead(cell)
}

void getBatteryData(){

  getInaValues(); // get voltage, current, and power readings
  getCellValues(); // get all cell voltage data

  if ((bat_safe == 1) && (voltage < 9.5)){ // disable lights if voltage is too low
    digitalWrite(GATE_SIGNAL, false);
  }

  //map max and min voltage to number of leds
  bat_indicator = map(voltage*10, 95, 126, 0, TOP_LEDS/2); // map function requires integers so voltage was multiplied by 10 to retain resolution

  //set red and green color for status
  int red = 255 + bat_indicator*(255/TOP_LEDS/2); //increase red value as battery gets lower
  int green = 255 - bat_indicator*(255/TOP_LEDS/2); //decrease green value as battery gets lower


  //set appropriate top leds to a color based on voltage
  for(int i=0; i<TOP_LEDS; i++){
    // SerialBT.print("leds on: ");
    // SerialBT.println(bat_indicator);
    top.setPixelColor(i, 0,green,red); // BGR
  }
}