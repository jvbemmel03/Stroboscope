#include <ADS1X15.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

ADS1115 ADS(0x48);
LiquidCrystal_I2C lcd(0x27,16,2);


float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}



void setup() 
{
  Serial.begin(115200);

lcd.init();                // initialize the lcd 
lcd.backlight();           // light the lcd


  Wire.begin();
  ADS.begin();
}


void loop() 
{
  ADS.setGain(0);

  int16_t val_0 = ADS.readADC(0); 

  float fpot = mapFloat(val_0, 200, 26500, 1,  100); // frequency stroboscope

  Serial.print("\tAnalog0: "); Serial.print(val_0); Serial.print('\t'); Serial.println(fpot);

  Serial.println();



  //LCD display
char buffer[7];

lcd.setCursor(0,0);
lcd.print("f(Hz)");

lcd.setCursor(0,1);


lcd.print(dtostrf(fpot,5,1, buffer));      
}

delay(100);
//  -- END OF FILE --

