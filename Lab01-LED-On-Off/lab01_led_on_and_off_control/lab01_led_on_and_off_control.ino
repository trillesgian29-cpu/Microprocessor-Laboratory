// Lab 01: LED On and Off Control
// Bicol University Polangui Campus - BSCpE
// Author: Gian Carlo D. Trilles

void setup() {
  pinMode(13, OUTPUT);

}

void loop() {
  digitalWrite(13, HIGH); // set pin 13 as an Output
  delay(1000); //Wait 1 second
  digitalWrite(13, LOW); //turn led off
  delay(1000); //wait 1 second
}
