/*

Ajustments: 
v1.1 using TimerOne library in stead of PWM for timer 1
v1.2 simplify script

Arduino script which includes: 
 - 2 PWM signals (2 transistors)
 - 2 optocouplers to split the electric circuit to protect the Arduino board and components
 - LCD display I2C which displays the frequency, pulse width and the intensity
 - 3 Analog readouts of potentiometers for frequency (0-100 Hz), pulse width (0-100 %) and intensity (0-100 %)

Jasper van Bemmel 03-04-2025

*/

//Include libraries
#include <TimerOne.h>
#include <PWM.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ADS1X15.h>

//Pin selection
const int PWM_9 = 9;                // PWM signal (low) frequency
const int PWM_3 = 3;                // PWM signal (high) frequency
const int switchPin = 4;            // switch between auto en manual mode


LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
ADS1115 ADS(0x48);


int frequency_dim = 1000;     //frequency of dimming LED (in Hz)

//Define function for mapping floats
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;


}



void setup()
{


pinMode(switchPin, INPUT); // set pinmode to input

  Timer1.initialize(0);      // Initialize timer 1 (lib: TimerOne.h)
  InitTimersSafe();          // Initialize all timers except for 0, to save time keeping functions (lib: PWM.h)

  lcd.init();                // Initialize the lcd 
  lcd.backlight();           // Light the lcd
  Wire.begin();
  ADS.begin();

  lcd.setCursor(0,0);       // set cursor to 0,0
  lcd.print("INITIALIZING");
  lcd.setCursor(0,1);
  lcd.print("SYSTEM...");
  delay(1000);
  lcd.clear();              // clear lcd


}




void loop()
{



//reading potentiometer and encoder



ADS.setGain(0);

int16_t val_0 = ADS.readADC(0);
int16_t val_1 = ADS.readADC(1);



int pwpot = map(val_0, 1000, 25000, 0,  1023);// pulse width
int Ipot = map(val_1, 1000, 25000, 0,  255);// pulse width

int pwpot_print = map(val_0, 1000, 25000, 0,  100);// pulse width
int Ipot_print = map(val_1, 1000, 25000, 0,  100);// pulse width



//LCD display
char buffer[7];


lcd.setCursor(6,0);
lcd.print("PW(%)");
lcd.setCursor(7,1);

if (pwpot_print<1){
pwpot = 0;
lcd.print("---");
}
else if (pwpot_print>100){
  pwpot = 1023;
pwpot_print = 100;
lcd.print(dtostrf(pwpot_print,3,0, buffer));
}
else {lcd.print(dtostrf(pwpot_print,3,0, buffer));}


lcd.setCursor(12,0);
lcd.print("I(%)");
lcd.setCursor(12,1);

if (Ipot_print<1){
  Ipot =0;
  lcd.print("---");

}
else if (Ipot_print>100){
  Ipot = 255;
  Ipot_print = 100;
lcd.print(dtostrf(Ipot_print,3,0, buffer));
}
else{lcd.print(dtostrf(Ipot_print,3,0, buffer));}




if (digitalRead(switchPin) == LOW) { // Laag betekent dat de schakelaar aan staat
        switchState = true;


int fpot = map(val_0, 1000, 25000, 1,  100);

lcd.setCursor(0,0);
lcd.print("f(Hz)");
lcd.setCursor(1,1);

if (fpot < 1){
  Ipot = 0;
  lcd.print("---");

}
else if (fpot>100){
  fpot=100;
  lcd.print(dtostrf(fpot,3,0, buffer));
}
else lcd.print(dtostrf(fpot,3,0, buffer));                                                                                       






// frequency and pulsewidth stroboscope (change fpot to frequency_strobo and pwpot to pulsewidth when analog readouts are unused)
Timer1.setPeriod(1000000 / fpot);
Timer1.pwm(PWM_9, pwpot);

// frequency and brighness of stroboscope (change Ipot to brighness when analog readouts are unused)
SetPinFrequency(PWM_3, frequency_dim);
pwmWrite(PWM_3, Ipot);


delay(100);

}
























}


