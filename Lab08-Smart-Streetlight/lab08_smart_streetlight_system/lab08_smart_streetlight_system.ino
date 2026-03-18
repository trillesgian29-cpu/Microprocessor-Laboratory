// Lab 08: Smart Streetlight System
// Bicol University Polangui Campus - BSCpE
// Author: Gian Carlo D. Trilles



/*
  Lab 8: Smart Streetlight System
  Logic: If Light Level < Threshold, turn ON Streetlight.
*/

const int ldrPin = A0;    // LDR connected to A0
const int ledPin = 9;     // LED connected to Pin 9
const int threshold = 400; // Adjust this based on your room's light

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int lightLevel = analogRead(ldrPin); // Read light (0-1023)

  Serial.print("Current Light Level: ");
  Serial.println(lightLevel);

  if (lightLevel < threshold) {
    digitalWrite(ledPin, HIGH); // It's dark! Turn on light.
  } else {
    digitalWrite(ledPin, LOW);  // It's bright! Turn off light.
  }

  delay(200); // Stability delay
}