// Lab 09: Integrated Environmental Monitoring System
// Bicol University Polangui Campus - BSCpE
// Author: Gian Carlo D. Trilles



/*
 * ================================================================
 * INTEGRATED INDUSTRIAL SAFETY MONITOR
 * Final Lab Exam Project
 * ================================================================
 * This system monitors ambient temperature and provides multi-level
 * visual and auditory feedback based on user-defined thresholds.
 * 
 * Hardware:
 * - Arduino Uno
 * - LM35 Temperature Sensor (A0)
 * - Potentiometer for threshold control (A1)
 * - 16x2 LCD Display (16-pin parallel interface)
 * - Piezo Buzzer (Pin 8)
 * - 3x3 LED Matrix (Pins 2-7)
 * ================================================================
 */

#include <LiquidCrystal.h>

// ================ PIN DEFINITIONS ================
// Analog Inputs
const int LM35_PIN = A0;           // LM35 temperature sensor input
const int POT_PIN = A1;            // Potentiometer for threshold setting

// Buzzer Output
const int BUZZER_PIN = 8;          // Piezo buzzer output

// 3x3 LED Matrix Pins
// Row pins (cathodes) - connected through 220Ω resistors
const int ROW_PINS[] = {2, 3, 4};  
// Column pins (anodes)
const int COL_PINS[] = {5, 6, 7};  

// LCD Pins (16-pin parallel interface)
// LCD Pin connections to Arduino:
// RS -> Pin 9
// E  -> Pin 10
// D4 -> Pin 11
// D5 -> Pin 12
// D6 -> Pin 13
// D7 -> A2
// VSS -> GND
// VDD -> 5V
// V0  -> Potentiometer middle pin (for contrast adjustment)
// RW  -> GND (write mode only)
// A (Anode/Backlight+) -> 5V through 220Ω resistor
// K (Cathode/Backlight-) -> GND

const int LCD_RS = 9;
const int LCD_E = 10;
const int LCD_D4 = 11;
const int LCD_D5 = 12;
const int LCD_D6 = 13;
const int LCD_D7 = A2;

// ================ CONSTANTS ================
const float MIN_THRESHOLD = 20.0;  // Minimum threshold temperature (°C)
const float MAX_THRESHOLD = 40.0;  // Maximum threshold temperature (°C)
const int TEMP_PRECISION = 1;      // Decimal places for temperature display
const int THRESHOLD_PRECISION = 1; // Decimal places for threshold display

// Timing constants for non-blocking operations
const unsigned long TEMP_READ_INTERVAL = 500;    // Read temperature every 500ms
const unsigned long THRESHOLD_READ_INTERVAL = 100; // Read potentiometer every 100ms
const unsigned long WARNING_BLINK_INTERVAL = 500;  // Blink rate for warning state
const unsigned long ALARM_BLINK_INTERVAL = 200;    // Fast blink rate for alarm state
const unsigned long BUZZER_PULSE_INTERVAL = 100;   // Buzzer pulse timing

// ================ GLOBAL VARIABLES ================
// Initialize LCD with pin connections (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// Temperature and threshold variables
float currentTemp = 0.0;            // Current temperature reading in Celsius
float targetThreshold = 30.0;       // User-defined threshold temperature

// State tracking variables
enum SystemState {NORMAL, WARNING, ALARM}; // System states
SystemState currentState = NORMAL;         // Current system state

// Timing variables for non-blocking operations
unsigned long lastTempRead = 0;            // Last temperature read timestamp
unsigned long lastThresholdRead = 0;       // Last threshold read timestamp
unsigned long lastBlinkTime = 0;           // Last LED blink timestamp
unsigned long lastBuzzerTime = 0;          // Last buzzer pulse timestamp
bool blinkState = false;                   // Current LED blink state
bool buzzerState = false;                  // Current buzzer state

// ================ LED MATRIX PATTERNS ================
// Each pattern is a 3x3 array where 1 = LED ON, 0 = LED OFF

// Normal state: Checkmark pattern
const bool PATTERN_NORMAL[3][3] = {
  {0, 0, 1},
  {1, 0, 1},
  {0, 1, 0}
};

// Warning state: Slow-blinking middle row
const bool PATTERN_WARNING[3][3] = {
  {0, 0, 0},
  {1, 1, 1},
  {0, 0, 0}
};

// Alarm state: Fast-blinking X pattern
const bool PATTERN_ALARM[3][3] = {
  {1, 0, 1},
  {0, 1, 0},
  {1, 0, 1}
};

