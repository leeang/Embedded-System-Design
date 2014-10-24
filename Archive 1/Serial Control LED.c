char inChar;

void setup() {
	Serial.begin(9600);
	pinMode(13, OUTPUT);
}

void loop() {
	while (Serial.available() > 0) {
		inChar = Serial.read();
		Serial.println(inChar);
	}

	if (inChar == 'W')
		digitalWrite(13, HIGH);
	if (inChar == 'B')
		digitalWrite(13, LOW);

	delay(100);
}

/*
#define F_CPU 8E6
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16))) - 1)
*/