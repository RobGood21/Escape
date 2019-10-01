/*
	Name:       Blower.ino
	Created:	30-9-2019 13:00:20
	Author:     Rob Antonisse
	Sketch for Arduino Uno for candle blow puzzle


*/

//for Fastled
#include <FastLED.h>
#define fastled COM_reg |=(1<<0);

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

float RG = 3.2; //relation between red and green
float GB = 6.5; //relation between green and blue


byte count[3];
byte switchcount;
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
	DDRB = 0xFF; //portB as outputs


}
void slowevents() {
	//switches


	byte changed;
	switchcount++;

	if (switchcount == 10) {
		switchcount = 0;

		changed = PIND ^ switchstatus[0];

		if (changed > 0) { //switch changed
			for (byte i; i < 8; i++) { //check all switch states
				if (bitRead(changed, i) == true) {

					if (bitRead(PIND, i) == true) {
						switchstatus[0] |= (1 << i);

					}
					else {
						switchstatus[0] &= ~(1 << i);
						switched(i); //button pushed
					}

				}

			}
		}
	}

}

void switched(byte sw) {
	switch (sw) {
	case 7:
		PINB |= (1 << 5);
		C_reg[0] ^= (1 << 2); //flickering onoff
		fcount[0] = random(4, 60);
		break;
	}
}


void burn() {

	byte i = 0; //temp

	count[i]++;
	if (count[i] == 255)randomSeed(analogRead(0));

	if (speed[i] == 0) { //targetwaarde bereikt
		speed[i] = random(5, 70);

		if (bitRead(C_reg[i], 1) == true) {
			C_reg[i] &= ~(1 << 1);
			kaars[i] = 0xFF6000;
		}
		if (speed[i] == 5)C_reg[i] |= (1 << 1); //pixel full

		if (speed[i] == 40) { //random flicker effect
			PINB |= (1 << 5);
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
				PINB |= (1 << 5);
			}

		}
		else { //slow burn
			TargetRed[i] = random(150, 255);
		}

		TargetGreen[i] = TargetRed[i] / RG - (random(0, TargetRed[i] / 5 / RG)); //4
		TargetBlue[i] = TargetGreen[i] / GB - (random(0, TargetGreen[i] / 7 / GB)); //6
		if (TargetBlue[i] > 50)TargetBlue[i] = 0;

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


	fastled;
}

void loop()
{
	if (millis() - tijd > 10) {
		burn();
		slowevents();
		tijd = millis();
		if (bitRead(COM_reg, 0) == true)FastLED.show();
		COM_reg &= ~(1 << 0);
	}


}
