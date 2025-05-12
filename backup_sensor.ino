// Sonar Parking Backup Sensor
// Created by: Ben Zacharias 2024
// Uses some modified code from: https://forum.arduino.cc/t/i-want-to-share-arduino-lcd-progress-bar-code/298348/2

#include <LiquidCrystal.h>
#include "Arduino.h"

byte percentage_1[8] = { B10000, B10000, B10000, B10000, B10000, B10000, B10000, B10000 };
byte percentage_2[8] = { B11000, B11000, B11000, B11000, B11000, B11000, B11000, B11000 };
byte percentage_3[8] = { B11100, B11100, B11100, B11100, B11100, B11100, B11100, B11100 };
byte percentage_4[8] = { B11110, B11110, B11110, B11110, B11110, B11110, B11110, B11110 };
byte percentage_5[8] = { B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111 }; 

byte caution[8] = {
  B00100,
  B01110,
  B01010,
  B11011,
  B11011,
  B11111,
  B11011,
  B11111
};

byte caution_left[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00001,
  B00011,
  B00011
};

byte caution_right[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B10000,
  B11000,
  B11000
};

class Ultrasonic {
	public:
		Ultrasonic(int pin);
        void DistanceMeasure(void);
		long microsecondsToCentimeters(void);
		long microsecondsToInches(void);
	private:
		int _pin;//pin number of Arduino that is connected with SIG pin of Ultrasonic Ranger.
        long duration;// the Pulse time received;
};

Ultrasonic::Ultrasonic(int pin) {
	_pin = pin;
}

/*Begin the detection and get the pulse back signal*/
void Ultrasonic::DistanceMeasure(void) {
    pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
	delayMicroseconds(2);
	digitalWrite(_pin, HIGH);
	delayMicroseconds(5);
	digitalWrite(_pin,LOW);
	pinMode(_pin,INPUT);
	duration = pulseIn(_pin,HIGH);
}

/*The measured distance from the range 0 to 400 Centimeters*/
long Ultrasonic::microsecondsToCentimeters(void) {
	return duration/29/2;	
}

/*The measured distance from the range 0 to 157 Inches*/
long Ultrasonic::microsecondsToInches(void) {
	return duration/74/2;	
}

Ultrasonic ultrasonic(7); // Use PIN 7

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

void setup() {
        Serial.begin(9600);
        lcd.begin(16, 2); // set up the LCD's number of columns and rows
        lcd.createChar(0, percentage_1);
        lcd.createChar(1, percentage_2);
        lcd.createChar(2, percentage_3);
        lcd.createChar(3, percentage_4);
        lcd.createChar(4, percentage_5);
        lcd.createChar(5, caution);
        lcd.createChar(6, caution_left);
        lcd.createChar(7, caution_right);
}

void symbol(int pos_x, int pos_y) { // code to draw the whole caution symbol(s)
  lcd.setCursor(pos_x, pos_y);
  lcd.write(byte(6));
  lcd.setCursor(pos_x + 1, pos_y);
  lcd.write(byte(5));
  lcd.setCursor(pos_x + 2, pos_y);
  lcd.write(byte(7));
}

void loop() {
	long RangeInInches;
	ultrasonic.DistanceMeasure();// get the current signal time;
        RangeInInches = ultrasonic.microsecondsToInches();//convert the time to inches;
        int feet = RangeInInches / 12;
        int remainder = RangeInInches % 12;
        int max_range = 4; // maximum range for the sensor to monitor, in feet.
        
        //Clear the display
        lcd.clear();
        
        if (RangeInInches == 0) {
            symbol(7, 0);
            lcd.setCursor(0, 1);
            lcd.print(" SENSOR BLOCKED");
            delay(500);
        } else {
          if (feet < max_range) {
              lcd_percentage(max_range, RangeInInches, 0, 16, 1);
          }
          symbol(0, 0);
          symbol(13, 0);
          
          lcd.setCursor(3, 0);
          lcd.print(feet);
      	  lcd.print(" ft, ");
          lcd.print(remainder);
          lcd.print(" in");
	  delay(150);
        }
}

void lcd_percentage(int max_range, int percentage, int cursor_x, int cursor_x_end, int cursor_y) {
  int calc = (percentage * cursor_x_end * 5 / (max_range * 12)) - (percentage * cursor_x * 5 / (max_range * 12));
  while(calc >= 5){
    lcd.setCursor(cursor_x,cursor_y);
    lcd.write((byte)4);
    calc-=5;
    cursor_x++;
  }
  while(calc >= 4 && calc < 5){
    lcd.setCursor(cursor_x,cursor_y);
    lcd.write((byte)3);
    calc-=4;
  }
  while(calc >= 3 && calc < 4){
    lcd.setCursor(cursor_x,cursor_y);
    lcd.write((byte)2);
    calc-=3;
  }
  while(calc >= 2 && calc < 3){
    lcd.setCursor(cursor_x,cursor_y);
    lcd.write((byte)1);
    calc-=2;
  }
  while(calc >= 1 && calc < 2){
    lcd.setCursor(cursor_x,cursor_y);
    lcd.write((byte)0);
    calc-=1;
  }
}
