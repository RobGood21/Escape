/*
	Name:       Blower.ino
	Created:	30-9-2019 13:00:20
	Author:     Rob Antonisse
	Sketch for Arduino Uno for candle blow puzzle
	10 candles, use of swiches or


*/

//for Fastled

#include <FastLED.h>
#define blowtime 1
#define timered 10000  //time a blownout candle stays out...10sec
#define reset 60000 //time puzzle resets when solved 60000
byte solution[4] ={ 1,3,8,10 };

float RG = 3; //relation between red and green
float GB = 5; //relation between green and blue

CRGB kaars[10];
CRGB bus[5];

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



unsigned long tijd;
unsigned long outtime[10];
unsigned long resettimer;

byte count[3];
byte countrandom;
byte switchcount;
byte speedcount;
byte switchstatus[2]; //0=port D 1=port C
byte COM_reg;

void setup()
{

	//Serial.begin(9600);

	DDRD |= (1 << 7);//portD pin 7  as outputs neo kaarsen
	DDRD |= (1 << 6); //pin6 neo brievenbus


	FastLED.addLeds<NEOPIXEL, 7>(kaars, 10);
	FastLED.addLeds<NEOPIXEL, 6>(bus, 5);

	PORTC = 0xFF; //set port C input with pull up.
	DDRB = 0x00;
	PORTB = 0xFF; //portB pull ups

	DDRD &= ~(1 << 5);
	PORTD |= (1 << 5); //PIN 5 input puzzle reset

	//mirror solution
	for (byte i = 0; i < 4; i++) {
		solution[i] = 11 - solution[i];
	}
}
void slowevents() {
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
		timeout(); //time red candle, check puzzle solved	
		if (bitRead(COM_reg, 3) == true)brievenbus(3);
		switchB();
		switchC();
		switchR();
	}
}
void switchR() {
	byte pp;
	pp = bitRead(PIND, 5);
	if (pp != bitRead(COM_reg, 4)) { //status switch changed
		if (pp == true) { //switch released
			COM_reg |= (1 << 4);
		}
		else { //switch pressed	
			COM_reg &= ~(1 << 4);
			COM_reg |= (1 << 1); //set puzzle soved status	
			resettimer = millis();
			brievenbus(1);
		}
	}
}
void switchB() {
	//reads portB
	byte changed;
	byte krs;
	//check changed switches
	changed = PINB ^ switchstatus[0];
	if (changed > 0) { //switch changed
		for (byte i = 0; i < 5; i++) { //check all switch states
			krs = 4 - i;
			if (bitRead(changed, i) == true) {

				if (bitRead(PINB, i) == true) { //switch released
					switchstatus[0] |= (1 << i);
					//blowcount[krs] = 0;
					//Serial.println(switchstatus[0]);
				}
				else { //switch pushed
					switchstatus[0] &= ~(1 << i);
					//test(krs);
					switched(krs); //button pushed
					blowcount[krs] = 0;
				}
			}
		}
	}
	//check switches hold
	for (byte i = 0; i < 5; i++) {
		krs = 4 - i;
		//krs = i;
		if (bitRead(PINB, i) == false & bitRead(switchstatus[0], i) == false) {
			blowcount[krs]++;
		}
		if (blowcount[krs] > blowtime)blowout(krs); //3 ? voor test ff 10
	}
}
void switchC() {
	//reads portC
	byte changed;
	byte krs;

	//check changed switches
	changed = PINC ^ switchstatus[1];
	if (changed > 0) { //switch changed
		for (byte i = 0; i < 5; i++) { //check all switch states
			krs = i + 5;
			if (bitRead(changed, i) == true) {
				if (bitRead(PINC, i) == true) { //switch released
					switchstatus[1] |= (1 << i);

					//Serial.println(switchstatus[1]);
					//blowcount[krs] = 0;
				}
				else { //switch pushed
					switchstatus[1] &= ~(1 << i);
					//test(krs);
					switched(krs); //button pushed
					blowcount[krs] = 0;
				}
			}
		}
	}

	//check switches hold
	for (byte i = 0; i < 5; i++) {
		/*
		if you make i<6 then krs can become 10 5+5 that will ruin the complete program because alle arrays have 10 positions.0~9
		*/
		krs = i + 5;
		if (bitRead(PINC, i) == false & bitRead(switchstatus[1], i) == false) {
			blowcount[krs]++;
		}
		if (blowcount[krs] > blowtime)blowout(krs); //3 ? voor test ff 10
	}
}
void test(byte k) {

	Serial.println(k);
	//test switches onoff candles 
	C_reg[k] ^= (1 << 6);

	if (bitRead(C_reg[k], 6) == true) {
		kaars[k] = 0xAAAAAA;
	}
	else {
		kaars[k] = 0x000000;
	}
	//Serial.println(kaars[k]);
		//FastLED.show();
}

void switched(byte blow) {
	//Serial.println(blow);	

	C_reg[blow] |= (1 << 2); //flickering onoff

	//Serial.println("");
	//Serial.print("start in switched:");
	//Serial.println(blow);
	fcount[blow] = random(30, 60);
}

