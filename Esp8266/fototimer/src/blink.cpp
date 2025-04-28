#include <Arduino.h>

void setup() {
    // Initialize the serial communication
    Serial.begin(115200);
    Serial.println("ESP testing");

    // Set the LED pin as an output
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {

    // Delay for a short period of time
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
}