
void mpu_setup(){
  byte data_r = 0xFF; //received data
  byte data_s = 0xFF; // sent data
  
  Serial.println("\n\nConnecting to MPU6050");
  Wire.begin(); // connect to mpu]

  //read from WHO_AM_I register to see if mpu is connected
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x75); // write to who am I register to get response
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,1,true); // read register data
  data_r = Wire.read();
  Wire.endTransmission(false);
  if(data_r == 0x68){
    Serial.println("\nGyro connected!\n");
  }
  else{
    Serial.println("\nMPU not detected. Wiring issue?");
    while(1);
  }

  //set power register to get out of sleep
  Serial.println("Setting power register to get out of sleep mode\n");
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); // write to gyro config register
  Wire.write(0b00000000); //set sleep mode to disabled
  Wire.endTransmission();
  
  //set gyro settings
  Serial.println("Setting MPU parameters:");

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x1B); // write to gyro config register
  if(gyro_set == "250dps"){
    Wire.write(dps2000); //set degrees per second
  }
  else if(gyro_set == "500dps"){
    Wire.write(dps2000); //set degrees per second
  }
  else if(gyro_set == "1000dps"){
    Wire.write(dps2000); //set degrees per second
  }
  else if(gyro_set == "2000dps"){
    Wire.write(dps2000); //set degrees per second
  }
  else{
    Serial.print("Improper gyro_set value");
    while(1);
  }
  Wire.endTransmission();
  
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x1B); // write to register to get response
  Wire.endTransmission();
  Wire.requestFrom(MPU_addr,1,true); // read register data
  data_r = Wire.read();
  //Serial.println(data_r);
  Wire.endTransmission();
  if(data_r == dps2000){
    Serial.println("Gyro set to 2000 degrees per second");
  }
  else if(data_r == dps1000){
    Serial.println("Gyro set to 1000 degrees per second");
  }
  else if(data_r == dps500){
    Serial.println("Gyro set to 500 degrees per second");
  }
  else if(data_r == dps250){
    Serial.println("Gyro set to 250 degrees per second");
  }
  else{
    Serial.println("Couldn't read data from gyro config register");
    while(1);
  }


  //set accel settings
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x1C); // write to accel config register
  //set accelerometer parameters based on user preference
  if(accel_set == "2g"){
    Wire.write(g2); //set G limit
  }
  else if(accel_set == "4g"){
    Wire.write(g4); //set G limit
  }
  else if(accel_set == "8g"){
    Wire.write(g8); //set G limit
  }
  else if(accel_set == "16g"){
    Wire.write(g16); //set G limit
  }
  else{
    Serial.print("Improper accel_set value");
    while(1);
  }
  Wire.endTransmission();
  
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x1C); // write to register to get response
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,1,true); // read register data
  data_r = Wire.read();
  //Serial.println(data_r);
  Wire.endTransmission();
  if(data_r == g16){
    Serial.println("Accel set to 16g");
  }
  else if(data_r == g8){
    Serial.println("Accel set to 8g");
  }
  else if(data_r == g4){
    Serial.println("Accel set to 4g");
  }
  else if(data_r == g2){
    Serial.println("Accel set to 2g");
  }
  else{
    Serial.println("Couldn't read data from accel config register");
    while(1);
  }

  Serial.println("getting gyro offsets");
  calibrate_gyro();
  
}



//===============================================================================================


void mpu(){
  get_gyro_data(); // get raw data from gyro in degrees per second
  get_accel_data(); // get raw data from accel in m/s^2
  get_temp(); // get temperature from module in degrees c
}

