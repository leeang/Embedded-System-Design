#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);	// RX, TX

void setup() {
	Serial.begin(9600);
	mySerial.begin(9600);
}

void loop() {
	String inString = "";

	while (Serial.available() > 0) {
		char inChar = Serial.read();
		if (inChar == '\n') {
			break;
		}
		inString += inChar;
	}

	if (inString[0] && !inString[1]) {
		switch (inString[0]) {
			case 'U':	// Up
				Serial.println("up");
				mySerial.println("up");
				break;
			case 'D':	// Down
				Serial.println("down");
				mySerial.println("down");
				break;
			case 'L':	// Left
				Serial.println("left");
				mySerial.println("left");
				break;
			case 'R':	// Right
				Serial.println("right");
				mySerial.println("right");
				break;
			case 'A':	// A
				Serial.println("buttonA");
				mySerial.println("buttonA");
				break;
			case 'B':	// B
				Serial.println("buttonB");
				mySerial.println("buttonB");
				break;
		}
	}

	if (inString[0] && inString[1]) {
		Serial.println(inString);
		mySerial.println(inString);
	}

	delay(100);
}