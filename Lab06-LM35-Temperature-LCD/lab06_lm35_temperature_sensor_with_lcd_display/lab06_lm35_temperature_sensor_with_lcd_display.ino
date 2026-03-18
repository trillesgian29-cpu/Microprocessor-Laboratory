// Lab 06: LM35 Temperature Sensor with LCD Display
// Bicol University Polangui Campus - BSCpE
// Author: Gian Carlo D. Trilles




#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Initialize the LCD(Addres 0x27 is common)
LiquidCrystal_I2C lcd(0x27, 16, 2);
const int sensorPin = A0;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Temp Monitor");
  delay(2000);
  lcd.clear();
}

void loop() {
  //Read analog value (0-1023)
  int reading = analogRead(sensorPin);

  //Convert reading to voltage (for 5V Arduino)
  float voltage = reading *(5.0 / 1024.0);

  //LM35 output is 10mv per degrees Celsius
  float tempC = voltage * 100.0;

  //UI Output
  lcd.setCursor(0,0);
  lcd.print("Current Temp: ");
  lcd.setCursor(0,1);
  lcd.print(tempC);
  lcd.print(" C");
  
  delay(1000);//Update every Second
}
