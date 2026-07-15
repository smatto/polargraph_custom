#include <Arduino.h>
#include <Servo.h>
#include <math.h>

// --- PIN ASSIGNMENTS ----!!
const int X_STEP_PIN   = 54; // Left Motor
const int X_DIR_PIN    = 55;
const int X_ENABLE_PIN = 38;

const int Y_STEP_PIN   = 60; // Right Motor
const int Y_DIR_PIN    = 61;
const int Y_ENABLE_PIN = 56;

const int SERVO_PIN    = 11; // Pen Lift

Servo penServo;
const int PEN_DOWN = 0;      // 0 degrees to draw
const int PEN_UP   = 60;     // 60 degrees to lift pen

// --- GEOMETRY & HARDWARE CONFIGURATION ---
const float MACHINE_WIDTH = 620.0;    
const int STEPS_PER_ROTATION = 3200;  
const float MM_PER_ROTATION = 40.0;   
const float STEPS_PER_MM = (float)STEPS_PER_ROTATION / MM_PER_ROTATION;

const float START_X = 310.0;  
const float START_Y = 300.0;  

long currentStepX = 0; long currentStepY = 0;
float baseL1, baseL2;
float currentX = START_X; float currentY = START_Y;

// Forward Declarations
void moveLinearSegmented(float targetX, float targetY);
void moveToTargetSteps(long targetX, long targetY);
void slowServoPlunge();
void slowServoLift();

// --- NEW PATH OPERATIONS FOR CONTINUOUS DRAWING ---
void startPath(float x, float y) {
  moveLinearSegmented(x, y); 
  delay(400); // Let transit settle
  slowServoPlunge();
  delay(400); // Let plunge settle
}

void drawTo(float x, float y) {
  moveLinearSegmented(x, y); // Moves directly with pen held down
}

void endPath() {
  delay(100); // Short hold at final node
  slowServoLift();
  delay(300); // Guard time
}

// Deprecated legacy hook to maintain fallback compatibility
void stroke(float x1, float y1, float x2, float y2) {
  startPath(x1, y1);
  drawTo(x2, y2);
  endPath();
}

void setup() {
  Serial.begin(115200);
  Serial.println("--- Polargraph Continuous-Path Engine Booted ---");

  pinMode(X_STEP_PIN, OUTPUT); pinMode(X_DIR_PIN, OUTPUT); pinMode(X_ENABLE_PIN, OUTPUT);
  pinMode(Y_STEP_PIN, OUTPUT); pinMode(Y_DIR_PIN, OUTPUT); pinMode(Y_ENABLE_PIN, OUTPUT);
  
  digitalWrite(X_ENABLE_PIN, LOW); digitalWrite(Y_ENABLE_PIN, LOW);
  penServo.attach(SERVO_PIN);
  penServo.write(PEN_UP); 

  baseL1 = sqrt((START_X * START_X) + (START_Y * START_Y));
  baseL2 = sqrt(((MACHINE_WIDTH - START_X) * (MACHINE_WIDTH - START_X)) + (START_Y * START_Y));
  
  delay(3000); 
  Serial.println("\n--- Injecting Vector Path ---");

  #include "machine-test-drawing.h"

  Serial.println("[MOVE] Sequence complete. Returning Home...");
  moveLinearSegmented(START_X, START_Y);
  delay(500);

  digitalWrite(X_ENABLE_PIN, HIGH); digitalWrite(Y_ENABLE_PIN, HIGH);
}

void loop() {}

void slowServoPlunge() {
  for (int pos = PEN_UP; pos >= PEN_DOWN; pos -= 1) { penServo.write(pos); delay(6); }
}

void slowServoLift() {
  for (int pos = PEN_DOWN; pos <= PEN_UP; pos += 1) { penServo.write(pos); delay(6); }
}

void moveLinearSegmented(float targetX, float targetY) {
  float distance = sqrt(pow(targetX - currentX, 2) + pow(targetY - currentY, 2));
  int segments = ceil(distance / 2.0); 
  if (segments < 1) segments = 1;
  float stepDeltaX = (targetX - currentX) / segments;
  float stepDeltaY = (targetY - currentY) / segments;

  for (int i = 0; i < segments; i++) {
    currentX += stepDeltaX; currentY += stepDeltaY;
    float targetL1 = sqrt((currentX * currentX) + (currentY * currentY));
    float targetL2 = sqrt(((MACHINE_WIDTH - currentX) * (MACHINE_WIDTH - currentX)) + (currentY * currentY));
    long targetStepX = (targetL1 - baseL1) * STEPS_PER_MM;
    long targetStepY = (targetL2 - baseL2) * STEPS_PER_MM;
    moveToTargetSteps(targetStepX, targetStepY);
  }
  currentX = targetX; currentY = targetY;
}

void moveToTargetSteps(long targetX, long targetY) {
  long deltaX = targetX - currentStepX; long deltaY = targetY - currentStepY;
  digitalWrite(X_DIR_PIN, (deltaX >= 0) ? LOW : HIGH); 
  digitalWrite(Y_DIR_PIN, (deltaY >= 0) ? HIGH : LOW);
  long absDeltaX = abs(deltaX); long absDeltaY = abs(deltaY);
  long maxDelta = max(absDeltaX, absDeltaY);
  long accumX = 0; long accumY = 0;
  
  for (long i = 0; i < maxDelta; i++) {
    accumX += absDeltaX; accumY += absDeltaY;
    bool stepX = false; bool stepY = false;
    if (accumX >= maxDelta) { accumX -= maxDelta; stepX = true; currentStepX += (deltaX >= 0) ? 1 : -1; }
    if (accumY >= maxDelta) { accumY -= maxDelta; stepY = true; currentStepY += (deltaY >= 0) ? 1 : -1; }
    if (stepX) digitalWrite(X_STEP_PIN, HIGH);
    if (stepY) digitalWrite(Y_STEP_PIN, HIGH);
    delayMicroseconds(2);
    if (stepX) digitalWrite(X_STEP_PIN, LOW);
    if (stepY) digitalWrite(Y_STEP_PIN, LOW);
    delayMicroseconds(600); 
  }
}
