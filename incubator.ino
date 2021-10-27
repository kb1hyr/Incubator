/** @file incubator.ino
 *  @brief Controls the heat for a microbiology incubator
 *
 *  This sketch is derived in part from ButtonTest.ino Copyright 2018 by Daniel Hooper
 *  and licensed under the LGPL and thus this retains that license.
 *
 *  @author    Brett Markham
 *  @copyright Copyright Brett Markham 2020
 *  @date      2020
 *  @license   LGPL
 *
 *  @bug No known bugs.
 */

#include <LiquidCrystal.h>

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
#define BACKLIGHT_PIN  10  ///< D10

#define aref_voltage 5.0 // Using 5.0V with TMP36 

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
float setTemp, sensedTemp;
float tempMin = 25.0;
float tempMax = 40.0;
bool heaterOn;
int tempPin = 1; // This is analog A1
int relayPin = 3; // this is digital 3
int lastButton = 1023;

// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 if (adc_key_in < 50)   return btnRIGHT; 
 if (adc_key_in < 195)  return btnUP;
 if (adc_key_in < 380)  return btnDOWN;
 if (adc_key_in < 555)  return btnLEFT;
 if (adc_key_in < 790)  return btnSELECT;  
 return btnNONE;  // when all others fail, return this...
}

/** Reads TMP36, returns temp in degrees C
* To account for fluctuations in reading, this reads 10x and averages it
*
* returns Degrees celsius as a float
*/
float get_temp(void) {
  int tempReading;
  float voltage;
  int counter;

  counter = 0;
  tempReading = 0;
  while (counter < 10)
  {
    tempReading = tempReading + analogRead(tempPin);
    counter++;
  }
  
  voltage = ((tempReading/10) * aref_voltage)/1024.0;
  return((voltage - 0.5)*100);
}

void heater() {
  if ((sensedTemp < (setTemp - 0.5)) && (heaterOn == false)) 
  {
    heaterOn = true;
    digitalWrite(relayPin, HIGH);
    lcd.setCursor(13,0);
    lcd.print(" ON");
    
  }
  if ((sensedTemp > (setTemp + 0.5)) && (heaterOn == true))
  {
    heaterOn = false;
    digitalWrite(relayPin, LOW);
    lcd.setCursor(13,0);
    lcd.print("OFF");
  }
}

void manage_display() {
  lcd.setCursor(5, 0);
  lcd.print(setTemp);
  lcd.setCursor(6, 1);
  lcd.print(sensedTemp);
}


void setup() {
  // put your setup code here, to run once:
//  analogReference(EXTERNAL); // because we are using the 3.3V reference
  pinMode(BACKLIGHT_PIN, OUTPUT);
  setTemp = 37; // Set default temp to 37C
  heaterOn = false; // Start with the heater off
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  
  lcd.begin(16,2);
  lcd.clear();
  digitalWrite(BACKLIGHT_PIN, HIGH);
  lcd.write("Incubator DOOM!");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0); // top Left
  lcd.print("Set: ");
  lcd.setCursor(11, 0);
  lcd.write("C");
  lcd.setCursor(0, 1);
  lcd.print("Read: ");
  lcd.setCursor(13, 1);
  lcd.write("C");
}

void loop() {
  // put your main code here, to run repeatedly:
  sensedTemp = get_temp();
  heater();
  manage_display();



  //

  lcd_key = read_LCD_buttons();
  switch (lcd_key)               // depending on which button was pushed, we perform an action
 {
   case btnRIGHT:
     {
     //lcd.print("RIGHT ");
     break;
     }
   case btnLEFT:
     {
     //lcd.print("LEFT   ");
     break;
     }
   case btnUP:
     {
      delay(200); //debounce
     if (setTemp < 40)
       {
         setTemp++;
         lcd.setCursor(5, 0);
         lcd.print(setTemp);
         lcd.setCursor(11, 0);
         lcd.write("C");
       }
     break;
     }
   case btnDOWN:
     {
      delay(200); // debounce
     if (setTemp > 25)
       {
         setTemp--;
         lcd.setCursor(5, 0);
         lcd.print(setTemp);
         lcd.setCursor(11, 0);
         lcd.write("C");
       }
     break;
     }
   case btnSELECT:
     {
     //lcd.print("SELECT");
     break;
     }
     case btnNONE:
     {
     //lcd.print("NONE  ");
     break;
     }
 }



}
