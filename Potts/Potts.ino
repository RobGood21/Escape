/*
 Name:		Potts.ino
 Created:	12/10/2019 10:19:31 AM
 Author:	Rob Antonisse
*/



//declarations
unsigned long puls;

// the setup function runs once when you press reset or power the board
void setup() {
	DDRB |= (1 << 3); //pin11 as output
	TCCR2B |= (1 << 0); //prescaler
	//TIMSK2 |= (1 << 0); //enable overflow interrupt

	TCCR2A |= (1 << 6); //Toggle OC2A on Compare Match
	//TCCR2A |= (1 << 1); //clear timer on compare A
	TCCR2B |= (1 << 3);
	TIMSK2 |= (1 << 1); //enable OC2A compare interrupt
	//OCR2A = 0x05;

}

// the loop function runs over and over again until power down or reset

//ISR(TIMER2_COMPA_vect) {
	//TCNT2 = 0x00; //reset counter
//}

//ISR(TIMER2_OVF_vect) {
//	PINB |= (1 << 0);
//}

void loop() {
	
	//

	//if (micros() - puls > 1) {
	//	PINB |= (1 << 0);
	//	puls = micros();
	//}
	//
}

