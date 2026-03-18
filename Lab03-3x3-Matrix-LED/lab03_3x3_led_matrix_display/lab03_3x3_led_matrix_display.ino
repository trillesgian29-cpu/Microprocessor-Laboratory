// Lab 03: 3x3 LED Matrix Display
// Bicol University Polangui Campus - BSCpE
// Author: Gian Carlo D. Trilles


// Pin definitions for rows and columns
int rowPins[] = {2, 3, 4};   // Rows connected to pins 2, 3, 4
int colPins[] = {5, 6, 7};   // Columns connected to pins 5, 6, 7
// Define the pattern to display on the matrix (1 = LED on, 0 = LED off)
byte pattern[3][3] = {
  {1, 0, 1},  // First row
  {0, 1, 0},  // Second row
  {1, 0, 1}   // Third row
};

void setup() {
  // Set all row pins as output
  for (int i = 0; i < 3; i++) {
    pinMode(rowPins[i], OUTPUT);
  }
  
  // Set all column pins as output
  for (int i = 0; i < 3; i++) {
    pinMode(colPins[i], OUTPUT);
  }
}
void loop() {
  // Loop through each row and column to display the pattern
  for (int row = 0; row < 3; row++) {
    // Turn on the current row
    digitalWrite(rowPins[row], HIGH);
    
    for (int col = 0; col < 3; col++) {
      // Turn on the LED in the current row and column if the pattern is 1
      if (pattern[row][col] == 1) {
        digitalWrite(colPins[col], HIGH);
      } else {
        digitalWrite(colPins[col], LOW);
      }
    }
    delay(500); // Delay to make the LED visible
    // Turn off the current row
    digitalWrite(rowPins[row], LOW);
  }
}
