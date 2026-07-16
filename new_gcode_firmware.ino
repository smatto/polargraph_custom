#include <Servo.h>

// --- PIN ASSIGNMENTS ---
const int X_STEP_PIN = 54;    // Left Motor
const int X_DIR_PIN = 55;
const int X_ENABLE_PIN = 38;
const int Y_STEP_PIN = 60;    // Right Motor
const int Y_DIR_PIN = 61;
const int Y_ENABLE_PIN = 56;
const int SERVO_PIN = 11;     // Pen Lift Servo

// --- PEN CONFIGURATION ---
Servo penServo;
const int PEN_DOWN = 0;       // 0 degrees to draw
const int PEN_UP = 60;        // 60 degrees to lift pen

// --- GEOMETRY & HARDWARE CONFIGURATION ---
const float MACHINE_WIDTH = 620.0;
const int STEPS_PER_ROTATION = 3200;
const float MM_PER_ROTATION = 40.0;
const float STEPS_PER_MM = (float)STEPS_PER_ROTATION / MM_PER_ROTATION;
const float START_X = 310.0;
const float START_Y = 300.0;

// Track the current positional state
float currentX = START_X;
float currentY = START_Y;
float feedRate = 1000.0;      // Default parsing movement speed mm/min

// Serial parsing string buffer
String inputBuffer = "";

void setup() {
  // Initialize Serial Communication for UGS (Standard Baud)
  Serial.begin(115200);
  
  // Configure Stepper Control Pins
  pinMode(X_STEP_PIN, OUTPUT);
  pinMode(X_DIR_PIN, OUTPUT);
  pinMode(X_ENABLE_PIN, OUTPUT);
  pinMode(Y_STEP_PIN, OUTPUT);
  pinMode(Y_DIR_PIN, OUTPUT);
  pinMode(Y_ENABLE_PIN, OUTPUT);
  
  // Enable the stepper drivers (RAMPS drivers activate LOW)
  digitalWrite(X_ENABLE_PIN, LOW);
  digitalWrite(Y_ENABLE_PIN, LOW);
  
  // Initialize Pen Position
  penServo.attach(SERVO_PIN);
  penServo.write(PEN_UP); 
  
  // Signal to Universal Gcode Sender that the board is online
  Serial.println("ok");
}

void loop() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        processGcode(inputBuffer);
        inputBuffer = "";
        Serial.println("ok"); // Send confirmation back to UGS to release the next block
      }
    } else {
      inputBuffer += c;
    }
  }
}

// Basic G-code Command Parser
void processGcode(String line) {
  line.toUpperCase();
  line.trim();
  
  // Handle Pen Controls via Spindle commands (M3 / M5) or Laser commands (M4)
  if (line.startsWith("M3") || line.startsWith("M03") || line.startsWith("M4")) {
    penServo.write(PEN_DOWN);
    delay(200); // Allow time for mechanical physical travel
    return;
  }
  if (line.startsWith("M5") || line.startsWith("M05")) {
    penServo.write(PEN_UP);
    delay(200); 
    return;
  }

  // Handle Linear Target Motions (G0 = Rapid, G1 = Linear Move)
  if (line.startsWith("G0") || line.startsWith("G1")) {
    float targetX = currentX;
    float targetY = currentY;
    
    // Parse individual coordinates out of the payload string
    if (line.indexOf('X') != -1) targetX = parseValue(line, 'X');
    if (line.indexOf('Y') != -1) targetY = parseValue(line, 'Y');
    if (line.indexOf('F') != -1) feedRate = parseValue(line, 'F');
    
    moveToTarget(targetX, targetY);
  }
}

// Extracts numeric values appended next to G-Code characters
float parseValue(String line, char key) {
  int index = line.indexOf(key);
  if (index == -1) return 0;
  String result = "";
  index++;
  while (index < line.length() && (isDigit(line[index]) || line[index] == '.' || line[index] == '-')) {
    result += line[index];
    index++;
  }
  return result.toFloat();
}

// Custom Kinematic Linear Profile Execution engine
void moveToTarget(float tx, float ty) {
  // Delta movements in real-world millimeters
  float dx = tx - currentX;
  float dy = ty - currentY;
  float distance = sqrt(dx*dx + dy*dy);
  
  if (distance < 0.01) return; // Prevent division bounds by zero

  // Convert target millimeter positioning into discrete step metrics
  long stepsX = round(dx * STEPS_PER_MM);
  long stepsY = round(dy * STEPS_PER_MM);

  // Establish stepper motor direction logic
  digitalWrite(X_DIR_PIN, (stepsX >= 0) ? HIGH : LOW);
  digitalWrite(Y_DIR_PIN, (stepsY >= 0) ? HIGH : LOW);

  long absStepsX = abs(stepsX);
  long absStepsY = abs(stepsY);
  long maxSteps = max(absStepsX, absStepsY);

  // Simple Bresenham-style linear interpolation stepping loop
  long xCounter = 0;
  long yCounter = 0;

  // Dynamically estimate delay to honor parsed feedrates
  // Feedrate is mm/min -> converted to microsecond delays per step unit
  float totalSteps = distance * STEPS_PER_MM;
  float stepsPerSecond = (feedRate / 60.0) * STEPS_PER_MM;
  unsigned long stepDelayMicros = (stepsPerSecond > 0) ? (1000000 / stepsPerSecond) : 500;
  
  // Sanity speed constraints to protect RAMPS drivers
  if (stepDelayMicros < 200) stepDelayMicros = 200; 

  for (long i = 0; i < maxSteps; i++) {
    bool stepX = false;
    bool stepY = false;

    xCounter += absStepsX;
    yCounter += absStepsY;

    if (xCounter >= maxSteps) {
      xCounter -= maxSteps;
      digitalWrite(X_STEP_PIN, HIGH);
      stepX = true;
    }
    if (yCounter >= maxSteps) {
      yCounter -= maxSteps;
      digitalWrite(Y_STEP_PIN, HIGH);
      stepY = true;
    }

    delayMicroseconds(10); // Keep pulse-width clean
    if (stepX) digitalWrite(X_STEP_PIN, LOW);
    if (stepY) digitalWrite(Y_STEP_PIN, LOW);

    delayMicroseconds(stepDelayMicros);
  }

  // Commit updated location coordinates to global tracking parameters
  currentX = tx;
  currentY = ty;
}