// ================================================================
// SETUP FUNCTION - Runs once at startup
// ================================================================
void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println("Industrial Safety Monitor Starting...");
  
  // ================ INITIALIZE LCD ================
  lcd.begin(16, 2);     // Initialize 16x2 LCD
  lcd.clear();          // Clear any residual display
  
  // Display startup message
  lcd.setCursor(0, 0);
  lcd.print("Safety Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);          // Show message for 2 seconds
  lcd.clear();
  
  // ================ CONFIGURE LED MATRIX PINS ================
  // Set all row pins (cathodes) as outputs and initialize to HIGH (off)
  for (int i = 0; i < 3; i++) {
    pinMode(ROW_PINS[i], OUTPUT);
    digitalWrite(ROW_PINS[i], HIGH); // Common cathode, HIGH = off
  }
  
  // Set all column pins (anodes) as outputs and initialize to LOW (off)
  for (int i = 0; i < 3; i++) {
    pinMode(COL_PINS[i], OUTPUT);
    digitalWrite(COL_PINS[i], LOW);  // LOW = off
  }
  
  // ================ CONFIGURE BUZZER PIN ================
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);     // Ensure buzzer is off initially
  
  // ================ CONFIGURE ANALOG INPUTS ================
  // Analog pins are inputs by default, but we set them explicitly for clarity
  pinMode(LM35_PIN, INPUT);
  pinMode(POT_PIN, INPUT);
  
  Serial.println("Initialization complete!");
}

// ================================================================
// MAIN LOOP - Runs continuously
// ================================================================
void loop() {
  // Read sensors at their respective intervals (non-blocking)
  readTemperature();
  readThreshold();
  
  // Determine system state based on temperature and threshold
  determineState();
  
  // Update LCD display with current values
  updateLCD();
  
  // Update LED matrix pattern based on current state
  updateLEDMatrix();
  
  // Handle buzzer for alarm state
  handleBuzzer();
}

// ================================================================
// TEMPERATURE READING FUNCTION
// Reads LM35 sensor and converts to Celsius
// Non-blocking: only reads at specified intervals
// ================================================================
void readTemperature() {
  unsigned long currentMillis = millis();
  
  // Check if it's time to read temperature
  if (currentMillis - lastTempRead >= TEMP_READ_INTERVAL) {
    lastTempRead = currentMillis;
    
    // Read analog value from LM35 sensor
    int rawValue = analogRead(LM35_PIN);
    
    // Convert ADC value to temperature
    // LM35 outputs 10mV per degree Celsius
    // Arduino ADC: 5V / 1024 steps = 4.88mV per step
    // Temperature (°C) = (rawValue * 5.0 * 100.0) / 1024.0
    currentTemp = (rawValue * 5.0 * 100.0) / 1024.0;
    
    // Debug output
    Serial.print("Temperature: ");
    Serial.print(currentTemp);
    Serial.println(" °C");
  }
}

// ================================================================
// THRESHOLD READING FUNCTION
// Reads potentiometer and maps to threshold range (20-40°C)
// Non-blocking: only reads at specified intervals
// ================================================================
void readThreshold() {
  unsigned long currentMillis = millis();
  
  // Check if it's time to read threshold
  if (currentMillis - lastThresholdRead >= THRESHOLD_READ_INTERVAL) {
    lastThresholdRead = currentMillis;
    
    // Read analog value from potentiometer
    int potValue = analogRead(POT_PIN);
    
    // Map potentiometer reading (0-1023) to threshold range (20-40°C)
    targetThreshold = map(potValue, 0, 1023, 
                          MIN_THRESHOLD * 10, MAX_THRESHOLD * 10) / 10.0;
    
    // Debug output
    Serial.print("Threshold: ");
    Serial.print(targetThreshold);
    Serial.println(" °C");
  }
}

// ================================================================
// STATE DETERMINATION FUNCTION
// Determines system state based on temperature vs threshold
// Normal: Temp < Threshold
// Warning: Temp within 2°C of Threshold
// Alarm: Temp >= Threshold
// ================================================================
void determineState() {
  SystemState previousState = currentState;
  
  if (currentTemp >= targetThreshold) {
    // Temperature at or above threshold - ALARM STATE
    currentState = ALARM;
  } 
  else if (currentTemp >= (targetThreshold - 2.0)) {
    // Temperature within 2°C of threshold - WARNING STATE
    currentState = WARNING;
  } 
  else {
    // Temperature below threshold - NORMAL STATE
    currentState = NORMAL;
  }
  
  // If state changed, log it and reset blink timing
  if (currentState != previousState) {
    Serial.print("State changed to: ");
    switch(currentState) {
      case NORMAL:
        Serial.println("NORMAL");
        break;
      case WARNING:
        Serial.println("WARNING");
        break;
      case ALARM:
        Serial.println("ALARM");
        break;
    }
    lastBlinkTime = millis(); // Reset blink timing on state change
  }
}

