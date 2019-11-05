/*
 Name:		Projector.ino
 Created:	November 2019
 Author:	Rob Antonisse
 Compagny:  Escape world group bv.
*/
//defines sterktes ledlicht in lens
#define Lil 350; //sterkte ledlicht in lens
#define BeepOff PORTD |= (1 << 6);

//declarations

#include <EEPROM.h>
unsigned long pulstijd;
unsigned long pwmtijd;
unsigned int pulsduur;
unsigned long filmtijd;
unsigned long uittijd;
unsigned long swtijd;
unsigned long animatietijd; //duur van de opstart animatie
unsigned long relaistijd;
byte movie;


byte soundmode;
unsigned long soundfreq;
unsigned long soundduur;
unsigned int soundtone;
byte soundcount;
byte COM_reg;
byte MEM_reg;
byte SW_status;
int countfade;
unsigned int pwmmax;

void setup() {
	Serial.begin(9600);

	SW_status = 0xFF;
	DDRD |= (1 << 7); //PIN 7 output
	pulsduur = 0; //10% duty cycle
	DDRD |= (1 << 6);//pin 6 output vibration

	//PORTD |= (1 << 6); //beep off
	BeepOff;

	//COM_reg |= (1 << 1); //film draaien
	//DDRC = 0x00;
	PORTC = 0xFF; //pullups to portC
	DDRB = 0xFF; //port B as outputs
	PORTB |= (1 << 2);
	PORTB |= (1 << 3);
	MEM_reg = EEPROM.read(100);
}
void lens() {

	if (millis() - filmtijd > 65 & bitRead(COM_reg, 1) == true) { //film draait
		filmtijd = millis();
		COM_reg |= (1 << 0);
		uittijd = millis();
		PORTD &= ~(1 << 7);
		PORTD |= (1 << 6);
	}

	if (bitRead(COM_reg, 0) == true) {
		if (millis() - uittijd > 20) {
			COM_reg &= ~(1 << 0);
			PORTD &= ~(1 << 6);
		}
	}
	else { //ledlens brand
		if (micros() - pwmtijd > pulsduur & bitRead(GPIOR0, 0) == true) {
			PORTD &= ~(1 << 7); //led off
			GPIOR0 &= ~(1 << 0);

			countfade++;
			if (countfade > 3) {
				countfade = 0;
				if (pulsduur < pwmmax) {
					pulsduur++; //fade in led lens 	
				}
				else {
					pulsduur--;
					if (pulsduur < 2) COM_reg &= ~(1 << 2);

				}
			}
		}

		if (millis() - pulstijd > 1) {
			pulstijd = millis();
			PORTD |= (1 << 7); //led on
			GPIOR0 |= (1 << 0);
			pwmtijd = micros(); //set off tijd
		}
	}
}

void lensuit() { //zet ledlens weer uit
	pwmmax = 1;
}

void setprograms() {
	PORTB &= ~(1 << 0); PORTB &= ~(1 << 1);
	if (bitRead(MEM_reg, 0) == true)PORTB |= (1 << 0);
	if (bitRead(MEM_reg, 1) == true)PORTB |= (1 << 1);
}


void SW_exe() {
	byte changed;
	byte port;
	if (millis() - swtijd > 100) { //very slow switches counters contact denderen
		BeepOff; //switch off beep to prevent hanging in active state
		swtijd = millis();
		port = PINC;
		changed = port ^ SW_status;

		if (changed > 0) { //switch status changed
			for (byte i = 0; i < 5; i++) {
				if (bitRead(changed, i) == true) {
					if (bitRead(port, i) == false) { //switch pressed
						SW_on(i);
					}
					else { //switch released
						SW_off(i);
					}
				}
			}
		}
		SW_status = port;

		if (bitRead(GPIOR0, 3) == true & millis() - relaistijd > 1000) {
			PORTB |= (1 << movie); //reset relais
			GPIOR0 &= ~(1 << 3); //free playbuttons
		}

		if (bitRead(COM_reg, 1) == true & millis() - animatietijd > 5000) {
			COM_reg &= ~(1 << 1); //free animation
			//pwmmax = 100; //fade out leds
		}
	}
}

