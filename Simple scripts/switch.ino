const int switchPin = 13;  // Pin waarop de schakelaar is aangesloten
bool switchState = false; // Boolean variabele

void setup() {
    pinMode(switchPin, INPUT); // Interne pull-up weerstand inschakelen
    Serial.begin(9600);
}

void loop() {
    if (digitalRead(switchPin) == LOW) { // Laag betekent dat de schakelaar aan staat
        switchState = true;
    } else {
        switchState = false;
    }
    
    Serial.println(switchState); // Print de boolean waarde naar de seriële monitor
    delay(100);
}