void blowout(byte krs) { //called from slowevents

	C_reg[krs] |= (1 << 4);
	C_reg[krs] &= ~(B00000110 << 1); //reset efx

	//PINB |= (1 << 5);
	blowcount[krs] = 0;
	speed[krs] = 0; //stop running proces
	kaars[krs] = 0x200000;
	outtime[krs] = millis();
}
void brievenbus(byte onoff) {

	switch (onoff) {
	case 0:
		COM_reg &= ~(1 << 2);
		COM_reg |= (1 << 3);
		//fill_solid(bus, 5, CRGB::Black);
		break;
	case 1:
		//fill_solid(bus, 5, CRGB::NavajoWhite);
		COM_reg |= (1 << 2);
		COM_reg |= (1 << 3);
		break;
	case 3:
		for (byte bb = 0; bb < 5; bb++) {
			if (bitRead(COM_reg, 2) == true) {
				bus[bb].r++;
				if (bus[bb].r > 200)COM_reg &= ~(1 << 3);

				bus[bb].g++;
				bus[bb].b++;
			}
			else {
				bus[bb].r--;
				if (bus[bb].r == 0)COM_reg &= ~(1 << 3);
				if (bus[bb].g > 0)bus[bb].g--;
				if (bus[bb].b > 0)bus[bb].b--;
			}
		}
		break;
	}
}


void timeout() { //called from slowevents
	byte result = 0; //resultold
	byte rc = 0; //redcount

	//check for puzzle is solved
	//solution[4]
	if (bitRead(COM_reg, 1) == false) {
		for (byte i = 0; i < 4; i++) {
			if (bitRead(C_reg[solution[i] - 1], 4) == true)result++;
		}

		for (byte i = 0; i < 10; i++) {
			if (bitRead(C_reg[i], 4) == true)rc++;
		}


		if (result == 4 & rc == 4) { //puzzle solved
			COM_reg |= (1 << 1);
			brievenbus(1);
			resettimer = millis();
		}

		//checks time candle is been blown out	   	 
		for (byte k; k < 10; k++) {
			if (bitRead(C_reg[k], 4) == true) {
				if (millis() - outtime[k] > timered) {
					C_reg[k] &= ~(1 << 4); //rest candle
				}
			}
		}

	}
	else { //check for reset puzzle
		if (millis() - resettimer > reset) {
			COM_reg &= ~(1 << 1); //reset puzzle
			for (byte i = 0; i < 10; i++) {
				C_reg[i] = 0;
				brievenbus(0);
			}
		}
	}
}
//void solved() {

//}

void burn() {
	//byte i = 0; //temp
	//countrandom++;
	//if (countrandom == 255)randomSeed(analogRead(0));

	for (byte i = 9; i < 255; i--) {
		if (speed[i] == 0) { //targetwaarde bereikt
			speed[i] = random(20, 100);
			/**/

			if (bitRead(C_reg[i], 1) == true) { //stop flash
				C_reg[i] &= ~(1 << 1);
				kaars[i] = 0xAA3000;
			}

			//efx
			if (speed[i] == 22 & bitRead(C_reg[i], 4) == false) C_reg[i] |= (1 << 1); //pixel full, not if blowout is active

			if (speed[i] == 40 & bitRead(C_reg[i], 4) == false) { //random flicker effect, not in blowout (speed 40)
				C_reg[i] ^= (1 << 5);

				if (bitRead(C_reg[i], 5) == true) {
					//Serial.println("");
					//Serial.print("start in burn:");
					//Serial.println(i);

					C_reg[i] |= (1 << 2);
					fcount[i] = random(5, 50);
				}
			}


			if (bitRead(C_reg[i], 2) == true) { //flickering moet zijn 2			
				speed[i] = speed[i] / 3; //speed up effect /2
				C_reg[i] ^= (1 << 3); //toggle direction in flicker
				//Serial.print("*");
				//Serial.print(i);

				if (bitRead(C_reg[i], 3) == true) { //up
					TargetRed[i] = random(160, 250);
				}
				else { //down
					TargetRed[i] = random(60, 110);
				}

				fcount[i]--; //fcount random set when set bit2 in C_reg
				if (fcount[i] == 0) {
					C_reg[i] &= ~(1 << 2); //stop flickering effect				

					//Serial.println("");
					//Serial.print("Stop:");
					//Serial.println(i);
				}
			}

			else { //slow burn
				TargetRed[i] = random(160, 250);
			}
			//***********************
			if (bitRead(C_reg[i], 4) == true) { //blow out
				TargetRed[i] = random(15, 80);
				TargetGreen[i] = 4;
				TargetBlue[i] = 1;
			}
			else { //burn		
				TargetGreen[i] = (TargetRed[i] / RG) -5;
				TargetBlue[i] = (TargetGreen[i] / GB)-10;
				if (TargetBlue[i] > 10)TargetBlue[i] = 0;
			}

			if (TargetRed[i] >= kaars[i].r) {
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

		if (bitRead(C_reg[i], 1) == true & speed[i] > 5) {
			kaars[i] = 0xFFFFFF;
			speed[i] = 5;
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
	FastLED.show();
}

void loop()
{
	if (millis() - tijd > 5) { //every 5 ms
		slowevents();
		tijd = millis();
		//if (bitRead(COM_reg, 0) == true)FastLED.show();
		COM_reg &= ~(1 << 0);
		//FastLED.show();
	}
}