float get_gyro_data(){
  // return Gyro data in degrees per second
  
  int16_t gyrox_data;
  int16_t gyroy_data;
  int16_t gyroz_data;
  float scale;
  
  //get gyro data
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x43); // write to register to get response
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,6,true); // read register data
  gyrox_data = Wire.read()<<8|Wire.read(); //combine 2 data registers into one variable
  gyroy_data = Wire.read()<<8|Wire.read(); //combine 2 data registers into one variable
  gyroz_data = Wire.read()<<8|Wire.read(); //combine 2 data registers into one variable
  Wire.endTransmission(false);

  //take 2's compliment to get proper data. needs 16 bit variable to work
  gyrox_data = ~gyrox_data+1;
  gyrox_data = gyrox_data + offsetx; // apply offset
  gyroy_data = ~gyroy_data+1;
  gyroy_data = gyroy_data + offsety; // apply offset
  gyroz_data = ~gyroz_data+1;
  gyroz_data = gyroz_data + offsetz; // apply offset

  if(gyro_set == "250dps"){
    scale = 131;
  }
  else if(gyro_set == "500dps"){
    scale = 65.5;
  }
  else if(gyro_set == "1000dps"){
    scale = 32.8;
  }
  else if(gyro_set == "2000dps"){
    scale = 16.4;
  }

  //assign final gyro values to global variable. in degrees per second
  gx = gyrox_data/scale;
  gy = gyroy_data/scale;
  gz = gyroz_data/scale;
  
  //Serial.print(" Gyro X Registers: "); Serial.print(gx);
  //Serial.print(" Gyro Y Registers: "); Serial.print(gy);
  //Serial.print(" Gyro Z Registers: "); Serial.println(gz);
}

float get_accel_data(){
  // return accel data in m/s^2
  int16_t accelx_data;
  int16_t accely_data;
  int16_t accelz_data;
  float scale;
  
  //get gyro data
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B); // write to register to get response
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,6,true); // read register data
  accelx_data = Wire.read()<<8|Wire.read(); //combine 2 data registers into one variable
  accely_data = Wire.read()<<8|Wire.read(); //combine 2 data registers into one variable
  accelz_data = Wire.read()<<8|Wire.read(); //combine 2 data registers into one variable
  Wire.endTransmission(false);

  //take 2's compliment to get proper data. needs 16 bit variable to work
  accelx_data = ~accelx_data+1;
  accely_data = ~accely_data+1;
  accelz_data = ~accelz_data+1;

  if(accel_set == "16g"){
    scale = 16384;
  }
  else if(accel_set == "8g"){
    scale = 8192;
  }
  else if(accel_set == "4g"){
    scale = 4096;
  }
  else if(accel_set == "2g"){
    scale = 2048;
  }

  //assign final accel values to global variable. in m/s^2
  ax = (accelx_data/scale)*9.81;
  ay = (accely_data/scale)*9.81;
  az = (accelz_data/scale)*9.81;
  
  //Serial.print(" Accel X Registers: "); Serial.print(ax);
  //Serial.print(" Accel Y Registers: "); Serial.print(ay);
  //Serial.print(" Accel Z Registers: "); Serial.println(az);
  
}

void get_temp(){
  //return temperature in degrees C

  int16_t temp;
  
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x41); // write to register to get response
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,2,true); // read register data
  temp = Wire.read()<<8|Wire.read(); //combine 2 data registers into one variable
  Wire.endTransmission();

  //following datasheet to convert to degrees c
  //The temperature in degrees C for a given register value may be computed as:
  //Temperature in degrees C = (TEMP_OUT Register Value as a signed quantity)/340 + 36.53
  //Please note that the math in the above equation is in decimal.

  //temp = ~temp + 1;
  temp_c = (temp/340) + 36.53;
  Serial.print("Temperature: "); Serial.println(temp_c);

}

void calibrate_gyro(){
  //gyro data integers
  int16_t gyrox_data;
  int16_t gyroy_data;
  int16_t gyroz_data;
  //add instances of gyro data together and sace to variables
  int gyrox_data_total = 0;
  int gyroy_data_total = 0;
  int gyroz_data_total = 0;
  
  //get instances of gyro data
  for(int i=0; i<cal; i++){
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x43); // write to register to get response
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr,6,true); // read register data
    gyrox_data = Wire.read()<<8|Wire.read(); //combine 2 data registers into one variable
    gyrox_data_total = gyrox_data_total + gyrox_data;
    gyroy_data = Wire.read()<<8|Wire.read(); //combine 2 data registers into one variable
    gyroy_data_total = gyroy_data_total + gyroy_data;
    gyroz_data = Wire.read()<<8|Wire.read(); //combine 2 data registers into one variable
    gyroz_data_total = gyroz_data_total + gyroz_data;
    Wire.endTransmission(false);
  }

  offsetx = gyrox_data_total / cal;
  Serial.print("X offset: "); Serial.println(offsetx);
  offsety = gyroy_data_total / cal;
  Serial.print("Y offset: "); Serial.println(offsety);
  offsetz = gyroz_data_total / cal;
  Serial.print("Z offset: "); Serial.println(offsetz);
}
