/*
 Name:		Projector.ino
 Created:	November 2019
 Author:	Rob Antonisse
 Compagny:  Escape world group bv.
*/

//declarations

unsigned long pulstijd;
unsigned long pwmtijd;
unsigned int pulsduur;
unsigned long filmtijd;
unsigned long uittijd;

byte COM_reg;



void setup() {
	DDRD |= (1 << 7); //PIN 7 output
	pulsduur = 250; //10% duty cycle
}
void lens() {

	if (millis() - filmtijd > 65 & bitRead(COM_reg,1)==true) {
		filmtijd = millis();
		COM_reg |= (1 << 0);
		uittijd = millis();
		PORTD &= ~(1 << 7);
	}

	if (bitRead(COM_reg, 0) == true) {
		if (millis() - uittijd > 50) {
			COM_reg &= ~(1 << 0);
		}
	}
	else {

		if (micros() - pwmtijd > pulsduur) {
			PORTD &= ~(1 << 7);
		}

		if (millis() - pulstijd > 1) {
			pulstijd = millis();
			PORTD |= (1 << 7);
			pwmtijd = micros();
		}
	}

}

void loop() {
	lens();
}
