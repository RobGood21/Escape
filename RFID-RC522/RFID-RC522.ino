/*
 Name:		RFID_RC522.ino
 Created:	12/20/2019 11:07:40 AM
 Author:	rob Antonisse
 Basis gebruik van RFID card alleen als toegang, identificatie. Storage op de card en verdere trucken niet bekeken.
 Gebruikt MFRC522 library BY COOQROBOT
 Als basis example: READNUID
*/

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10  // slave select pin SPI
#define RST_PIN 9 //reset pin SPI

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

///***** alleen teksten, soort kaart weergave
//MFRC522::MIFARE_Key key; //defineer een key
///alleen voor teksten

// Init array that will store new NUID 
byte nuidPICC[4];

byte akkoord[4] = { 0xD9, 0x8F, 0x27, 0xA3 };

unsigned long periode;
void setup() {
	Serial.begin(9600);
	SPI.begin(); // Init SPI bus
	rfid.PCD_Init(); // Init MFRC522 


	/*  waar is dit voor, volgens mij alleen teksten
	for (byte i = 0; i < 6; i++) {
		key.keyByte[i] = 0xFF;
	}

	Serial.println(F("This code scan the MIFARE Classsic NUID."));
	Serial.print(F("Using the following key:"));
	printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
*/
}

void loop() {
	if (millis() - periode > 100) { //make periodic read of cards
		periode = millis();

		// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.			
		if (!rfid.PICC_IsNewCardPresent()) return;   //rfid.PICC_IsNewCardPresent(); //nodig wakes up the card
	
		// Verify if the NUID has been readed
		if (!rfid.PICC_ReadCardSerial())return;  //rfid.PICC_ReadCardSerial(); //ook nodig, als true dan is UID en SAK gelezen

		/*
		/////alleen voor tekst
		Serial.print(F("PICC type: "));			   
		MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
		Serial.println(rfid.PICC_GetTypeName(piccType));

		// Check is the PICC of Classic MIFARE type
		if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
			piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
			piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
			Serial.println(F("Your tag is not of type MIFARE Classic."));
			return;
		}
		/////alleen voor tekst
		*/



		//****Zorg dat alleen een nieuwe card wordt gelezen

		//if (rfid.uid.uidByte[0] != nuidPICC[0] ||
			//rfid.uid.uidByte[1] != nuidPICC[1] ||
			//rfid.uid.uidByte[2] != nuidPICC[2] ||
			//rfid.uid.uidByte[3] != nuidPICC[3]) {
			//Serial.println(F("A new card has been detected."));
			// Store NUID into nuidPICC array
		//for (byte i = 0; i < 4; i++) {
		//	nuidPICC[i] = rfid.uid.uidByte[i];
		//}


		Serial.println(F("The NUID tag is:"));
		Serial.print(F("In hex: "));
		printHex(rfid.uid.uidByte, rfid.uid.size);
		Serial.println();
		Serial.print(F("In dec: "));
		printDec(rfid.uid.uidByte, rfid.uid.size);
		Serial.println();
		//}
		//else Serial.println(F("Card read previously."));

		// Halt PICC
		rfid.PICC_HaltA(); //set de card is pauze, nu wordt niet telkens deze card gelezen, pas als de card
		//stroomloos is geweest wordt het opnieuw herkent.

		// Stop encryption on PCD
		//rfid.PCD_StopCrypto1();
	}
}




 //Helper routine to dump a byte array as hex values to Serial.

void printHex(byte *buffer, byte bufferSize) {
	for (byte i = 0; i < bufferSize; i++) {
		Serial.print(buffer[i] < 0x10 ? " 0" : " ");
		Serial.print(buffer[i], BIN);
	}
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
	for (byte i = 0; i < bufferSize; i++) {
		Serial.print(buffer[i] < 0x10 ? " 0" : " ");
		Serial.print(buffer[i], DEC);
	}


}