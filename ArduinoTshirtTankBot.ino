#include "SparkPWM.h"

SparkPWM tankLeft{5};
SparkPWM tankRight{6};
float tankLeftSpeed = 0.0;
float tankRightSpeed = 0.0;

#define BTN_LEFT_FWD 7
#define BTN_LEFT_BWD 8
#define BTN_RIGHT_FWD 12
#define BTN_RIGHT_BWD 13
#define POT_SPEED A0

#define DRIVE_SPEED_MIN -1.0
#define DRIVE_SPEED_MAX 1.0
float driveSpeed = 0.5; // adjustable with pot

void setup() {
	Serial.begin(9600);
	pinMode(BTN_LEFT_FWD, INPUT_PULLUP);
	pinMode(BTN_LEFT_BWD, INPUT_PULLUP);
	pinMode(BTN_RIGHT_FWD, INPUT_PULLUP);
	pinMode(BTN_RIGHT_BWD, INPUT_PULLUP);
}

void updateFloatFromSerial(float* val) {
	if (Serial.available() > 0) {
		String input = Serial.readStringUntil('\n');
		*val = (float)atof(input.c_str());
	}
}

// handles reading and setting the speed of the motor from 3-val rocker
void updateMotorSpeed(SparkPWM* motor, float* spd, byte btnFwd, byte btnBwd) {
	if (!digitalRead(btnFwd)) *spd = driveSpeed;
	else if (!digitalRead(btnBwd)) *spd = -1.0 * driveSpeed;
	else *spd = 0.0;

	motor->setSpeed(*spd);
};

void loop() {
	driveSpeed = analogRead(POT_SPEED) * (DRIVE_SPEED_MAX - DRIVE_SPEED_MIN) / 1024.0 + DRIVE_SPEED_MIN;

//	updateMotorSpeed(&tankLeft, &tankLeftSpeed, BTN_LEFT_FWD, BTN_LEFT_BWD);
//	updateMotorSpeed(&tankRight, &tankRightSpeed, BTN_RIGHT_FWD, BTN_RIGHT_BWD);

//	updateFloatFromSerial(&tankLeftSpeed);
	tankLeft.setSpeed(driveSpeed);

	Serial.print("speed: L"); Serial.print(tankLeftSpeed);
	Serial.print(" R"); Serial.println(tankRightSpeed);

	delay(15);
}
