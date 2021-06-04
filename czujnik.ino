#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);	//LCD display with pins to which it's connected
unsigned int Pm25 = 0;	//variables which store measurements
unsigned int Pm10 = 0;

void Display() {	//displays measurements for both sensors to the console
	/*Serial.print("PM 2.5: ");	//version without LCD display
	Serial.println(Pm25/10.0);	//values get by GetData are 10 times higher
	Serial.print("PM 10: ");
	Serial.println(Pm10/10.0);*/
	lcd.setCursor(0, 0);	//version with LCD display
	lcd.print("PM2.5: ");
	lcd.print(Pm25/10.0);	//values get by GetData are 10 times higher
	lcd.setCursor(0, 1);
	lcd.print("PM10:  ");
	lcd.print(Pm10/10.0);
}

void GetData() { //reads data from sensors and processes it into readable state
	uint8_t mPkt[10] = {0};	//data from sensor; packet format: AA, C0, PM25_Low, PM25_High, PM10_Low, PM10_High, ID byte 1, ID byte 2, CRC, AB
	uint8_t mCheck = 0;	//CRC sum from received packet (PM25_Low, PM25_High, PM10_Low, PM10_High, ID byte 1, ID byte 2)
	while (Serial.available() > 0) {
		mPkt[0] = Serial.read();
		delay(2);	//wait until packet is received
		if (mPkt[0] == 0xAA) {	//checks if Message header is ok
			mPkt[0] =  mData;
			mPkt[1] = Serial.read();
			if (mPkt[1] == 0xC0) {	//checks if Commander No. is ok
				mCheck = 0;
				for (int i = 2; i < 8; i++) {	//reads data bytes
					mPkt[i] = Serial.read();
					delay(2);
					mCheck += mPkt[i];
				}
				mPkt[8] = Serial.read();
				delay(1);
				mPkt[9] = Serial.read();
				if (mCheck == mPkt[8]) {	//checks if CRC sum is ok
					Serial.flush();
					Pm25 = ((uint16_t)mPkt[2] | (uint16_t)(mPkt[3] << 8));	//using formula: (256 * higher byte + lower byte)
					Pm10 = ((uint16_t)mPkt[4] | (uint16_t)(mPkt[5] << 8));
					if (Pm25 > 9999) Pm25 = 9999;	//the highest obtainable value is 9999
					if (Pm10 > 9999) Pm10 = 9999;
				}
			}
		}
	}
}

void setup() {
	Serial.begin(9600);
	Pm25 = 0;
	Pm10 = 0;
	lcd.begin(16, 2);
	lcd.clear();
	Display();
}

void loop() {
	GetData();
	Display();
	delay(1000);
}
