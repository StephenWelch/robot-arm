#include <Arduino.h>

#include <Arduino.h>
#include <Wire.h>
#include <AccelStepper.h>
#include "angle_stepper.h"

#define STEPS_PER_REV_28BYJ48 4076
#define GEAR_RATIO_28BYJ48 63.68395

// Pin order is 1-3-2-4
AngleStepper lowerLeft = AngleStepper(4076, 360, false, AccelStepper::MotorInterfaceType::HALF4WIRE, 12, 10, 11, 9);
AngleStepper lowerRight = AngleStepper(4076, 360, true, AccelStepper::MotorInterfaceType::HALF4WIRE, 8, 6, 7, 5);
AngleStepper upperLeft = AngleStepper(4076, 360, false, AccelStepper::MotorInterfaceType::HALF4WIRE, 4, 2, 3, 14);
AngleStepper upperRight = AngleStepper(4076, 360, true, AccelStepper::MotorInterfaceType::HALF4WIRE, 15, 17, 16, 18);

AngleStepper* allSteppers[] = {&lowerLeft, &lowerRight, &upperLeft, &upperRight};
const int NUM_STEPPERS = sizeof(allSteppers) / sizeof(AngleStepper*);

bool first_run = true;

/**
 * Calls each stepper's run() function.
 * Returns true if any steppers are still running, false if steppers are all stopped.
**/
boolean runAll() {
	boolean allRunning = false;
	for(auto& stepper : allSteppers) {
		if(stepper->run()) {
			allRunning = true;
		}
	}

	return allRunning;
}

/**
 * Disables all steppers so they can be turned by hand
**/
void disableAll() {
	for(auto& stepper : allSteppers) {
		stepper->disableOutputs();
	}
}

void sendStatus() {
	Serial.println("LOWER");
	Serial.print(lowerLeft.currentAbsolutePosition());
	Serial.print(" ");
	Serial.print(lowerRight.currentAbsolutePosition());
	Serial.println();

	Serial.println("UPPER");
	Serial.print(upperLeft.currentAbsolutePosition());
	Serial.print(" ");
	Serial.print(upperRight.currentAbsolutePosition());
	Serial.println();
}

void testRelativeMotion() {
	lowerLeft.move(90);
	lowerRight.move(90);
	upperLeft.move(-85);
	upperRight.move(-85);
	while(runAll());

	delay(500);

	lowerLeft.move(-90 * 2);
	lowerRight.move(-90 * 2);
	upperLeft.move(85 * 2);
	upperRight.move(85 * 2);
	while(runAll());

	delay(500);

	lowerLeft.move(90);
	lowerRight.move(90);
	upperLeft.move(-85);
	upperRight.move(-85);
	while(runAll());
}

void testAbsoluteMotion() {
	lowerLeft.setMinAngle(270);
	lowerLeft.setMaxAngle(90);
	lowerRight.setMinAngle(270);
	lowerRight.setMaxAngle(90);

	upperLeft.setMinAngle(275);
	upperLeft.setMaxAngle(90);
	upperRight.setMinAngle(275);
	upperRight.setMaxAngle(90);

	lowerLeft.moveTo(90);
	lowerRight.moveTo(90);
	upperLeft.moveTo(275);
	upperRight.moveTo(275);
	while(runAll());

	lowerLeft.moveTo(0);
	lowerRight.moveTo(0);
	upperLeft.moveTo(0);
	upperRight.moveTo(0);
	while(runAll());

	lowerLeft.moveTo(270);
	lowerRight.moveTo(270);
	upperLeft.moveTo(85);
	upperRight.moveTo(85);
	while(runAll());

	lowerLeft.moveTo(0);
	lowerRight.moveTo(0);
	upperLeft.moveTo(0);
	upperRight.moveTo(0);
	while(runAll());
}

void testMinMaxAnglesSmallBounds() {
	lowerLeft.setMinAngle(315);
	lowerLeft.setMaxAngle(45);

	lowerRight.setMinAngle(315);
	lowerRight.setMaxAngle(45);

	lowerLeft.moveTo(90);
	lowerRight.moveTo(90);
	while(runAll());

	lowerLeft.moveTo(45);
	lowerRight.moveTo(45);
	while(runAll());

	lowerLeft.moveTo(270);
	lowerRight.moveTo(270);
	while(runAll());

	lowerLeft.moveTo(0);
	lowerRight.moveTo(0);
	while(runAll());

	lowerLeft.moveTo(180);
	lowerRight.moveTo(180);
	while(runAll());
}

void testMinMaxAnglesBigBounds() {
	
	lowerLeft.setMinAngle(45);
	lowerLeft.setMaxAngle(315);

	lowerRight.setMinAngle(45);
	lowerRight.setMaxAngle(315);

	lowerLeft.setCurrentPosition(50);
	lowerRight.setCurrentPosition(50);

	lowerLeft.moveTo(270);
	lowerRight.moveTo(270);
	while(runAll());

}

void setup()
{
	Serial.begin(115200);
	Serial.print("Initializing arm with ");
	Serial.print(NUM_STEPPERS);
	Serial.println(" steppers");

	for(auto& stepper : allSteppers) {
		stepper->setMaxSpeed(90);
		stepper->setAcceleration(90);
		stepper->setCurrentPosition(0);
	}

	Serial.println("Finished initialization");



	disableAll();
}

void loop()
{
	testAbsoluteMotion();
}