// Include the Ultrasonic library
#include <HCSR04.h>

// Define the pins for the ultrasonic sensor
const int trigPin = 5;
const int echoPin = 6;
const int maxdistance = 200; // Maximum distance in cm

// Create an instance of the Ultrasonic class
UltraSonicDistanceSensor ultrasonic(trigPin, echoPin, maxdistance, 30000); // Set max distance to 400 cm and max timeout to 30 milliseconds

void setup() {
    // Initialize the serial communication
    Serial.begin(115200);
}

void loop() {
    // Measure the distance in centimeters
    float distance = ultrasonic.measureDistanceCm(30);

    // Print the distance to the serial monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Delay for a short period of time
    delay(1000);
}