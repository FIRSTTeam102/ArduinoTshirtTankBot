#include <EthernetENC.h> // for ENC28J60
#include <EthernetUdp.h> // for UDP

// controller inputs
#define POT_DRIVE_SPEED A0
#define BTN_LEFT_FWD 2
#define BTN_LEFT_BWD 3
#define BTN_RIGHT_FWD 4
#define BTN_RIGHT_BWD 5
#define BTN_LOAD 6
#define BTN_FIRE 7

#define DRIVE_SPEED_MIN 0.0
#define DRIVE_SPEED_MAX 1.0

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
struct ControllerMessage controllerMessage = { 0b10 }; // buffer to hold outgoing controller data, initialized with message id

void setup() {
	Serial.begin(9600);

	pinMode(BTN_LEFT_FWD, INPUT_PULLUP);
	pinMode(BTN_LEFT_BWD, INPUT_PULLUP);
	pinMode(BTN_RIGHT_FWD, INPUT_PULLUP);
	pinMode(BTN_RIGHT_BWD, INPUT_PULLUP);
	pinMode(BTN_LOAD, INPUT_PULLUP);
	pinMode(BTN_FIRE, INPUT_PULLUP);

	Ethernet.init(10); // CS pin
	mac[5] = controllerIP[3]; // change default MAC address to IP
	Ethernet.begin(mac, controllerIP);

	if (Ethernet.hardwareStatus() == EthernetNoHardware) {
		Serial.println("No ethernet hardware found");
		while (true);
	}

	Udp.begin(PORT);
	Serial.print("Ethernet UDP started ");
	displayIPAddress(controllerIP, PORT);
}

float maxSpeed;
float getDriveSpeed(byte btnFwd, byte btnBwd) {
	if (!digitalRead(btnFwd)) return maxSpeed;
	else if (!digitalRead(btnBwd)) return -maxSpeed;
	else return 0.0;
}

void loop() {
	if (Ethernet.linkStatus() == LinkOFF) {
		Serial.println("Ethernet link lost");
		while (Ethernet.linkStatus() != LinkON);
		Udp.begin(PORT);
		Serial.println("Ethernet link established");
	}

	// update and send controller message
	maxSpeed = analogRead(POT_DRIVE_SPEED) * (DRIVE_SPEED_MAX - DRIVE_SPEED_MIN) / 1024.0 + DRIVE_SPEED_MIN;
	controllerMessage.header.timestamp = millis();
	controllerMessage.left = getDriveSpeed(BTN_LEFT_FWD, BTN_LEFT_BWD);
	controllerMessage.right = getDriveSpeed(BTN_RIGHT_FWD, BTN_RIGHT_BWD);
	controllerMessage.load = !digitalRead(BTN_LOAD);
	controllerMessage.fire = !digitalRead(BTN_FIRE);

	Udp.beginPacket(robotIP, PORT);
	Udp.write((uint8_t*)&controllerMessage, sizeof(controllerMessage)); // turn struct into generic data, will be identified on robot by header
	Udp.endPacket();

	// todo: check for a message from the server
	int packetSize = Udp.parsePacket();
	if (packetSize) {
		Serial.print("Received packet of size ");
		Serial.print(packetSize);
		Serial.print(" from ");
		displayIPAddress(Udp.remoteIP(), Udp.remotePort());

		// read the packet into packetBuffer
		Udp.read(packetBuffer, packetSize); // UDP_TX_PACKET_MAX_SIZE);
	}
}
