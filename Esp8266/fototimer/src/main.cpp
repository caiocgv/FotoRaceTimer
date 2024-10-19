#include <Arduino.h>
#include <ESP8266WebServer.h> 
#include <ESP8266WiFi.h>
#include <DNSServer.h>

const char* ssid = "FotoCelula1";  // SSID of your access point
const byte DNS_PORT = 53;           // Port for DNS server
DNSServer dnsS;                     // Create a DNSServer object

ESP8266WebServer server(80);        // Create a webserver object that listens for HTTP request on port 80
ESP8266WiFiClass Wifi;              // Create a Wifi object

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  Wifi.softAP(ssid);               // Set the ESP8266 to Access Point mode
  Serial.println("Access Point mode enabled at IP: " + WiFi.softAPIP().toString());

  dnsS.start(DNS_PORT, "*", WiFi.softAPIP()); // Start the DNS server

  server.on("/", [](){
    String text = "Uptime: " + String(millis()/1000) + " seconds";
    server.send(200, "text/html", "<meta http-equiv='refresh' content='2'>"  // Refresh the page every 2 seconds
                                  "<h1>"
                                      + text +                               // Display the uptime, variable text can be added here
                                  "</h1>"
                                  );
  });
  server.begin(); // Start the server
}

void loop(){
  dnsS.processNextRequest();  // Handle DNS requests
  server.handleClient();      // Handle client requests
  
  if (WiFi.softAPgetStationNum() == 0) {
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else{
    digitalWrite(LED_BUILTIN, LOW);
  }
  
}