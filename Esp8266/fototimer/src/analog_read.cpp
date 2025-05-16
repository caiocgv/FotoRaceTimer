#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    pinMode(BUILTIN_LED, OUTPUT); // Initialize the LED pin as an output
}

void loop() {
    int sensorValue = analogRead(A0);
    if (sensorValue > 500){
        digitalWrite(BUILTIN_LED, HIGH); // Turn the LED on
    } else {
        digitalWrite(BUILTIN_LED, LOW); // Turn the LED off
    }
    Serial.print("Analog reading: ");
    Serial.println(sensorValue);
    delay(1000);
}