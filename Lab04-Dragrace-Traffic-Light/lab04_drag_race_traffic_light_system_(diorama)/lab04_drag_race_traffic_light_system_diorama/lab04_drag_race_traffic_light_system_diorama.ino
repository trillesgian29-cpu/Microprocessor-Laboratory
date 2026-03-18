// Lab 04: Drag Race Traffic Light System (Diorama)
// Bicol University Polangui Campus - BSCpE
// Author: Gian Carlo D. Trilles




// LEFT LANE - Christmas Tree Lights
#define LEFT_RED 13
#define LEFT_YELLOW_1 12
#define LEFT_YELLOW_2 11
#define LEFT_YELLOW_3 10
#define LEFT_GREEN 9

// RIGHT LANE - Christmas Tree Lights
#define RIGHT_RED 8
#define RIGHT_YELLOW_1 7
#define RIGHT_YELLOW_2 6
#define RIGHT_YELLOW_3 5
#define RIGHT_GREEN 4

// Ultrasonic Sensor (HC-SR04)
#define TRIG_PIN A0
#define ECHO_PIN A1

// Controls
#define READY_BUTTON A2
#define BUZZER_PIN A3

// Sensor settings
#define MOVEMENT_THRESHOLD 15  // Distance change in cm to detect movement
#define NUM_READINGS 3         // Number of readings to average

// Race States
enum RaceState {
  WAITING_READY,
  COUNTDOWN_DELAY,
  YELLOW_1_PHASE,
  YELLOW_2_PHASE,
  YELLOW_3_PHASE,
  GREEN_PHASE,
  FALSE_START,
  RACE_COMPLETE
};

RaceState currentState = WAITING_READY;
unsigned long stateStartTime = 0;
int baselineDistance = 0;

