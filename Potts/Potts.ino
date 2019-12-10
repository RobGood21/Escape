/*
 Name:		Potts.ino
 Created:	12/10/2019 10:19:31 AM
 Author:	Rob Antonisse
*/



//declarations
unsigned long puls;

// the setup function runs once when you press reset or power the board
void setup() {
	DDRB |= (1 << 0); //pin8 as output
	TCCR2B |= (1 << 0); //prescaler
	TIMSK2 |= (1 << 0); //enable overflow interrupt
}

// the loop function runs over and over again until power down or reset

ISR(TIMER2_OVF_vect) {
	PINB |= (1 << 0);
}
void loop() {
	
	//

	if (micros() - puls > 1) {
		PINB |= (1 << 0);
		puls = micros();
	}
	//
}