// ================================================================
// LCD UPDATE FUNCTION
// Updates the 16x2 LCD display with current information
// Row 1: Target Threshold value
// Row 2: Current Temperature and Warning status (if applicable)
// ================================================================
void updateLCD() {
  // ================ ROW 1: DISPLAY THRESHOLD ================
  lcd.setCursor(0, 0);
  lcd.print("Target: ");
  lcd.print(targetThreshold, THRESHOLD_PRECISION);
  lcd.print((char)223); // Degree symbol
  lcd.print("C ");      // Ensure we clear any extra characters
  
  // ================ ROW 2: DISPLAY TEMPERATURE & STATUS ================
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(currentTemp, TEMP_PRECISION);
  lcd.print((char)223); // Degree symbol
  lcd.print("C");
  
  // Clear remaining characters and show warning if in ALARM state
  if (currentState == ALARM) {
    lcd.print("     "); // Clear space
    lcd.setCursor(0, 1);
    lcd.print("*** WARNING! ***");
  } else {
    lcd.print("   "); // Clear any residual characters
  }
}

// ================================================================
// LED MATRIX UPDATE FUNCTION
// Updates the 3x3 LED matrix based on current system state
// Uses multiplexing to display patterns
// ================================================================
void updateLEDMatrix() {
  unsigned long currentMillis = millis();
  unsigned long blinkInterval;
  
  // Determine blink interval based on state
  switch(currentState) {
    case NORMAL:
      // Static display, no blinking
      displayLEDPattern(PATTERN_NORMAL);
      return;
      
    case WARNING:
      // Slow blinking (500ms interval)
      blinkInterval = WARNING_BLINK_INTERVAL;
      break;
      
    case ALARM:
      // Fast blinking (200ms interval)
      blinkInterval = ALARM_BLINK_INTERVAL;
      break;
  }
  
  // Handle blinking for WARNING and ALARM states
  if (currentMillis - lastBlinkTime >= blinkInterval) {
    lastBlinkTime = currentMillis;
    blinkState = !blinkState; // Toggle blink state
  }
  
  // Display pattern based on state and blink state
  if (blinkState) {
    if (currentState == WARNING) {
      displayLEDPattern(PATTERN_WARNING);
    } else {
      displayLEDPattern(PATTERN_ALARM);
    }
  } else {
    // Turn off all LEDs during blink-off phase
    clearLEDMatrix();
  }
}

// ================================================================
// LED PATTERN DISPLAY FUNCTION
// Displays a specific pattern on the 3x3 LED matrix
// Uses multiplexing to light individual LEDs
// ================================================================
void displayLEDPattern(const bool pattern[3][3]) {
  // Multiplex through each row
  for (int row = 0; row < 3; row++) {
    // Activate current row (LOW for common cathode)
    digitalWrite(ROW_PINS[row], LOW);
    
    // Set column states for this row
    for (int col = 0; col < 3; col++) {
      if (pattern[row][col]) {
        digitalWrite(COL_PINS[col], HIGH); // Turn LED on
      } else {
        digitalWrite(COL_PINS[col], LOW);  // Turn LED off
      }
    }
    
    // Small delay for persistence of vision
    delayMicroseconds(500);
    
    // Deactivate current row
    digitalWrite(ROW_PINS[row], HIGH);
    
    // Turn off all columns to prevent ghosting
    for (int col = 0; col < 3; col++) {
      digitalWrite(COL_PINS[col], LOW);
    }
  }
}

// ================================================================
// LED MATRIX CLEAR FUNCTION
// Turns off all LEDs in the matrix
// ================================================================
void clearLEDMatrix() {
  // Set all rows to HIGH (off for common cathode)
  for (int i = 0; i < 3; i++) {
    digitalWrite(ROW_PINS[i], HIGH);
  }
  
  // Set all columns to LOW (off)
  for (int i = 0; i < 3; i++) {
    digitalWrite(COL_PINS[i], LOW);
  }
}

// ================================================================
// BUZZER HANDLER FUNCTION
// Handles buzzer operation in ALARM state
// Produces a pulsed tone (non-blocking)
// ================================================================
void handleBuzzer() {
  if (currentState == ALARM) {
    unsigned long currentMillis = millis();
    
    // Toggle buzzer state at specified interval
    if (currentMillis - lastBuzzerTime >= BUZZER_PULSE_INTERVAL) {
      lastBuzzerTime = currentMillis;
      buzzerState = !buzzerState;
      
      if (buzzerState) {
        // Turn buzzer on with a specific frequency
        tone(BUZZER_PIN, 2000); // 2000 Hz tone
      } else {
        // Turn buzzer off
        noTone(BUZZER_PIN);
      }
    }
  } else {
    // Ensure buzzer is off in NORMAL and WARNING states
    noTone(BUZZER_PIN);
    buzzerState = false;
  }
}