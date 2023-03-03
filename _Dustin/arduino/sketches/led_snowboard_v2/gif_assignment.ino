//#define NUM_LEDS 2090 //2090 sets the amount of pixels being controlled

void assign_nyan(){
  //image data is split between two arrays 
  delay(50); // fake the frame rate
  Serial.println("nyan");
/*
  for(int x=0; x<7; x++){
    for(int i=0; i<(NUM_LEDS/2); i++){
      half1.setPixelColor(i, half1.gamma32(nyanData[x][i]));
      half2.setPixelColor(i, half2.gamma32(nyanData[x][i+NUM_LEDS/2]));
      delay(50); // fake the frame rate
      half1.show();
      half2.show();
    }
  }
  */

}

void assign_mario(){
  //image data is split between two arrays
  Serial.println("mario");
  for(int x=0; x<1; x++){
    for(int i=0; i<(NUM_LEDS/2); i++){
      half1.setPixelColor(i, half1.gamma32(marioData[x][i]));
      half2.setPixelColor(i+1, half2.gamma32(marioData[x][i+NUM_LEDS/2]));
      //delay(50); // fake the frame rate
    }
    half1.show();
    half2.show();
  }

}

void assign_pacman(){
  //image data is split between two arrays 
  Serial.println("pacman");

  for(int x=0; x<25; x++){
    for(int i=0; i<(NUM_LEDS/2); i++){
      //pacmanData[x][i] = 0;
      half1.setPixelColor(i, half1.gamma32(pacmanData[x][i]));
      half2.setPixelColor(i+1, half2.gamma32(pacmanData[x][i+NUM_LEDS/2]));
    }
    delay(100); // fake the frame rate
    half1.show();
    half2.show();
  }

}
