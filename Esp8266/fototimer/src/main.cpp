#include <Arduino.h>
#include <ESP8266WebServer.h> 
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <LittleFS.h>
#include <RTClib.h>

#define sensorPin A0

RTC_DS1307 rtc;

const char* ssid = "largada";  // SSID of your access point, change name as needed
const byte DNS_PORT = 53;           // Port for DNS server
DNSServer dnsS;                     // Create a DNSServer object

ESP8266WebServer server(80);        // Create a webserver object that listens for HTTP request on port 80
ESP8266WiFiClass Wifi;              // Create a Wifi object

String text, newText, tempo, strTime, id, mode = "Inicio/Fim"; // Initialize variables                

int seconds, sensorValue, range = 100; 
unsigned long sec_mill, previousMillis, interval = 100, start, finish; // Initialize variables for timing and sensor reading


void handle_root() {
  File file = LittleFS.open("/landingpage.html", "r");
  if (file) {
    String root = file.readString();
    root.replace("{{text}}", text); // Replace the placeholder with the text from the file
    root.replace("{{mode}}", mode); // Replace the placeholder with the mode
    file.close();    
    server.send(200, "text/html", root);
  } else {
    server.send(500, "text/plain", "Failed to open file for reading");
  }
}


void recalibrar() {
  sensorValue = 0;
  sensorValue = analogRead(sensorPin);
  sensorValue = sensorValue - range;
}


void get_time(){
  DateTime now = rtc.now();
  int hora = now.hour();
  int minuto = now.minute();
  int segundo = now.second();
  int milisegundo = (millis() - sec_mill) % 1000;
  tempo = "<td>" + String(hora) + ":" + String(minuto) + ":" + String(segundo) + ":" + String(milisegundo) + "</td></tr>" + text;
  strTime = String(hora) + ":" + String(minuto) + ":" + String(segundo) + ":" + String(milisegundo);
  recalibrar();
}


void settings() {
  get_time(); // Get the current time from the RTC module
  File file = LittleFS.open("/settings_page.html", "r"); // Open the settings HTML file
  if (file) {
    String settings = file.readString(); // Read the content of the file
    file.close(); // Close the file
    settings.replace("{{tempo}}", strTime); // Replace the placeholder with the current time
    settings.replace("{{range}}", String(range)); // Replace the placeholder with the range value
    server.send(200, "text/html", settings); // Send the settings page to the client
  } else {
    server.send(500, "text/plain", "Failed to open file for reading"); // Send HTTP status 500 (Internal server error) and the content type of the response
  }
}


void FileWrite() {
  File file = LittleFS.open("/text.txt", "w"); // Open the file in write mode 
  if (file) {
    file.println(newText); // Write the text to the file
    file.close();
    newText = "";
  } else {
    server.send(500, "text/plain", "Failed to open file for writing"); // Send HTTP status 500 (Internal server error) and the content type of the response
  }
}


void handle_post() {
  if (server.hasArg("message")) { // Check if the POST request has the message parameter
    id = server.arg("message");
  }

  if (tempo != "" && id != "") { // Check if the text is not empty

    newText = "<tr><td>" + id + "</td>" + tempo;
    text = newText;
    tempo = "";
    id = "";

    FileWrite(); // Write the text to permanent memory
  }
  
  handle_root();
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
      server.sendHeader("Content-Disposition", "attachment; filename=" + String(ssid) + ".txt");
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


void update_time() {
  if (server.hasArg("current-time")) { // Check if the POST request has the current-time parameter
    String time = server.arg("current-time");
    int hour = time.substring(0, 2).toInt();
    int minute = time.substring(3, 5).toInt();
    int second = time.substring(6, 8).toInt();
    rtc.adjust(DateTime(rtc.now().year(), rtc.now().month(), rtc.now().day(), hour, minute, second));
  }
  settings(); // Display the updated settings page
}


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  Serial.begin(9600);
  Wifi.softAP(ssid);               // Set the ESP8266 to Access Point mode
  Serial.println("Access Point mode enabled at IP: " + WiFi.softAPIP().toString());

  dnsS.start(DNS_PORT, "*", WiFi.softAPIP()); // Start the DNS server

  while (WiFi.softAPgetStationNum() == 0) { // Wait for a client to connect to the access point
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }

  if (!LittleFS.begin()) { // Initialize LittleFS
    Serial.println("Failed to initialize LittleFS");
    while (1) {} // Stop the program if LittleFS initialization fails
  }

  recalibrar();

  server.serveStatic("/style.css", LittleFS, "/style.css"); // Serve the CSS file
  server.onNotFound(handle_root); // Handle requests to the root URL
  server.on("/", HTTP_GET, handle_root);
  server.on("/post", HTTP_POST, handle_post);
  server.on("/delete", HTTP_GET, FileDelete);
  server.on("/download", HTTP_GET, FileDownload);
  server.on("/settings", HTTP_GET, settings);
  server.on("/update_time", HTTP_POST, update_time);
  server.on("/start_finish", HTTP_GET, []() {
    mode = "Inicio/Fim";
    handle_root();
  });
  server.on("/round_course", HTTP_GET, []() {
    mode = "Circuito Fechado";
    handle_root();
  });
  server.on("/range_set", HTTP_POST, []() {
    if (server.hasArg("range")) { // Check if the POST request has the message parameter
    range = server.arg("range").toInt();    
    }
    settings();
  });
  server.begin(); // Start the server
  FileRead();
}


void loop(){
  dnsS.processNextRequest();  // Handle DNS requests
  server.handleClient();      // Handle client requests

  // check for stable wifi connection
  if (WiFi.softAPgetStationNum() == 0){
    digitalWrite(LED_BUILTIN, HIGH);
  }

  // emulate milliseconds on RTC module
  if (seconds != rtc.now().second()){
    seconds = rtc.now().second();
    sec_mill = millis();
  }

  // Leitura do sensor com intervalo de tempo sem bloqueio do código
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) { // Verifica se o intervalo de leitura foi atingido
    previousMillis = currentMillis;
    interval = 100;    
    digitalWrite(LED_BUILTIN,LOW);
  
    if (analogRead(sensorPin) < sensorValue){ // Se a leitura do sensor for menor que o valor de referencia registra o tempo
      digitalWrite(LED_BUILTIN,HIGH);
      
      if (mode == "Inicio/Fim"){
        get_time();

      } else if (mode == "Circuito Fechado"){
        if (start == 0){
          start = millis();

        } else {
          finish = millis();
          float elapsedTime = (finish - start) / 1000.0;
          tempo = "<td>" + String(elapsedTime, 3) + "s</td></tr>" + text;
          start = 0;
        }
      }
      interval = 2000; // Aumenta o intervalo de leitura para evitar múltiplas leituras
    } else if (analogRead(sensorPin) > sensorValue + range * 2){ // Se a leitura do sensor for maior que o valor de referencia recalibra
      recalibrar();
    }
  }
}
