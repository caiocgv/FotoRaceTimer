#include <Arduino.h>
#include <ESP8266WebServer.h> 
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <LittleFS.h>

const char* ssid = "FotoCelula1";  // SSID of your access point
const byte DNS_PORT = 53;           // Port for DNS server
DNSServer dnsS;                     // Create a DNSServer object

ESP8266WebServer server(80);        // Create a webserver object that listens for HTTP request on port 80
ESP8266WiFiClass Wifi;              // Create a Wifi object

String text;                   // Variable to store the text to be displayed on the webpage



void handle_root() {
    server.send(200, "text/html",                     // Send HTTP status 200 (Ok) and the content type of the response
                                  "<!DOCTYPE html>"
                                  "<html>"
                                  "<head>"
                                    "<meta name='viewport' content='width=device-width, initial-scale=1'>"
                                    "<style>"
                                      "body {font-family: Arial; margin-left: 20px;}"
                                      "form {display: inline-block;}"
                                      "button {background-color: #4CAF50; color: white; padding: 14px 20px; margin: 8px 0; border: none; cursor: pointer; font-size: xx-large;}"
                                      "button:hover {opacity: 0.8;}"
                                    "</style>"
                                  "<title> FotoCelula1 </title>"  
                                  "</head>"
                                  "<body>"
                                  "<form action='/post' method='post'>"
                                    "<label for='numberInput' style='font-size: xx-large'>Enter a number:</label>"
                                    "<input type='number' id='message' name='message' onchange='submitForm()'>"
                                    ""
                                  "</form>"
                                  "<h2 style='font-size: xx-large'>"
                                      + text +        // Display the text on the webpage
                                  "</h2>"
                                  "<form action='/delete'>"
                                    "<button type='submit'>Limpar Resultados</button>"
                                  "</form>"
                                  "<form action='/download'>"
                                    "<button type='submit'>Salvar Dados</button>"
                                  "</form>"
                                  "</body>"
                                  );
}


void FileWrite() {
  File file = LittleFS.open("/text.txt", "w"); // Open the file in write mode
  if (file) {
    file.println(text);
    file.close();
  } else {
    server.send(500, "text/plain", "Failed to open file for writing"); // Send HTTP status 500 (Internal server error) and the content type of the response
  }
}

void FileRead() {
  if (LittleFS.exists("/text.txt")) {
    File file = LittleFS.open("/text.txt", "r");
    if (file) {
      String content = file.readString();
      text = content;
      file.close();
    } else {
      server.send(500, "text/plain", "Failed to open file for reading");
    }
  }
}

void FileDelete() {
  if (LittleFS.exists("/text.txt")) {
    if (LittleFS.remove("/text.txt")) {
      Serial.println("File deleted successfully");
    } else {
      Serial.println("Failed to delete file");
    }
  } else {
    Serial.println("File does not exist");
  }
  text = ""; // Clear the text
  handle_root(); // Display the updated text on the webpage
}

void FileDownload() {
  if (LittleFS.exists("/text.txt")) {
    File file = LittleFS.open("/text.txt", "r");
    if (file) {
      server.sendHeader("Content-Disposition", "attachment; filename=FotoCelula1.txt");
      server.streamFile(file, "application/octet-stream");
      file.close();
    } else {
      server.send(500, "text/plain", "Failed to open file for reading");
    }
  } else {
    server.send(404, "text/plain", "File not found");
  }
  handle_root(); // Display the updated text on the webpage
}

void handle_post() {
  String message = "POST request with no parameters";
  if (server.hasArg("message")) {
    text = text + "<br>" + server.arg("message");
  }
  FileWrite(); // Write the text to permanent memory
  handle_root(); // Display the updated text on the webpage
}

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  Wifi.softAP(ssid);               // Set the ESP8266 to Access Point mode
  Serial.println("Access Point mode enabled at IP: " + WiFi.softAPIP().toString());

  dnsS.start(DNS_PORT, "*", WiFi.softAPIP()); // Start the DNS server

  if (!LittleFS.begin()) { // Initialize LittleFS
    Serial.println("Failed to initialize LittleFS");
    while (1) {} // Stop the program if LittleFS initialization fails
  }

  server.on("/", HTTP_GET, handle_root);
  server.on("/post", HTTP_POST, handle_post);
  server.on("/delete", HTTP_GET, FileDelete);
  server.on("/download", HTTP_GET, FileDownload);
  server.begin(); // Start the server
  FileRead();
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