void SW_on(byte sw) {
	//PINB |= (1 << sw);

	if (bitRead(COM_reg, 3) == false) { //program mode off

		switch (sw) {
		case 0: //spoel 1 geplaatst

			if(bitRead(MEM_reg, 0)== true) {
				COM_reg |= (1 << 2); //lensled aan
				pwmmax = Lil;
				soundmode = 1;
				soundduur = millis();
			}

			GPIOR0 |= (1 << 1);//spoel 1 geplaatst
			PORTB |= (1 << 0);
			break;

		case 1: //spoel 2 geplaatst
			if (bitRead(MEM_reg, 0) == true) {
				COM_reg |= (1 << 2); //lensled aan
				pwmmax = Lil;
				soundmode = 1;
				soundduur = millis();
			}
			GPIOR0 |= (1 << 2);//spoel 1 geplaatst
			PORTB |= (1 << 1);
			break;
		case 2:
			if (bitRead(GPIOR0, 1) == true & bitRead(GPIOR0, 3) == false) {
				GPIOR0 |= (1 << 3);//block new request for play
				movie = 2; //=movie 1
				startmovie();
			}
			break;
		case 3:
			if (bitRead(GPIOR0, 2) == true & bitRead(GPIOR0, 3) == false) {
				GPIOR0 |= (1 << 3);//block new request for play
				movie = 3; //=movie 2
				startmovie();
			}
			break;
		case 4: //program switch
			soundmode = 3;
			soundduur = millis();
			soundtone = 100;
			COM_reg |= (1 << 3); //program mode on
			setprograms();
			GPIOR0 &= ~(1 << 1); //clear filmspoel geplaatst
			GPIOR0 &= ~(1 << 2);
			break;
		}

	}
	else { //program mode on
		switch (sw) {
		case 2: //switch 1
			MEM_reg ^= (1 << 0);
			setprograms();
			break;
		case 3: //switch 2
			MEM_reg ^= (1 << 1);
			setprograms();
			break;
		}
	}
}

void SW_off(byte sw) {

	if (bitRead(COM_reg, 3) == false) { //program mode off

		switch (sw) {
		case 0:

			if(bitRead(MEM_reg,0)==true) soundmode = 2;
			soundduur = millis();
			
			GPIOR0 &= ~(1 << 1); //spoel niet geplaatst
			PORTB &= ~(1 << 0);
			if (bitRead(GPIOR0, 2) == false)lensuit();
			break;
		case 1:
			
			if(bitRead(MEM_reg,0)==true) soundmode = 2;
			soundduur = millis();
			
			GPIOR0 &= ~(1 << 2); //spoel niet geplaatst
			PORTB &= ~(1 << 1);
			if (bitRead(GPIOR0, 1) == false)lensuit();
			break;
		}
	}
	else { //program mode on

		if (sw == 4) {
			soundmode = 4;
			soundduur = millis();
			soundtone = 500;
			COM_reg &= ~(1 << 3); //program mode off
			EEPROM.update(100, MEM_reg);
			PORTB &= ~(3 << 0); //switch off leds in switches
			SW_status = 0xFF; //reset switch status
		}
	}
}

void startmovie() {
	//Serial.println(movie);

	//animatie
	if (bitRead(MEM_reg, 1) == true) {	
	COM_reg |= (1 << 2);
	animatietijd = millis();
	GPIOR0 |= (1 << 4); //animation plays
	COM_reg |= (1 << 1); //start animation
	pwmmax = 500; //fade out leds
	}

	relaistijd = millis();
	GPIOR0 |= (1 << 3); //movie plays	
	PORTB &= ~(1 << movie);
	

}


void sound() {
	switch (soundmode) {
	case 1: //spoel 1 geplaatst
		if (micros() - soundfreq > 200) {
			soundfreq = micros();
			PIND |= (1 << 6);
		}
		if (millis() - soundduur > 30) {
			soundmode = 0;
			//PORTD |= (1 << 6); //beep off
			BeepOff;	
		}
		break;

	case 2: //spoel 1 verwijderd	
		if (micros() - soundfreq > 2000) {
			soundfreq = micros();
			PIND |= (1 << 6);
		}
		if (millis() - soundduur > 30) {
			soundmode = 0;
			//PORTD |= (1 << 6); //beep off
			BeepOff;
		}
		break;
	case 3:
		if (micros() - soundfreq > soundtone) {
			soundcount++;
			if (soundcount == 100) {
				soundtone++;
				soundcount = 0;
			}
			soundfreq = micros();
			PIND |= (1 << 6);
		}
		if (millis() - soundduur > 300) {
			soundmode = 0;
			//PORTD |= (1 << 6); //beep off
			BeepOff;
		}
		break;

	case 4:
		if (micros() - soundfreq > soundtone) {
			soundcount++;
			if (soundcount == 5) {
				soundtone--;
				soundcount = 0;
			}

			soundfreq = micros();
			PIND |= (1 << 6);
		}
		if (millis() - soundduur > 300) {
			soundmode = 0;
			//PORTD |= (1 << 6); //beep off
			BeepOff;
		}
		break;
	}
}

void loop() {

	if (soundmode > 0) sound();
	if (bitRead(COM_reg, 2) == true) lens();
	SW_exe();

}
