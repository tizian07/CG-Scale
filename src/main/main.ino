#include "HX711.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LOADCELL_1_DOUT_PIN 2
#define LOADCELL_1_SCK_PIN 3
#define LOADCELL_2_DOUT_PIN 4
#define LOADCELL_2_SCK_PIN 5
#define LOADCELL_3_DOUT_PIN 6
#define LOADCELL_3_SCK_PIN 7
#define LOADCELL_4_DOUT_PIN 8
#define LOADCELL_4_SCK_PIN 9

HX711 loadCell1;
HX711 loadCell2;
HX711 loadCell3;
HX711 loadCell4;

float xCoordinates[] = {100.0, 0.0, 100.0, -0.0}; // Modify these values based on your setup in mm
float yCoordinates[] = {100.0, -100.0, -100.0, 100.0}; // Modify these values based on your setup in mm

float calibration_factor_1 = 435; // Modify these values based on your calibration
float calibration_factor_2 = 440; // Modify these values based on your calibration
float calibration_factor_3 = -436; // Modify these values based on your calibration
float calibration_factor_4 = -432; // Modify these values based on your calibration

float centerX;
float centerY;

  float totalWeight_old = 0;
  float centerX_old = 0;
  float centerY_old = 0;

// Define the I2C address of your LCD (you may need to adjust this)
#define LCD_I2C_ADDR 0x27

// Define the number of columns and rows of your LCD (20x4)
#define LCD_COLUMNS 20
#define LCD_ROWS 4

// Create an LCD object
LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_COLUMNS, LCD_ROWS);

void initializeLoadCells() {
  loadCell1.begin(LOADCELL_1_DOUT_PIN, LOADCELL_1_SCK_PIN);
  loadCell2.begin(LOADCELL_2_DOUT_PIN, LOADCELL_2_SCK_PIN);
  loadCell3.begin(LOADCELL_3_DOUT_PIN, LOADCELL_3_SCK_PIN);
  loadCell4.begin(LOADCELL_4_DOUT_PIN, LOADCELL_4_SCK_PIN);
}

void tareLoadCells() {
  loadCell1.set_scale();
  loadCell1.tare();
  loadCell2.set_scale();
  loadCell2.tare();
  loadCell3.set_scale();
  loadCell3.tare();
  loadCell4.set_scale();
  loadCell4.tare();
}

// Initial LCD display
void initializeLCD() {
  lcd.clear();
  lcd.print("Weight [g]: ");
  lcd.print("0");
  lcd.setCursor(0, 1);
  lcd.print("Center of Gravity:");
  lcd.setCursor(0, 2);
  lcd.print("X [mm]: ");
  lcd.print("0");
  lcd.setCursor(0, 3);
  lcd.print("Y [mm]: ");
  lcd.print("0");
  delay(10);
}

float readLoadCell(HX711& loadCell, float calibrationFactor) {
  loadCell.set_scale(calibrationFactor);
  float weight = loadCell.get_units();
  if (isnan(weight)) {
    lcd.clear();
    lcd.print("Error: Invalid weight");
    lcd.setCursor(0, 1);
    lcd.print("reading. Check cell.");
    delay(2000);  // Display error for 2 seconds
  }
  return weight;
}

void setup() {
  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  delay(100);
  

  initializeLoadCells();
  lcd.clear();
  lcd.print("Load cells ready");
  lcd.setCursor(0, 1);
  lcd.print("Taring...");
  tareLoadCells();

  initializeLCD();
}

void loop() {
  float weight1 = readLoadCell(loadCell1, calibration_factor_1);
  float weight2 = readLoadCell(loadCell2, calibration_factor_2);
  float weight3 = readLoadCell(loadCell3, calibration_factor_3);
  float weight4 = readLoadCell(loadCell4, calibration_factor_4);

  if (isnan(weight1) || isnan(weight2) || isnan(weight3) || isnan(weight4)) {
    lcd.clear();
    lcd.print("Error: Invalid weight");
    lcd.setCursor(0, 1);
    lcd.print("reading. Check cells.");
    delay(2000);  // Display error for 2 seconds
    return;
  }

  float totalWeight = weight1 + weight2 + weight3 + weight4;

  if (totalWeight < 10) {
    centerX = 0;
    centerY = 0;
  } else {
    centerX = ((weight1 * xCoordinates[0]) + (weight2 * xCoordinates[1]) + (weight3 * xCoordinates[2]) + (weight4 * xCoordinates[3])) / totalWeight;
    centerY = ((weight1 * yCoordinates[0]) + (weight2 * yCoordinates[1]) + (weight3 * yCoordinates[2]) + (weight4 * yCoordinates[3])) / totalWeight;
  }


  // Print output onto screen only if the value changes
  if(abs(totalWeight_old - totalWeight) >= 0.5){
    lcd.setCursor(12, 0);
    lcd.print("        ");
    lcd.setCursor(12, 0);
    lcd.print(totalWeight, 0);
  }
  
  if(abs(centerX_old - centerX) >= 0.5){
    lcd.setCursor(8, 2);
    lcd.print("            ");
    lcd.setCursor(8, 2);
    lcd.print(centerX, 0);
  }

  if(abs(centerY_old - centerY) >= 0.5){
    lcd.setCursor(8, 3);
    lcd.print("            ");
    lcd.setCursor(8, 3);
    lcd.print(centerY, 0);
  }


  totalWeight_old = totalWeight;
  centerX_old = centerX;
  centerY_old = centerY;

  delay(100);
}
