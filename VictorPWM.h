/*
 * Class to control Victor 883-888 motor controllers over PWM
 * 
 * Victors use "rc pwm" - small pulse in a 20ms period
 * https://commons.wikimedia.org/wiki/File:Servomotor_Timing_Diagram.svg
 * 
 * Pulse: 1ms - 1.5ms - 2ms
 * which means full forward - stop - full reverse and everything in between
 * 
 * The servo library handles values in the 20ms period with writeMicroseconds
 */

#ifndef HEADER_VICTOR
#define HEADER_VICTOR

// from https://github.com/wpilibsuite/allwpilib/blob/main/wpilibc/src/main/native/include/frc/motorcontrol/Victor.h
#define PWM_MAX_BWD 1026
#define PWM_MIN_BWD 1490
#define PWM_STOP 1507
#define PWM_MIN_FWD 1525
#define PWM_MAX_FWD 2027

#include <Servo.h>

class VictorPWM {
	public:
		VictorPWM(int pin): mPin{pin} {
			pinMode(pin, OUTPUT);
			mPWM.attach(pin);
		}

		void setSpeed(float speed) {
			speed = constrain(speed, -1.0, 1.0);
			int pulseLength; // μs
			if (speed < 0) {
				// output from 1499 to 1000
				pulseLength = (int) (PWM_MIN_BWD - fabs(speed) * (PWM_MIN_BWD - PWM_MAX_BWD));
			} else if (speed > 0) {
				// output from 1501 to 2000
				pulseLength = (int) (PWM_MIN_FWD + speed * (PWM_MAX_FWD - PWM_MIN_FWD));
			} else {
				pulseLength = PWM_STOP;
			}
			Serial.println(pulseLength);

			mPWM.writeMicroseconds(pulseLength);
		}

	private:
		int mPin;
		Servo mPWM;
};
#endif
