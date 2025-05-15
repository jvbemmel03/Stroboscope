#include <Encoder.h>

Encoder mijnEncoder(2, 4);     // CLK op pin 2, DT op pin 3
const int knopPin = 5;         // Schakelaar aangesloten op pin 4

long vorigeRuweWaarde = 0;
int vorigeKlikWaarde = 0;
float waarde = 0.0;

void setup() {
  Serial.begin(9600);
  pinMode(knopPin, INPUT);  // Interne pull-up inschakelen
}

void loop() {
  // === Encoder uitlezen ===
  long ruweWaarde = mijnEncoder.read();
  int klikWaarde = ruweWaarde / 4;

  if (klikWaarde != vorigeKlikWaarde) {
    int verschil = klikWaarde - vorigeKlikWaarde;
    vorigeKlikWaarde = klikWaarde;

    waarde += verschil * 0.1;

    // Clamp waarde tussen 0.0 en 100.0
    if (waarde < 0.0) waarde = 0.0;
    if (waarde > 100.0) waarde = 100.0;

    Serial.print("f (Hz): ");
    Serial.println(waarde, 1);
  }

  // === Schakelaar controleren ===
  if (digitalRead(knopPin) == LOW) {
    // Wacht tot knop losgelaten is (debounce)
    delay(50);
    if (digitalRead(knopPin) == LOW) {

      waarde = 0.0;
      vorigeKlikWaarde = 0;
      mijnEncoder.write(0);  // Zet interne encoderwaarde terug op 0

      Serial.print("f (Hz): ");
      Serial.println(waarde, 1);

      // Wacht tot knop losgelaten is
      while (digitalRead(knopPin) == LOW) {
        delay(10);
      }
    }
  }

  delay(5);
}
