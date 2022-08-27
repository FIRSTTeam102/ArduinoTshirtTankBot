/*
 * Class to control Spark motor controllers over PWM
 * 
 * See the Spark manual
 * 
 * Pulse: 1ms - 1.5ms - 2ms
 * which means full reverse - stop - full forward and everything in between
 * except that it doesn't seem to work and i hate pwm ifsaduksdfkfsdahksadl
 * so instead we're using the servo library to write an angle in degrees
 * 
 * The servo library handles values in the 20ms period with writeMicroseconds
 */

#ifndef HEADER_SPARK
#define HEADER_SPARK

#include <Servo.h>

// from https://github.com/wpilibsuite/allwpilib/blob/main/wpilibc/src/main/native/include/frc/motorcontrol/Spark.h
#define DEG_MAX_BWD 0
#define DEG_MIN_BWD 89
#define DEG_STOP 90
#define DEG_MIN_FWD 91
#define DEG_MAX_FWD 179

class SparkPWM {
	public:
		SparkPWM(int pin): mPin{pin} {
			pinMode(mPin, OUTPUT);
			mPWM.attach(mPin);
		}

		void setSpeed(float speed) {
			speed = constrain(speed, -1.0, 1.0);
			int deg;
			if (speed < 0) {
				deg = (int) (DEG_MIN_BWD - fabs(speed) * (DEG_MIN_BWD - DEG_MAX_BWD));
			} else if (speed > 0) {
				deg = (int) (DEG_MIN_FWD + speed * (DEG_MAX_FWD - DEG_MIN_FWD));
			} else {
				deg = DEG_STOP;
			}
			Serial.print(mPin);
			Serial.print(": ");
			Serial.println(deg);
			mPWM.write(deg);
		}

	private:
		int mPin;
		Servo mPWM;
};
#endif
