#include "Arduino.h"
#include <AccelStepper.h>

// This is an improvement over the standard Stepper library included with Arduino
#define HALFSTEP 8

// Stepper GPIO pins on Arduino board
#define motorPin1  3
#define motorPin2  4
#define motorPin3  5
#define motorPin4  6

// Motor pin definitions
// Blue   - 28BYJ48 pin 1
// Pink   - 28BYJ48 pin 2
// Yellow - 28BYJ48 pin 3
// Orange - 28BYJ48 pin 4
// Red    - 28BYJ48 pin 5 (VCC)

// NOTE: The sequence 1-3-2-4 is required for proper sequencing of 28BYJ48
AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

// -------------------------------------------------------------------------
// SETUP     SETUP     SETUP     SETUP     SETUP     SETUP     SETUP
// -------------------------------------------------------------------------
void setup()
{
  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(50.0);
  stepper1.setSpeed(50);
  stepper1.moveTo(2048);  // about 1 revolution
}

// -------------------------------------------------------------------------
// LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP
// -------------------------------------------------------------------------
void loop()
{
  //Change direction at the limits
  if (stepper1.distanceToGo() == 0)
    stepper1.moveTo(-stepper1.currentPosition());

  stepper1.run();
}
