#include <EthernetENC.h> // for ENC28J60
#include <EthernetUdp.h> // for UDP
#include <Servo.h>

// pin mappings: https://docs.arduino.cc/hacking/hardware/PinMapping2560

// cannon (https://dylan-frc-docs.readthedocs.io/en/latest/_images/spikeRelay2Light.png)
#define PIN_SOLENOID_LOAD 22 // spike m+
#define PIN_SOLENOID_FIRE 23 // spike m-

// tank drive (https://revrobotics.com/content/docs/REV-11-1200-UM.pdf)
#define PIN_TANK_LEFT 5
#define PIN_TANK_RIGHT 6
Servo tankLeft;
Servo tankRight;
#define TANK_MULT_LEFT -1.0
#define TANK_MULT_RIGHT 1.0

// #define DEBUG // log packets

/* start reused code between robot and controller */
#define UDP_TX_PACKET_MAX_SIZE 48 // default 24
#define PORT 10000
IPAddress robotIP(192, 168, 1, 176);
IPAddress controllerIP(192, 168, 1, 177);

void displayIPAddress(const IPAddress address, unsigned int port) {
	Serial.print("IP ");
	for (int i = 0; i < 4; i++) {
		Serial.print(address[i], DEC);
		if (i < 3) Serial.print(".");
	}
	Serial.print(":");
	Serial.println(port);
}

// first part of my udp message, contains metadata
struct MessageHeader {
	byte header;
	unsigned long timestamp;
};

struct ControllerMessage {
	MessageHeader header;
	float left;
	float right;
	bool load : 1;
	bool fire : 1;
};
/* end reused code between robot and controller */

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x66, 0x00 };
EthernetUDP Udp;

char packetBuffer[UDP_TX_PACKET_MAX_SIZE] = { 0 }; // buffer to hold incoming packet
struct ControllerMessage controllerMessage; // last contoller data

void setup() {
	Serial.begin(9600);

	pinMode(PIN_TANK_LEFT, OUTPUT);
	tankLeft.attach(PIN_TANK_LEFT);
	pinMode(PIN_TANK_RIGHT, OUTPUT);
	tankRight.attach(PIN_TANK_RIGHT);
	pinMode(PIN_SOLENOID_LOAD, OUTPUT);
	pinMode(PIN_SOLENOID_FIRE, OUTPUT);

	Ethernet.init(10); // CS pin
	mac[5] = robotIP[3]; // change default MAC address to IP
	Ethernet.begin(mac, robotIP);

	if (Ethernet.hardwareStatus() == EthernetNoHardware) {
		Serial.println("No ethernet hardware found");
		while (true);
	}

	Udp.begin(PORT);
	Serial.print("Ethernet UDP started ");
	displayIPAddress(controllerIP, PORT);
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
void setSparkSpeed(Servo motor, float speed, float mult) {
	speed = constrain(speed, -1.0, 1.0) * mult;
	int pulse;
	if (speed < 0) pulse = (int)(PWM_MIN_BWD - fabs(speed) * (PWM_MIN_BWD - PWM_MAX_BWD));
	else if (speed > 0) pulse = (int)(PWM_MIN_FWD + speed * (PWM_MAX_FWD - PWM_MIN_FWD));
	else pulse = PWM_STOP;
	motor.writeMicroseconds(pulse);
}

void loop() {
	if (Ethernet.linkStatus() == LinkOFF) {
		Serial.println("Ethernet link lost");
		while (Ethernet.linkStatus() != LinkON);
		Udp.begin(PORT);
		Serial.println("Ethernet link established");
	}

	int packetSize = Udp.parsePacket();
	if (packetSize) {
		// Serial.print("Received packet of size "); Serial.print(packetSize);
		// Serial.print(" from "); displayIPAddress(Udp.remoteIP(), Udp.remotePort());

		// read the packet into packetBuffer
		Udp.read(packetBuffer, packetSize);

		if (((MessageHeader*)packetBuffer)->header == 0b10) { // controller update
			memcpy((void*)&controllerMessage, (void*)packetBuffer, sizeof(ControllerMessage)); // store controller data until we get a new one
			// Serial.print(controllerMessage.header.timestamp);
		} else Serial.println("unknown packet");

		#ifdef DEBUG
		for (int i = 0; i < packetSize; i++) {
			if (packetBuffer[i] <= 0x0F) Serial.print("0");
			Serial.print(packetBuffer[i], HEX);
		}
		Serial.println();
		#endif
	}

	// do driving
	setSparkSpeed(tankLeft, controllerMessage.left, TANK_MULT_LEFT);
	setSparkSpeed(tankRight, controllerMessage.right, TANK_MULT_RIGHT);

	// todo: solenoids
	// todo: don't shoot if person detected
	// todo: send status information (tank/1/2 pressure, person detected)
}