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
boolean updown[10];


byte count[3];
byte COM_reg;


//tijdelijk
unsigned long tijd;
unsigned long periode;

void setup()
{
	Serial.begin(9600);
	FastLED.addLeds<NEOPIXEL, 8>(kaars, 10);
	

}

void burn() {

	count[0]++;
	if (count[0]==255)randomSeed(analogRead(0));

	if (speed[0] == 0) { //targetwaarde bereikt
		speed[0] = random(5, 50);

		if (bitRead(COM_reg, 1) == true){
			COM_reg &= ~(1 << 1);
			kaars[0] = 0xFF6000;
		}

		if (speed[0] ==5)COM_reg |= (1 << 1); //pixel full




		TargetRed[0] = random(150, 255);
		TargetGreen[0] = TargetRed[0] / 4 - (random(5, 20));
		TargetBlue[0] = TargetGreen[0] / 6 - (random(5, 20));
		if (TargetBlue[0] > 50)TargetBlue[0] = 0;

		if (TargetRed[0] > kaars[0].r) {
			updown[0] = true;
			StepRed[0] = (TargetRed[0] - kaars[0].r) / speed[0];
			StepGreen[0] = (TargetGreen[0] - kaars[0].g) / speed[0];
			StepBlue[0] = (TargetBlue[0] - kaars[0].b) / speed[0];
		}
		else {
			updown[0] = false;
			StepRed[0] = (kaars[0].r - TargetRed[0]) / speed[0];
			StepGreen[0] = (kaars[0].g - TargetGreen[0]) / speed[0];
			StepBlue[0] = (kaars[0].b - TargetBlue[0]) / speed[0];
		}
	}


	if (bitRead(COM_reg, 1) == true) {
		kaars[0] = 0xFFFFFF;
		//COM_reg &= ~(1 << 1);
	}
	else {



		if (updown[0] == true) {
			kaars[0].r = kaars[0].r + StepRed[0];
			kaars[0].g = kaars[0].g + StepGreen[0];
			kaars[0].b = kaars[0].b + StepBlue[0];
		}
		else {
			kaars[0].r = kaars[0].r - StepRed[0];
			kaars[0].g = kaars[0].g - StepGreen[0];
			kaars[0].b = kaars[0].b - StepBlue[0];
		}
	}
speed[0]--;


	fastled;
	
}

void loop()
{
	if (millis() - tijd > 10) {
		burn();
		tijd = millis();
		if (bitRead(COM_reg, 0) == true)FastLED.show();
		COM_reg &= ~(1 << 0);
	}


}
