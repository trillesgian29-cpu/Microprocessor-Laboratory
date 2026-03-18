// Lab 05: Analog Input Challenge - Potentiometer & LCD
// Bicol University Polangui Campus - BSCpE
// Author: Gian Carlo D. Trilles


/*
  Analog Input Lab
  Reads an analog input on pin 0, prints the result to the Serial Monitor.
*/
int potPin = A0;     // The middle pin of the potentiometer
int rawValue = 0;    // Variable to store the raw ADC value (0-1023)
float voltage = 0;   // Variable to store the calculated voltage

void setup() {
  Serial.begin(9600); // Open the serial port at 9600 bps
  Serial.println("Potentiometer Lab Started...");
}
void loop() {

  // Read the value from the sensor
  rawValue = analogRead(potPin);

  // Convert the raw value (0-1023) to voltage (0-5V)
  voltage = rawValue * (5.0 / 1023.0);

  // Print results to Serial Monitor
  Serial.print("Raw Value: ");
  Serial.print(rawValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage);
  Serial.println("V");

  delay(200); // Wait 200ms so the screen doesn't flicker too fast
}
