
#include "HX711.h"


HX711 scale;

int LOADCELL_DOUT_PIN[] = {2,4,6,8};
int LOADCELL_SCK_PIN[] = {3,5,7,9};

float calibration_factor = 435; //Starting point

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");

  Serial.println("Enter the Load Cell Nr. to test (1-4)");
  int temp = 10;
  while(temp == 10){
    if(Serial.available() > 0){
      temp = Serial.parseInt();
      scale.begin(LOADCELL_DOUT_PIN[temp-1], LOADCELL_SCK_PIN[temp-1]);
      Serial.print("Load Cell Nr. ");
      Serial.print(temp);
      Serial.println(" selected.");
    }
  }
  scale.set_scale();
  scale.tare();	//Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
}

void loop() {

  scale.set_scale(calibration_factor); //Adjust to this calibration factor

  Serial.print("Reading: ");
  Serial.print(scale.get_units(), 1);
  Serial.print(" g"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();

  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor += 0.5;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 0.5;
  }
  delay(200);
}