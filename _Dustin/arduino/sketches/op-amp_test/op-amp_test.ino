#define cell1 36
#define cell2 39
#define cell3 34

float cell_1;
float cell_2;
float cell_3;

float gain = 100/49.9; //gain for the op-amp
float gainCorrect1 = 1.0824;
float gainCorrect2 = 1.0755;
float gainCorrect3 = 1.0789;

void setup() {
  Serial.begin(115200);

  pinMode(cell1, INPUT);
  pinMode(cell2, INPUT);
  pinMode(cell3, INPUT);

}

void loop() {
  //read cell values
  cell_1 = analogRead(cell1);
  cell_2 = analogRead(cell2);
  cell_3 = analogRead(cell3);

  //turn analog values into voltages
  cell_1 = ((cell_1*(3.3/4095))*gain)*gainCorrect1;
  cell_2 = ((cell_2*(3.3/4095))*gain)*gainCorrect2;
  cell_3 = ((cell_3*(3.3/4095))*gain)*gainCorrect3;

  //output data to terminal
  Serial.print("\nCell 1 Voltage: ");
  Serial.print(cell_1);
  Serial.print("   Cell 2 Voltage: ");
  Serial.print(cell_2);
  Serial.print("   Cell 3 Voltage: ");
  Serial.println(cell_3);

  delay(10);

}
