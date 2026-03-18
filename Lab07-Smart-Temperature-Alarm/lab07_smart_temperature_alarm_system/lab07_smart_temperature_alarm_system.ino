// Lab 07: Smart Temperature Alarm System
// Bicol University Polangui Campus - BSCpE
// Author: Gian Carlo D. Trilles



#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int sensorPin = A0;
const int buzzerPin = 8;
const float threshold = 30.0; //Trigger temp in Celsius

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  int reading = analogRead(sensorPin);
  float voltage = reading * (5.0/1024.0);
  float tempC = voltage * 100.0;

  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.print(tempC);
  lcd.print("C "); //Spaces clear old digits

  if (tempC >= threshold){
    //Visual alert
    lcd.setCursor(0,1);
    lcd.print("***WARNING!***");

    //Auditort alert(Pulsed Tone)
    tone(buzzerPin, 1000);
    delay(200);
    noTone(buzzerPin);
    delay(200);
  }else{
    lcd.setCursor(0,1);
    lcd.print("Status: Normal ");
    noTone(buzzerPin);
  }

  delay(500);

}
