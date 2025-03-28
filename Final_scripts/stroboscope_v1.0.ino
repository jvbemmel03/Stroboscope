/*

Arduino script which includes: 
 - 2 PWM signals (2 transistors)
 - LCD display I2C which displays the frequency, pulse width and the intensity
 - 3 analog readouts of potentiometers for frequency (0-100 Hz), pulse width (0-100 %) and intensity (0-100 %)

Jasper van Bemmel 19-03-2025

*/


//Include libraries
#include <PWM.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//Pin selection
int led = 9;                // the pin that the LED is attached to
int dim = 3;                // the pin that dimms the LED

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display



//Manual settings when analog inputs are disconnected
int dutycycle_1 = 50;         //pulsewidth in percentage
int frequency_strob = 20;     //frequency stroboscope
int frequency_dim = 1000;     //frequency of dimming LED (in Hz)
int dutycycle_2 = 70;  	      //brightness of led in percentage


//Map dutycycles to valules between 0 and 255 (8-bits)
int pulsewidth = map(dutycycle_1, 0, 100, 0, 255);
int brightness = map(dutycycle_2, 0, 100, 0, 255);

void setup()
{

    
  InitTimersSafe();          // initialize all timers except for 0, to save time keeping functions

  lcd.init();                // initialize the lcd 
  lcd.backlight();           // light the lcd

  lcd.setCursor(0,0);       // set cursor to 0,0
  lcd.print("INITIALIZING");
  lcd.setCursor(0,1);
  lcd.print("SYSTEM...");
  delay(1000);
  lcd.clear();              // clear lcd

}


void loop()
{




// reading and scaling analog ports (10-bit)
int fpot = map(analogRead(A0), 0, 1023, 0,  101); // frequency stroboscope
int pwpot = map(analogRead(A1), 0, 1023, 0,  256);// pulse width
int Ipot = map(analogRead(A2), 0, 1023, 0,  256); // Intensity

int pwpot_print = map(analogRead(A1), 0, 1023, 0,  101);// pulse width
int Ipot_print = map(analogRead(A2), 0, 1023, 0,  101); // Intensity

if (fpot == 0) Ipot = 0; 


// frequency and pulsewidth stroboscope (change fpot to frequency_strobo and pwpot to pulsewidth when analog readouts are unused)
SetPinFrequency(led, fpot);
pwmWrite(led, pwpot);

// frequency and brighness of stroboscope (change Ipot to brighness when analog readouts are unused)
SetPinFrequency(dim, frequency_dim);
pwmWrite(dim, Ipot);






//LCD display

lcd.setCursor(0,0);
lcd.print("f(Hz)");

lcd.setCursor(1,1);

if (analogRead(A0)<=2) fpot = 0;
if (analogRead(A0)>=1022) fpot = 100;

lcd.print(fpot);
                                                                                        
  if (fpot < 10) lcd.setCursor(2,1); lcd.print("  "); //refresh lcd screen when values drops to values lower then 10.        

lcd.setCursor(6,0);
lcd.print("PW(%)");

lcd.setCursor(7,1);

if (analogRead(A1)<=2) pwpot_print = 0;
if (analogRead(A1)>=1022) pwpot_print = 100;

lcd.print(pwpot_print);


  if (pwpot_print < 10) lcd.setCursor(8,1); lcd.print("  ");


lcd.setCursor(12,0);
lcd.print("I(%)");

lcd.setCursor(13,1);

if (analogRead(A2)<=2) Ipot_print = 0;
if (analogRead(A2)>=1022) Ipot_print = 100;

lcd.print(Ipot_print);
  
  if (Ipot_print < 10) lcd.setCursor(14,1); lcd.print("  ");


  

delay(10);



}

