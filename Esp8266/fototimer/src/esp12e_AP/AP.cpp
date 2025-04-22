#include <ESP8266WiFi.h>

const char* ssid = "teste";

void setup() {
    Serial.begin(115200);
    WiFi.softAP(ssid, "123", 6);

    IPAddress apIP(192, 168, 4, 1);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
}

void loop() {
    // Your code here
}