/*
Script for creating PWM signal manual. 

This script first calculates the OCR value based on the desired frequency. Then the best prescaler is selected.

Jasper van Bemmel      28-03-2025
*/

float freq = 100; // Frequency in Hz
const long F_CLK = 16000000; // Clock frequency (16 MHz)

void setup() {
  Serial.begin(9600);

  // Pin 9 is the output
  pinMode(9, OUTPUT);


  int prescaler_array[] = {1, 8, 64, 256, 1024}; 
  int prescaler_select = 0;
  float minima = 100;
  int OCR = 0;
  
  
  // Calculate OCR value
for (int i = 0; i < 5; i++) {
  int OCR_loop = F_CLK / ((prescaler_array[i] * freq) - 1);  // calculate for every prescaler
  float f_print_loop = F_CLK / (prescaler_array[i] * ((float)OCR_loop+1)); // calculate frequency based on interger values OCR and prescaler
  float delta_f = freq-f_print_loop; // calculate error
  if (abs(delta_f) < minima){     // determine smalles error and select best prescaler
    minima = delta_f;
    prescaler_select = prescaler_array[i];
    OCR = OCR_loop;
  } 
}



  // Check if OCR value fits in 16-bit value
  if (OCR < 0 || OCR > 65535) {
    Serial.println("ERROR: OCR value out of range.");
    return;
  }

  // Set Timer1 Fast PWM-mode
  TCCR1A = _BV(WGM11) | _BV(COM1A1); // Fast PWM, clear OC1A on compare match


// Prescaler values
  if (prescaler_select == 1)   TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);
  if (prescaler_select == 8)   TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11);
  if (prescaler_select == 64)  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11) | _BV(CS10);
  if (prescaler_select == 256) TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS12);
  if (prescaler_select == 1024) TCCR1B = _BV(WGM13) | _BV(WGM12)| _BV(CS12) | _BV(CS10);

  // OCR value for timer1
  ICR1 = OCR;

  // Dutycycle of 50%
  OCR1A = OCR/2;
}

void loop() {
  delay(1000);  // delay
}
