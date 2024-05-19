void INA_setup() {
  uint8_t availableDevices = ina220.begin(MAX_CUR, SHUNT_R, INA_ADC_MODE_128AVG, INA_ADC_MODE_128AVG, INA_MODE_CONTINUOUS_BOTH, ina_addresses, NUM_INA);
  Serial.print("Configured "); Serial.print(availableDevices); Serial.print(" of "); Serial.print(NUM_INA); Serial.println(" INA220 current sensors");
  delay(100);
}

void getInaValues() {
  voltage = ina220.getBusMilliVolts(0) / 1000.0; // voltage in volts
  current = (ina220.getBusMicroAmps(0) / 1000000.0)/3; // current in amps
  power = (ina220.getBusMicroWatts(0) / 1000000.0)/3; // power in watts
  //Serial.print("INA at 0x"); Serial.print(ina_addresses[0], HEX); Serial.print(" measures "); Serial.print(voltage); Serial.print(" V, ");
  //Serial.print(current); Serial.print(" A, and "); Serial.print(power); Serial.println(" mW");
}

void getCellValues(){
  float gain = 100/49.9; //gain for the op-amp
  float gainCorrect1 = 1.0824;
  float gainCorrect2 = 1.0755;
  float gainCorrect3 = 1.0789;

  float cell_1, cell_2, cell_3;

  //read cell values
  cell_1 = analogRead(CELL_1);
  cell_2 = analogRead(CELL_2);
  cell_3 = analogRead(CELL_3);

  //turn analog values into voltages
  cell_1 = ((cell_1*(3.3/4095))*gain)*gainCorrect1;
  cell_2 = ((cell_2*(3.3/4095))*gain)*gainCorrect2;
  cell_3 = ((cell_3*(3.3/4095))*gain)*gainCorrect3;

  //output data to terminal
  /*
  Serial.print("\nCell 1 Voltage: ");
  Serial.print(cell_1);
  Serial.print("   Cell 2 Voltage: ");
  Serial.print(cell_2);
  Serial.print("   Cell 3 Voltage: ");
  Serial.println(cell_3);
  */

  //find min cell value
  if((cell_1 <= cell_2) && (cell_1 < cell_3)){
    minCell = cell_1;
  }

  else if((cell_2 <= cell_1) && (cell_2 < cell_3)){
    minCell = cell_2;
  }

  else if((cell_3 <= cell_1) && (cell_3 < cell_2)){
    minCell = cell_3;
  }

  else{
    Serial.print("CELL ERROR. Deriving from average");
    minCell = (cell_1+cell_2+cell_3)/3;
  }
}

void getBatteryData(){
  int bat_indicator; // for use with top led color

  getInaValues(); // get voltage, current, and power readings
  getCellValues(); // get all cell voltage data

  //Serial.print("Minimum cell voltage: "); Serial.println(minCell);
  //Serial.print("Voltage: "); Serial.println(voltage);

  if (((bat_safe == 1) && (minCell < 2.9))){ // disable lights if voltage is too low
    digitalWrite(GATE_SIGNAL, false);
  }

  //map max and min voltage to number of leds
  //bat_indicator = map(voltage*10, 95, 126, 0, TOP_LEDS/2); // map function requires integers so voltage was multiplied by 10 to retain resolution
  bat_indicator = map(minCell*100, 290, 420, 0, 255); 

  //set red and blue color for status
  int red = 255 - bat_indicator; //increase red value as battery gets lower
  int blue = 255 + bat_indicator; //decrease blue value as battery gets lower

  //set appropriate top leds to a color based on voltage
  for(int i=0; i<TOP_LEDS; i++){
    top[i] = CRGB(0, blue, red);
  }
}