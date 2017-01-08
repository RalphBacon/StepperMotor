#include "Arduino.h"

// The preferred stepper motor driver
#include <AccelStepper.h>
#include <OneWire.h>

// Easy-to-use OneWire DS18B20 temperature library
#include <DallasTemperature.h>

// We want half step (not full step) control of the stepper
#define HALFSTEP 8

// Motor pin definitions
// Blue   - 28BYJ48 pin 1
// Pink   - 28BYJ48 pin 2
// Yellow - 28BYJ48 pin 3
// Orange - 28BYJ48 pin 4
// Red    - 28BYJ48 pin 5 (VCC)

#define motorPin1 3		// IN1 on the ULN2003 driver 1
#define motorPin2 4		// IN2 on the ULN2003 driver 1
#define motorPin3 5		// IN3 on the ULN2003 driver 1
#define motorPin4 6		// IN4 on the ULN2003 driver 1

#define sensorPin 2		// Stepper microswitch sensor
#define tempPin 12		// DS18B20 Temperature sensor

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper stepMotor(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

// Initialise OneWire comms
OneWire oneWire(tempPin);

// DallasTemp object needs that OneWire setup
DallasTemperature tempSensor(&oneWire);

// Temperature capture
int currTemp = 0;
int oldTemp = 0;

// -------------------------------------------------------------------
// SETUP     SETUP    SETUP    SETUP    SETUP    SETUP    SETUP
// -------------------------------------------------------------------
void setup() {
	Serial.begin(9600);

	// Start the temperature sensor
	tempSensor.begin();

	// Set some constraints on the stepper motor
	stepMotor.setMaxSpeed(500.0);
	stepMotor.setAcceleration(100.0);
	stepMotor.setSpeed(50);

	// Complete anticlockwise circle until we find the reference point
	stepMotor.moveTo(4096 + 100); //4096

	// This is the microswitch reference point
	pinMode(sensorPin, INPUT_PULLUP);

	// Power to the motor is ON
	stepMotor.enableOutputs();
	Serial.println("Moving stepper to known reference point");

	//If we are already STOPPED then move off the mark and re-reference
	while (digitalRead(sensorPin) == LOW) {
		stepMotor.run();
	}

	// Now continue to turn until we reach our reference point
	while (!digitalRead(sensorPin) == LOW) {
		stepMotor.run();
	}

	// Stop when reference point reached and set the zero position
	stepMotor.stop();
	stepMotor.setCurrentPosition(0);
	Serial.println("Stepper motor at reference point.");

	// Set the stepper motor power off
	stepMotor.disableOutputs();
	delay(1000);
}

long mapTempToPos(long newTemp);
void doPointerMove();

// -------------------------------------------------------------------
// LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP
// -------------------------------------------------------------------
void loop() {

	// 1. Get the temperature and remember its value
	// 2. Move pointer to temperature value via stepper
	// 3. Repeat if temperature changes (whole degrees only)
	// 4. Allow serial monitor input if connected to IDE

	// Get temperature reading in whole degrees rounded to nearest value
	tempSensor.requestTemperatures();
	currTemp = (int) round(tempSensor.getTempCByIndex(0));

	// Has temperature changed (significantly - i.e. whole degrees)
	if (currTemp != oldTemp) {
		doPointerMove();
	}
	else
	{
		// Allow serial monitor input (for testing!) If you put in
		// non-numeric values the world as we know it will end.
		char manualTemp[3];
		if (Serial.available()){
			// Read all NUMERIC characters until new line
			Serial.println("Manual entry detected");
			int charCnt = Serial.readBytesUntil('\n', manualTemp, 3);
			manualTemp[charCnt] = '\0';

			// Overwrite the current temperature sensor value
			currTemp = atoi(manualTemp);
			doPointerMove();
		}
	}

	// Loop delay
	delay(5000);
}

// -------------------------------------------------------------------
// POINTER MOVE       POINTER MOVE      POINTER MOVE      POINTER MOVE
// -------------------------------------------------------------------
void doPointerMove() {
	Serial.println("Temperature Change Detected");

	Serial.print("Temperature: ");
	Serial.print(oldTemp);
	Serial.print(" vs. ");
	Serial.println(currTemp);

	stepMotor.moveTo(mapTempToPos(currTemp));

	// turn on stepper motor
	stepMotor.enableOutputs();
	Serial.println("Stepper Motor ON");
	stepMotor.run();

	Serial.println("Stepper moving to new position");
	while (stepMotor.isRunning()) {
		stepMotor.run();
	}

	// turn off stepper motor
	stepMotor.disableOutputs();
	Serial.println("Stepper Motor OFF");

	// Remember current temperature
	oldTemp = currTemp;
}

// -------------------------------------------------------------------
// MAP TEMP TO SCALE         MAP TEMP TO SCALE       MAP TEMP TO SCALE
// -------------------------------------------------------------------
long mapTempToPos(long newTemp) {

	// Convert the temperature to the stepper 90-degree range
	// Note that if you change this the pointer will have a different range
	long mappedTemp = map(newTemp - 5, -5, 30, 0, 1024);

	// Debugging messages
	Serial.print("Input temperature: ");
	Serial.print(newTemp);
	Serial.print("\tMapped to scale value: ");
	Serial.println(mappedTemp);

	// All done
	return -mappedTemp;
}
