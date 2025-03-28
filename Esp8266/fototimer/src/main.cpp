#include <Arduino.h>
#include <ESP8266WebServer.h> 
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <LittleFS.h>
#include <RTClib.h>

#define sensorPin A0

RTC_DS1307 rtc;

const char* ssid = "FotoCelula1";  // SSID of your access point
const byte DNS_PORT = 53;           // Port for DNS server
DNSServer dnsS;                     // Create a DNSServer object

ESP8266WebServer server(80);        // Create a webserver object that listens for HTTP request on port 80
ESP8266WiFiClass Wifi;              // Create a Wifi object

String text, newText, tempo, id;                   // Variable to store the text to be displayed on the webpage

int seconds, sensorValue; 
unsigned long sec_mill, previousMillis, interval = 100;


void handle_root() {
    server.send(200, "text/html",                     // Send HTTP status 200 (Ok) and the content type of the response
                                   "<!DOCTYPE html> \
                                   <html> \
                                   <head> \
                                     <meta name='viewport' content='width=device-width, initial-scale=1'> \
                                     <style> \
                                    body { \
                                        font-family: Arial; \
                                        margin-left: 20px; \
                                        background-color: rgb(48, 46, 46); \
                                        color: aliceblue; \
                                    } \
 \
                                    div { \
                                        display: flex; \
                                        justify-content: space-evenly; \
                                    } \
 \
                                    form { \
                                        display: inline-block; \
                                    } \
 \
                                    input { \
                                        width: 100px; \
                                        height: 30px; \
                                        font-size: xx-large; \
                                        border-radius: 5px; \
                                    } \
 \
                                    button { \
                                        background-color: gray; \
                                        color: aliceblue; \
                                        padding: 4px 8px; \
                                        margin: 8px 0; \
                                        border: none; \
                                        cursor: pointer; \
                                        font-size: large; \
                                        border-radius: 5px; \
                                    } \
 \
                                    button:hover { \
                                        opacity: 0.8; \
                                    } \
                                    table { \
                                        width: 50%; \
                                        text-align: center; \
                                        font-size: x-large; \
                                        border-radius: 5px;  \
                                    } \
 \
                                    table tr:nth-child(even) { \
                                        background-color: lightgray; \
                                        color: black; \
                                    } \
 \
                                    table tr:nth-child(odd) { \
                                        background-color: rgb(76, 76, 76); \
                                    } \
                                    h1 { \
                                        text-align: center; \
                                    } \
                                     </style> \
                                    <title> FotoCelula1 </title>   \
                                    </head> \
                                    <body> \
                                    <h1>Modo de Uso</h1> \
                                    <div> \
                                    <div style='width: 500px;'> \
                                        <form action='/start_finish'> \
                                            <button type='submit'>Inicio/Fim</button> \
                                        </form> \
                                        <form action='/round_course'> \
                                            <button type='submit'>Circuito Fechado</button> \
                                        </form> \
                                    </div> \
                                    </div> \
                                    <br><hr><br> \
                                    <div> \
                                        <form action='/post' method='post'> \
                                            <label for='numberInput' style='font-size: xx-large'>Digite ID: </label> \
                                            <input type='number' id='message' height= 20px name='message' onchange='submitForm()'>                                        \
                                        </form> \
                                    </div> \
                                    <br><hr><br> \
                                    <div> \
                                        <table> \
                                            <tr> \
                                                <th>ID</th> \
                                                <th>Time</th> \
                                            </tr>"
                                                + text +
                                        "</table> \
                                    </div> \
                                    <br><hr><br> \
                                    <div> \
                                        <form action='/delete'> \
                                            <button type='submit'>Limpar Resultados</button> \
                                        </form> \
                                        <form action='/download'> \
                                            <button type='submit'>Salvar Dados</button> \
                                        </form> \
                                    </div> \
                                   </body> \
                                  ");
}

void recalibrar() {
  sensorValue = 0;
  sensorValue = analogRead(sensorPin);
  sensorValue = sensorValue - 50;
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

void get_time(){
  DateTime now = rtc.now();
  int hora = now.hour();
  int minuto = now.minute();
  int segundo = now.second();
  int milisegundo = (millis() - sec_mill) % 1000;
  tempo = "<td>" + String(hora) + ":" + String(minuto) + ":" + String(segundo) + ":" + String(milisegundo) + "</td></tr>" + text;
  recalibrar();
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

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  // setTime();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

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
      get_time();
      interval = 2000;

    } else if (analogRead(sensorPin) > sensorValue + 100){ // Se a leitura do sensor for maior que o valor de referencia recalibra
      recalibrar();
    }
  }
}