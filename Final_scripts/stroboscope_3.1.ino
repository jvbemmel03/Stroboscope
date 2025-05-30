/*
Script for stroboscoop driven by PWM signals Arduino. Two PWM signals are generated by the PWM libraries. Pin D9 generates a low frequency signal
for controlling the over all frequency of the stroboscope. Pin 3 controls the intensity of the stroboscope. This script contains two modes: manual and automatic.
By making use of the following components: ADS1115, LCD 16x2 I2C, potentiometers, encoder (EC11) and toggle switches.

connections:
toggle switch: D6
PWM frequency output: D9
PWM intensity output: D3
Encoder: D2 & D4
Switch Encoder: D5
SDA: A5
SCL: A4

Jasper van Bemmel 27-05-2025
*/

#include <TimerOne.h>
#include <PWM.h>
#include <ADS1X15.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>

// define pins and variabels
const int switchPin = 5;  // Switch between manual and auto mode
bool vorigeSwitchState = false;

// PWM settings
int PWM_9 = 9;
int PWM_3 = 3;
int frequency_dim = 1000;

// encoder
const int knopPin = 6;
long vorigeRuweWaarde = 0;
int vorigeKlikWaarde = 0;
float waarde = 0.0;       // for manual mode
float autoWaarde = 0.0;   // for auto mode

// LCD + ADS + encoder
LiquidCrystal_I2C lcd(0x27,16,2);
ADS1115 ADS(0x48);
Encoder mijnEncoder(2, 4);  // CLK on 2, DT on 4


float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup() {
  Serial.begin(9600);
  pinMode(switchPin, INPUT);
  pinMode(knopPin, INPUT);

  Timer1.initialize(0);
  InitTimersSafe();

  lcd.init();
  lcd.backlight();
  lcd.clear();
  Wire.begin();

  ADS.begin();
  ADS.setGain(0);

  delay(200);
}

void loop() {
  bool huidigeSwitchState = (digitalRead(switchPin) == LOW);  // LOW = manual

  if (huidigeSwitchState != vorigeSwitchState) {
    lcd.clear();
    vorigeKlikWaarde = 0;
    mijnEncoder.write(0);  // reset encoder position

    lcd.setCursor(6, 0); lcd.print("PW(%)");
    lcd.setCursor(12, 0); lcd.print("I(%)");
    lcd.setCursor(0, 0); lcd.print("f(Hz)");

    vorigeSwitchState = huidigeSwitchState;
  }

  // read and map values ADS
  int16_t val_1 = ADS.readADC(1);
  int16_t val_2 = ADS.readADC(2);
  int pwpot = map(val_1, 1000, 25000, 0,  1023);
  int Ipot = map(val_2, 1000, 25000, 0,  255);
  int pwpot_print = map(val_1, 1000, 25000, 0,  100);
  int Ipot_print = map(val_2, 1000, 25000, 0,  100);

  char buffer[8];

  // === Manual mode ===
  if (huidigeSwitchState) {
    long ruweWaarde = mijnEncoder.read();
    int klikWaarde = ruweWaarde / 4;

    if (klikWaarde != vorigeKlikWaarde) {
      int verschil = klikWaarde - vorigeKlikWaarde;
      vorigeKlikWaarde = klikWaarde;

      waarde += verschil * 0.1;

      // limits readings
      if (waarde > 100.0) waarde = 0.0;
      else if (waarde < 0.0) waarde = 100.0;

      Serial.print("f (Hz): ");
      Serial.println(waarde, 1);
    }

    if (digitalRead(knopPin) == LOW) {
      delay(50);
      if (digitalRead(knopPin) == LOW) {
        lcd.clear();
        lcd.setCursor(6, 0); lcd.print("PW(%)");
        lcd.setCursor(12, 0); lcd.print("I(%)");
        lcd.setCursor(0, 0); lcd.print("f(Hz)");
        waarde = 0.0;
        vorigeKlikWaarde = 0;
        mijnEncoder.write(0);
        while (digitalRead(knopPin) == LOW) delay(10);
      }
    }

    lcd.setCursor(7, 1);
    if (pwpot_print < 1) {
      pwpot = 0;
      lcd.print("---");
    } else if (pwpot_print > 100) {
      pwpot = 1023;
      lcd.print("100");
    } else {
      lcd.print(dtostrf(pwpot_print,3,0, buffer));
    }

    lcd.setCursor(12, 1);
    if (Ipot_print < 1) {
      Ipot = 0;
      lcd.print("---");
    } else if (Ipot_print > 100) {
      Ipot = 255;
      lcd.print("100");
    } else {
      lcd.print(dtostrf(Ipot_print,3,0, buffer));
    }

    lcd.setCursor(0, 1);
    lcd.print("     ");  // clear old data
    lcd.setCursor(0, 1);
    if (waarde < 0.1) Ipot = 0;
    lcd.print(dtostrf(waarde, 4, 1, buffer));

    // PWM settings
    Timer1.setPeriod(1000000 / waarde);
    Timer1.pwm(PWM_9, pwpot);
    SetPinFrequency(PWM_3, frequency_dim);
    pwmWrite(PWM_3, Ipot);
  }

  // === AUTO mode ===
  else {
    long ruweWaarde = mijnEncoder.read();
    int klikWaarde = ruweWaarde / 4;

    if (klikWaarde != vorigeKlikWaarde) {
      int verschil = klikWaarde - vorigeKlikWaarde;
      vorigeKlikWaarde = klikWaarde;

      autoWaarde += verschil * 0.1;
      autoWaarde = constrain(autoWaarde, 0.0, 5.0);

      Serial.print("Auto waarde: ");
      Serial.println(autoWaarde, 1);
    }

    lcd.setCursor(6, 0); lcd.print("PW(%)");
    lcd.setCursor(12, 0); lcd.print("I(%)");
    lcd.setCursor(0, 0); lcd.print("f(Hz)");

    lcd.setCursor(7, 1);
    if (pwpot_print < 1) {
      pwpot = 0;
      lcd.print("---");
    } else if (pwpot_print > 100) {
      pwpot = 1023;
      lcd.print("100");
    } else {
      lcd.print(dtostrf(pwpot_print,3,0, buffer));
    }

    lcd.setCursor(12, 1);
    if (Ipot_print < 1) {
      Ipot = 0;
      lcd.print("---");
    } else if (Ipot_print > 100) {
      Ipot = 255;
      lcd.print("100");
    } else {
      lcd.print(dtostrf(Ipot_print,3,0, buffer));
    }

    lcd.setCursor(1, 1);
    lcd.print("    ");
    lcd.setCursor(1, 1);
    lcd.print(dtostrf(autoWaarde, 3, 1, buffer));

    Timer1.pwm(PWM_9, 0);
    pwmWrite(PWM_3, 0);
  }

  delay(200);
}
