#include <Wire.h>
#include <ADS1X15.h>        // Bibliotheek voor de ADS1115
#include <arduinoFFT.h>      // Bibliotheek voor ArduinoFFT van Enrique Condes

#define NUM_SAMPLES 128      // Aantal samples voor de FFT (moet een macht van 2 zijn)
#define SAMPLING_FREQUENCY 860  // Sampling frequentie in Hz (max van de ADS1115)

// Maak een object aan voor de ADS1115
ADS1115 Aads(0x48);

// Maak een FFT object aan
ArduinoFFT FFT = ArduinoFFT();

// Array voor het opslaan van de samples
double vReal[NUM_SAMPLES];
double vImag[NUM_SAMPLES];

// Variabelen voor het meten van de frequentie
unsigned long sampling_period_us;
unsigned long microseconds;

void setup() {
  Serial.begin(115200);       // Start seriële communicatie
  ads.setGain(ADS1115_REG_CONFIG_PGA_1);  // Stel de versterking in op 1x
  ads.begin();                // Begin de ADS1115
  sampling_period_us = 1000000 / SAMPLING_FREQUENCY;  // Bereken de sampling period (in microseconden)
}

void loop() {
  // Lees de samples van het analoge signaal
  for (int i = 0; i < NUM_SAMPLES; i++) {
    microseconds = micros();  // Start tijd bijhouden
    vReal[i] = ads.readADC_Differential_0_1();  // Lees een waarde van de ADS1115 in differentiële modus
    vImag[i] = 0;  // Zet de imaginaire delen op 0 (om een reële FFT te doen)
    
    // Wacht de juiste tijd om het volgende sample te nemen
    while (micros() - microseconds < sampling_period_us) {
      // Wacht totdat de sampling periode is verstreken
    }
  }

  // Voer de FFT uit op de verzamelde data
  FFT.Windowing(vReal, NUM_SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  // Pas venster toe op de data
  FFT.Compute(vReal, vImag, NUM_SAMPLES, FFT_FORWARD);  // Bereken de FFT
  FFT.ComplexToMagnitude(vReal, vImag, NUM_SAMPLES);  // Zet de complexe FFT naar magnitude

  // Zoek de piekfrequentie (de frequentie van het sterkste signaal)
  double peak = FFT.MajorPeak(vReal, NUM_SAMPLES);  // Bereken de frequentie van de piek

  // Print de frequentie van de piek naar de seriële monitor
  Serial.print("Piek frequentie: ");
  Serial.print(peak, 6);  // Frequentie met 6 decimalen nauwkeurigheid
  Serial.println(" Hz");

  delay(100);  // Wacht een beetje voordat we de volgende meting doen
}
