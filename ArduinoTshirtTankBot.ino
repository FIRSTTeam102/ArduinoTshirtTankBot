#include <Servo.h>

#define PIN_TANK_LEFT 5
#define PIN_TANK_RIGHT 6
Servo tankLeft;
Servo tankRight;
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
	tankLeft.attach(PIN_TANK_LEFT);
	pinMode(PIN_TANK_LEFT, OUTPUT);
	tankRight.attach(PIN_TANK_RIGHT);
	pinMode(PIN_TANK_RIGHT, OUTPUT);
	pinMode(BTN_LEFT_FWD, INPUT_PULLUP);
	pinMode(BTN_LEFT_BWD, INPUT_PULLUP);
	pinMode(BTN_RIGHT_FWD, INPUT_PULLUP);
	pinMode(BTN_RIGHT_BWD, INPUT_PULLUP);
}

void updateFloatFromSerial(float *val) {
	if (Serial.available() > 0) {
		String input = Serial.readStringUntil('\n');
		*val = (float)atof(input.c_str());
	}
}

/*
 * Writes the speed to the motor
 * Pulse: 1ms - 1.5ms - 2ms
 * which means full reverse - stop - full forward and everything in between
 * The servo library handles values in the 20ms period with writeMicroseconds
 */
// from https://github.com/wpilibsuite/allwpilib/blob/main/wpilibc/src/main/native/include/frc/motorcontrol/Spark.h
#define PWM_MAX_BWD 999
#define PWM_MIN_BWD 1450
#define PWM_STOP 1500
#define PWM_MIN_FWD 1550
#define PWM_MAX_FWD 2003
void setSparkSpeed(Servo motor, float speed) {
	speed = constrain(speed, -1.0, 1.0);
	int pulse;
	if (speed < 0) {
		pulse = (int)(PWM_MIN_BWD - fabs(speed) * (PWM_MIN_BWD - PWM_MAX_BWD));
	} else if (speed > 0) {
		pulse = (int)(PWM_MIN_FWD + speed * (PWM_MAX_FWD - PWM_MIN_FWD));
	} else
		pulse = PWM_STOP;
	motor.writeMicroseconds(pulse);
}

// handles reading and setting the speed of the motor from 3-val rocker
void driveMotorSpeed(Servo motor, float *spd, byte btnFwd, byte btnBwd) {
	if (!digitalRead(btnFwd))
		*spd = driveSpeed;
	else if (!digitalRead(btnBwd))
		*spd = -1.0 * driveSpeed;
	else
		*spd = 0.0;
	setSparkSpeed(motor, *spd);
};

void loop() {
	driveSpeed = analogRead(POT_SPEED) * (DRIVE_SPEED_MAX - DRIVE_SPEED_MIN) / 1024.0 + DRIVE_SPEED_MIN;

	driveMotorSpeed(tankLeft, &tankLeftSpeed, BTN_LEFT_FWD, BTN_LEFT_BWD);
	driveMotorSpeed(tankRight, &tankRightSpeed, BTN_RIGHT_FWD, BTN_RIGHT_BWD);

	// updateFloatFromSerial(&tankLeftSpeed);
	// setSparkSpeed(tankLeft, driveSpeed);

	Serial.print("speed: L");
	Serial.print(tankLeftSpeed);
	Serial.print(" R");
	Serial.println(tankRightSpeed);
}