void setup() {
  Serial.begin(9600);

  // Configure LED pins
  pinMode(LEFT_RED, OUTPUT);
  pinMode(LEFT_YELLOW_1, OUTPUT);
  pinMode(LEFT_YELLOW_2, OUTPUT);
  pinMode(LEFT_YELLOW_3, OUTPUT);
  pinMode(LEFT_GREEN, OUTPUT);

  pinMode(RIGHT_RED, OUTPUT);
  pinMode(RIGHT_YELLOW_1, OUTPUT);
  pinMode(RIGHT_YELLOW_2, OUTPUT);
  pinMode(RIGHT_YELLOW_3, OUTPUT);
  pinMode(RIGHT_GREEN, OUTPUT);

  // Configure ultrasonic sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Configure controls
  pinMode(READY_BUTTON, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize
  turnOffAllLights();
  digitalWrite(BUZZER_PIN, LOW);

  // Show RED light
  digitalWrite(LEFT_RED, HIGH);
  digitalWrite(RIGHT_RED, HIGH);

  Serial.println(F("\n=== DRAG RACE READY ==="));
  Serial.println(F("Single Sensor - Improved Stability"));
  Serial.println(F("RED LIGHT ON - Press button\n"));
}

void loop() {
  switch (currentState) {
    case WAITING_READY:
      handleWaitingReady();
      break;
    case COUNTDOWN_DELAY:
      handleCountdownDelay();
      break;
    case YELLOW_1_PHASE:
      handleYellow1();
      break;
    case YELLOW_2_PHASE:
      handleYellow2();
      break;
    case YELLOW_3_PHASE:
      handleYellow3();
      break;
    case GREEN_PHASE:
      handleGreen();
      break;
    case FALSE_START:
      handleFalseStart();
      break;
    case RACE_COMPLETE:
      handleComplete();
      break;
  }
}

void handleWaitingReady() {
  if (digitalRead(READY_BUTTON) == LOW) {
    delay(50);
    if (digitalRead(READY_BUTTON) == LOW) {
      Serial.println(F("\n=== BUTTON PRESSED ==="));

      // Take multiple readings and average them
      Serial.println(F("Calibrating sensor..."));
      delay(200);  // Let sensor stabilize

      baselineDistance = getStableDistance();

      Serial.print(F("Baseline: "));
      Serial.print(baselineDistance);
      Serial.println(F(" cm"));

      if (baselineDistance < 5 || baselineDistance > 300) {
        Serial.println(F("WARNING: Sensor reading unusual!"));
        Serial.println(F("Check sensor position"));
      }

      tone(BUZZER_PIN, 800, 500);
      changeState(COUNTDOWN_DELAY);
    }
  }
}

void handleCountdownDelay() {
  if (millis() - stateStartTime >= 1000) {
    changeState(YELLOW_1_PHASE);
  }
}

void handleYellow1() {
  if (checkMotionDuringStaging()) return;

  if (millis() - stateStartTime >= 1000) {
    changeState(YELLOW_2_PHASE);
  }
}

void handleYellow2() {
  if (checkMotionDuringStaging()) return;

  if (millis() - stateStartTime >= 1000) {
    changeState(YELLOW_3_PHASE);
  }
}

void handleYellow3() {
  if (checkMotionDuringStaging()) return;

  if (millis() - stateStartTime >= 1000) {
    changeState(GREEN_PHASE);
  }
}

void handleGreen() {
  static bool checkDone = false;

  if (!checkDone && millis() - stateStartTime >= 500) {
    int currentDistance = getStableDistance();
    int distanceChange = abs(currentDistance - baselineDistance);

    Serial.print(F("Distance check: "));
    Serial.print(baselineDistance);
    Serial.print(F(" -> "));
    Serial.print(currentDistance);
    Serial.print(F(" (change: "));
    Serial.print(distanceChange);
    Serial.println(F(" cm)"));

    if (distanceChange > MOVEMENT_THRESHOLD) {
      Serial.println(F("\n=== VALID START ==="));
      Serial.println(F("Movement detected!"));
      checkDone = true;
    } else {
      Serial.println(F("\n=== INVALID START ==="));
      Serial.println(F("No movement detected!"));
      changeState(FALSE_START);
      return;
    }
  }

  if (millis() - stateStartTime >= 15000) {
    checkDone = false;
    changeState(RACE_COMPLETE);
  }
}

void handleFalseStart() {
  Serial.println(F("\n*** FALSE START ***"));

  for (int i = 0; i < 8; i++) {
    setAllLights(HIGH);
    tone(BUZZER_PIN, 1000);
    delay(150);

    setAllLights(LOW);
    noTone(BUZZER_PIN);
    delay(150);
  }

  delay(1000);
  Serial.println(F("\nRestart - Press button\n"));

  digitalWrite(LEFT_RED, HIGH);
  digitalWrite(RIGHT_RED, HIGH);
  changeState(WAITING_READY);
}

void handleComplete() {
  Serial.println(F("\n=== RACE COMPLETE ===\n"));

  for (int i = 0; i < 5; i++) {
    digitalWrite(LEFT_GREEN, HIGH);
    digitalWrite(RIGHT_GREEN, HIGH);
    tone(BUZZER_PIN, 2000, 200);
    delay(300);

    digitalWrite(LEFT_GREEN, LOW);
    digitalWrite(RIGHT_GREEN, LOW);
    delay(300);
  }

  delay(2000);
  Serial.println(F("Press button for next race\n"));

  digitalWrite(LEFT_RED, HIGH);
  digitalWrite(RIGHT_RED, HIGH);
  changeState(WAITING_READY);
}

void changeState(RaceState newState) {
  currentState = newState;
  stateStartTime = millis();

  switch (newState) {
    case COUNTDOWN_DELAY:
      Serial.println(F("Starting in 1 sec..."));
      break;

    case YELLOW_1_PHASE:
      digitalWrite(LEFT_RED, LOW);
      digitalWrite(RIGHT_RED, LOW);
      digitalWrite(LEFT_YELLOW_1, HIGH);
      digitalWrite(RIGHT_YELLOW_1, HIGH);
      tone(BUZZER_PIN, 1500, 100);
      Serial.println(F("YELLOW 1"));
      break;

    case YELLOW_2_PHASE:
      digitalWrite(LEFT_YELLOW_2, HIGH);
      digitalWrite(RIGHT_YELLOW_2, HIGH);
      tone(BUZZER_PIN, 1500, 100);
      Serial.println(F("YELLOW 2"));
      break;

    case YELLOW_3_PHASE:
      digitalWrite(LEFT_YELLOW_3, HIGH);
      digitalWrite(RIGHT_YELLOW_3, HIGH);
      tone(BUZZER_PIN, 1500, 100);
      Serial.println(F("YELLOW 3"));
      break;

    case GREEN_PHASE:
      digitalWrite(LEFT_YELLOW_1, LOW);
      digitalWrite(LEFT_YELLOW_2, LOW);
      digitalWrite(LEFT_YELLOW_3, LOW);
      digitalWrite(RIGHT_YELLOW_1, LOW);
      digitalWrite(RIGHT_YELLOW_2, LOW);
      digitalWrite(RIGHT_YELLOW_3, LOW);

      digitalWrite(LEFT_GREEN, HIGH);
      digitalWrite(RIGHT_GREEN, HIGH);

      tone(BUZZER_PIN, 2000, 2000);

      Serial.println(F("\nGREEN - GO!"));
      break;
  }
}

bool checkMotionDuringStaging() {
  // Don't check on every loop - only every 200ms to reduce sensor noise
  static unsigned long lastCheck = 0;

  if (millis() - lastCheck < 200) {
    return false;  // Skip this check
  }

  lastCheck = millis();

  int currentDistance = getStableDistance();
  int distanceChange = abs(currentDistance - baselineDistance);

  // Show readings in serial for debugging
  Serial.print(F("Check: "));
  Serial.print(currentDistance);
  Serial.print(F(" cm (diff: "));
  Serial.print(distanceChange);
  Serial.println(F(")"));

  if (distanceChange > MOVEMENT_THRESHOLD) {
    Serial.println(F("\n*** EARLY MOVEMENT ***"));
    changeState(FALSE_START);
    return true;
  }

  return false;
}

int getStableDistance() {
  // Take multiple readings and average them for stability
  long total = 0;
  int validReadings = 0;

  for (int i = 0; i < NUM_READINGS; i++) {
    int reading = getSingleDistance();

    // Filter out bad readings (0 or > 300cm)
    if (reading > 0 && reading < 300) {
      total += reading;
      validReadings++;
    }

    delay(30);  // Small delay between readings
  }

  if (validReadings == 0) {
    return 400;  // No valid readings
  }

  return total / validReadings;  // Return average
}

int getSingleDistance() {
  // Send pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read echo with timeout
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  // Calculate distance
  if (duration == 0) return 0;  // No echo

  int distance = duration * 0.034 / 2;
  return distance;
}

void turnOffAllLights() {
  digitalWrite(LEFT_RED, LOW);
  digitalWrite(LEFT_YELLOW_1, LOW);
  digitalWrite(LEFT_YELLOW_2, LOW);
  digitalWrite(LEFT_YELLOW_3, LOW);
  digitalWrite(LEFT_GREEN, LOW);

  digitalWrite(RIGHT_RED, LOW);
  digitalWrite(RIGHT_YELLOW_1, LOW);
  digitalWrite(RIGHT_YELLOW_2, LOW);
  digitalWrite(RIGHT_YELLOW_3, LOW);
  digitalWrite(RIGHT_GREEN, LOW);
}

void setAllLights(int state) {
  digitalWrite(LEFT_RED, state);
  digitalWrite(LEFT_YELLOW_1, state);
  digitalWrite(LEFT_YELLOW_2, state);
  digitalWrite(LEFT_YELLOW_3, state);
  digitalWrite(LEFT_GREEN, state);

  digitalWrite(RIGHT_RED, state);
  digitalWrite(RIGHT_YELLOW_1, state);
  digitalWrite(RIGHT_YELLOW_2, state);
  digitalWrite(RIGHT_YELLOW_3, state);
  digitalWrite(RIGHT_GREEN, state);
}
