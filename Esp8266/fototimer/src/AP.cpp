#include <ESP8266WiFi.h>

const char* ssid = "teste";
String devices = ""; // String to hold the scanned devices


void scan_all() { // Function to scan for all devices
  Serial.println("Scanning for devices..."); // Print message to the serial monitor
  int numDevices = WiFi.scanNetworks(false, true); // Scan for networks on all channels
  WiFi.scanComplete(); // Wait for the scan to complete
  if (numDevices > 0) { // If there are devices found
    for (int i = 0; i < numDevices; i++) { // Loop through the devices
      Serial.println(String(WiFi.SSID(i)) + " : " + String(WiFi.RSSI(i)) + " : " + String(WiFi.channel(i))); // Get the device information
    }
  }
}

void setup() {
    Serial.begin(115200);
    WiFi.softAP(ssid, "123", 6);

    IPAddress apIP(192, 168, 4, 1);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
    scan_all(); // Scan for all devices
    
}


void loop() {
    // Your code here
}