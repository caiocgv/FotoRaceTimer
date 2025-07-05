// Include the Ultrasonic library
#include <Arduino.h>

// Define the pins for the ultrasonic sensor
const int trigPin = 14; // GPIO 14 (D5 on NodeMCU)
const int echoPin = 12; // GPIO 12 (D6 on NodeMCU)

float sensorValue = 0, range = 10; // Default range in cm


float ultrasonic() {
    // Set the trigger pin as an output
    pinMode(trigPin, OUTPUT);
    // Set the echo pin as an input
    pinMode(echoPin, INPUT);

    // Send a 10 microsecond pulse to the trigger pin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Measure the time it takes for the echo to return
    long duration = pulseIn(echoPin, HIGH, 50000);

    // Calculate the distance in centimeters
    float distance = (duration / 2.0) * 0.0343;

    return distance;
}


void recalibrar() {
  
  sensorValue = 0;
  pinMode(LED_BUILTIN, OUTPUT);
    for (int i = 0; i < 3; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(200);
        digitalWrite(LED_BUILTIN, LOW);
        delay(200);
        sensorValue = sensorValue + ultrasonic(); // Get the distance from the ultrasonic sensor
    }
    Serial.print("Recalibrating... New sensor value: ");
    Serial.println(sensorValue/3);
    sensorValue = sensorValue / 3; // Average the sensor value over 3 readings
}

void setup() {
    // Initialize the serial communication
    Serial.begin(115200);
    Serial.println("Ultrasonic Sensor Test");

    // Set the LED pin as an output
    pinMode(LED_BUILTIN, OUTPUT);
    recalibrar(); // Call the recalibrar function to set the initial sensor value
}

void loop() {
    // Blink the LED to indicate the program is running    
    digitalWrite(LED_BUILTIN, LOW);

    // Measure the distance in centimeters
    float distance = ultrasonic();
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    // Check if the distance is less than 10 cm
    if (distance < sensorValue - range) {
        Serial.println("Object detected!");        
        digitalWrite(LED_BUILTIN, HIGH);
        // Wait for a short period of time
        delay(5000);
        // Add your action here, e.g., turn on a relay or send a notification
        recalibrar(); // Call the recalibrar function to update the sensor value
    } else if (distance > sensorValue + range) {
        sensorValue = distance; // Update the sensor value
        
        for (int i = 0; i < 3; i++) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(200);
            digitalWrite(LED_BUILTIN, LOW);
            delay(200);
        }
        Serial.print("Recalibrating... New sensor value: ");
        Serial.println(sensorValue);
        
    } else {
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);

    }
}