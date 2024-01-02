//#define NUM_LEDS 2090 //2090 sets the amount of pixels being controlled

void assign_nyan(){
  for(int i=0; i<6; i++){
    Serial.println(i);
    readFrame("nyan", i);
    half1.show();
    half2.show();
  }
}

void assign_mario(){
  //image data is split between two arrays
  //assign mario from sd card
  readFrame("mario", 0);
  half1.show();
  half2.show();

}


void assign_pacman(){
  for(int i=0; i<54; i++){
    Serial.println(i);
    readFrame("pacman", i);
    half1.show();
    half2.show();
  }

}

void assign_neon(){
  // assign neon board from sd card
  for(int i=0; i<25; i++){
    Serial.println(i);
    readFrame("The_Heart_of_Neon_board_background", i);
    half1.show();
    half2.show();
  }
}
