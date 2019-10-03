/*
	Name:       Blower.ino
	Created:	30-9-2019 13:00:20
	Author:     Rob Antonisse
	Sketch for Arduino Uno for candle blow puzzle


*/

//for Fastled
#include <FastLED.h>
#define fastled COM_reg |=(1<<0);

unsigned int timered = 15000; //time a blownout cabdle stays out...15sec

CRGB kaars[10];
byte TargetRed[10];
byte StepRed[10];
byte TargetGreen[10];
byte StepGreen[10];
byte TargetBlue[10];
byte StepBlue[10];
byte speed[10];
byte C_reg[10];
byte fcount[10]; //counter of flickering effect, auto stop
byte blowcount[10]; //time candle is blown

unsigned long outtime[10];

float RG = 3.2; //relation between red and green
float GB = 6.5; //relation between green and blue


byte count[3];
byte countrandom;
byte switchcount;
byte speedcount;
byte switchstatus[2]; //0=port D 1=port C
byte COM_reg;


//tijdelijk
unsigned long tijd;
unsigned long periode;

void setup()
{
	Serial.begin(9600);
	FastLED.addLeds<NEOPIXEL, 8>(kaars, 10);
	DDRD = 0x0;//portD as inputs
	PORTD = 0xFF; //pullups on portD
	DDRC = 0x0;
	PORTC = 0xFF; //set port C input with pull up.

	DDRB = 0xFF; //portB as outputs	
	Serial.println("Hallo, dit is um....");
}
void slowevents() { //called from loop
	//switches, switches are low active	
	byte changed;
	byte krs;
	byte offset;
	byte inputs;
	byte pins;
	burn();
	switchcount++;
	if (switchcount == 10) { //10 speed of reading the switches
		switchcount = 0;
		//pressed switch
		//opletten i=hier de switch blowcount= aan de kaars

		if (bitRead(COM_reg, 1) == false) timeout(); //only if puzzle is not solved		

		for (byte s = 0; s < 2; s++) {
			if (s == 0) {
				inputs = PIND;
				pins = 8;
			}
			else {
				inputs = PINC;
				pins = 5;
			}
			for (byte i = 0; i < pins; i++) {
				if (s == 0) {
					krs = 7 - i;
				}
				else {
					krs = 5 + i;
				}					

				if (bitRead(inputs, i) == false & bitRead(switchstatus[s], i) == false) {
					Serial.println(krs);
					blowcount[krs]++;
				}				
				if (blowcount[krs] > 3 & bitRead(C_reg,4)==false) blowout(krs); //3 ? voor test ff 10
			}

		//}
		//for (byte s = 0; s < 2; s++) {
			//changed status switch
		//	if (s == 0) {
		//		inputs = PIND;
		//		pins = 8;
		//	}
		//	else {
		//		inputs = PINC;
		//		pins = 5;
		//	}

			changed = inputs ^ switchstatus[s];
			if (changed > 0) { //switch changed
				//
				for (byte i = 0; i < pins; i++) { //check all switch states
					if (s == 0) {
						krs = 7 - i;
					}
					else {
						krs = 5 + i;
					}

					if (bitRead(changed, i) == true) {
						if (bitRead(inputs, i) == true) { //switch released
							switchstatus[s] |= (1 << i);
							//blowcount[krs] = 0;

						}
						else { //switch pushed
							switchstatus[s] &= ~(1 << i);
							switched(krs); //button pushed
							blowcount[krs] = 0; //dit naar switched...??
						}
					}
				}
			}
		}
	}
}

void switched(byte blow) {
	//Serial.println(blow);

	//kaars[blow] = 0xFFFFFF;


	C_reg[blow] ^= (1 << 2); //flickering onoff
	fcount[blow] = random(30, 60);

	//fastled
}

void blowout(byte krs) { //called from slowevents
	//hier ook de overgang tussen 2x5 switches naar 10x kaars maken., krs zou de kaars moeten zijn

	C_reg[krs] |= (1 << 4);
	C_reg[krs] &= ~(B00000110 << 1); //reset efx
	//PINB |= (1 << 5);
	blowcount[krs] = 0;
	speed[krs] = 0; //stop running proces
	kaars[krs] = 0x000000;
	outtime[krs] = millis();
}

