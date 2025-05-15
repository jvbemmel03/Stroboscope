/*
Simple script for PWM sending to pin 9 Arduino

change parameter f and dc

Jasper van Bemmel     28-03-2025
*/



#include <TimerOne.h>

float f = 88.5; //frequency in Hz
int dc = 50;    //dutycycle 0-100%
int dc_scaled = map(dc, 0, 100, 0, 1023);
int led = 9;



void setup() {
Timer1.initialize(1000000); //The led will blink in a half second time interval
}


void loop() {

// send frequency and dutycycle to timer
Timer1.setPeriod(1000000 / f);
Timer1.pwm(led, dc_scaled);

}