void timeout() { //called from slowevents
	//checks time candle is been blown out
	for (byte k; k < 10; k++) {
		if (bitRead(C_reg[k], 4) == true) {
			if (millis() - outtime[k] > timered) {
				C_reg[k] &= ~(1 << 4); //rest candle
			}
		}
	}
}

void burn() {
	//byte i = 0; //temp
	countrandom++;
	if (countrandom == 255)randomSeed(analogRead(0));

	for (byte i=0; i < 10; i++) {

	if (speed[i] == 0) { //targetwaarde bereikt
		speed[i] = random(10, 60);


		if (bitRead(C_reg[i], 1) == true) { //stop flash
			C_reg[i] &= ~(1 << 1);
			kaars[i] = 0xAA3000;
		}

		//efx
		if (speed[i] == 5 & bitRead(C_reg[i], 4) == false) C_reg[i] |= (1 << 1); //pixel full, not if blowout is active

		if (speed[i] == 40 & bitRead(C_reg[i], 4) == false) { //random flicker effect, not in blowout
			C_reg[i] |= (1 << 2);
			fcount[i] = random(5, 50);
		}

		if (bitRead(C_reg[i], 2) == true) { //flickering

			speed[i] = speed[i] / 2; //speed up effect
			C_reg[i] ^= (1 << 3); //toggle direction in flicker

			if (bitRead(C_reg[i], 3) == true) { //up
				TargetRed[i] = random(200, 255);
			}
			else { //doen
				TargetRed[i] = random(30, 150);
			}

			fcount[i]--; //fcount random set when set bit2 in C_reg
			if (fcount[i] == 0) {
				C_reg[i] &= ~(1 << 2); //stop flickering effect				
			}
		}
		else { //slow burn
			TargetRed[i] = random(150, 255);
		}

		if (bitRead(C_reg[i], 4) == true) { //blow out
			TargetRed[i] = random(15, 80);
			TargetGreen[i] = 4;
			TargetBlue[i] = 1;
		}
		else { //burn
			TargetGreen[i] = TargetRed[i] / RG - (random(0, TargetRed[i] / 5 / RG)); //4
			TargetBlue[i] = TargetGreen[i] / GB - (random(0, TargetGreen[i] / 7 / GB)); //6
			if (TargetBlue[i] > 50)TargetBlue[i] = 0;
		}

		if (TargetRed[i] > kaars[i].r) {
			C_reg[i] |= (1 << 0);
			StepRed[i] = (TargetRed[i] - kaars[i].r) / speed[i];
			StepGreen[i] = (TargetGreen[i] - kaars[i].g) / speed[i];
			StepBlue[i] = (TargetBlue[i] - kaars[i].b) / speed[i];
		}
		else {
			C_reg[i] &= ~(1 << 0);
			StepRed[i] = (kaars[i].r - TargetRed[i]) / speed[i];
			StepGreen[i] = (kaars[i].g - TargetGreen[i]) / speed[i];
			StepBlue[i] = (kaars[i].b - TargetBlue[i]) / speed[i];
		}
	}

	if (bitRead(C_reg[i], 1) == true) {
		kaars[i] = 0xFFFFFF;
		speed[i] = 1;
	}


	else {
		if (bitRead(C_reg[i], 0) == true) {
			kaars[i].r = kaars[i].r + StepRed[i];
			kaars[i].g = kaars[i].g + StepGreen[i];
			kaars[i].b = kaars[i].b + StepBlue[i];
		}
		else {
			kaars[i].r = kaars[i].r - StepRed[i];
			kaars[i].g = kaars[i].g - StepGreen[i];
			kaars[i].b = kaars[i].b - StepBlue[i];
		}
	}

	speed[i]--;
	} //	for (byte i; i < 11; i++) {
	fastled;
}

void loop()
{
	if (millis() - tijd > 10) { //every 10 ms
		slowevents();
		tijd = millis();
		if (bitRead(COM_reg, 0) == true)FastLED.show();
		COM_reg &= ~(1 << 0);
	}
}
