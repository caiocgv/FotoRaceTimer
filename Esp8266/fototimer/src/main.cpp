#include <Arduino.h>
#include <ESP8266WebServer.h> 
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <LittleFS.h>
#include <Wire.h>

#define DS1307_ADDRESS 0x68
byte zero = 0x00;

const char* ssid = "FotoCelula1";  // SSID of your access point
const byte DNS_PORT = 53;           // Port for DNS server
DNSServer dnsS;                     // Create a DNSServer object

ESP8266WebServer server(80);        // Create a webserver object that listens for HTTP request on port 80
ESP8266WiFiClass Wifi;              // Create a Wifi object

String text;                   // Variable to store the text to be displayed on the webpage



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


byte decToBcd(byte val){
// Conversão de decimal para binário
  return ( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val)  {
// Conversão de binário para decimal
  return ( (val/16*10) + (val%16) );
}

void get_time(){
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_ADDRESS, 3);

  int segundo = bcdToDec(Wire.read());
  int minuto = bcdToDec(Wire.read());
  int hora = bcdToDec(Wire.read() & 0b111111);    //Formato 24 horas
  text = "<td>" + String(hora) + ":" + String(minuto) + ":" + String(segundo) + "</td></tr>" + text;
}

void FileWrite() {
  File file = LittleFS.open("/text.txt", "w"); // Open the file in write mode
  if (file) {
    file.println(text); // Write the text to the file
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
    get_time();
    text = "<tr><td>" + server.arg("message") + "</td>" + text; // Add the new text to the existing text
    
  }
  FileWrite(); // Write the text to permanent memory
  handle_root(); // Display the updated text on the webpage
}

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);

  Wire.begin();
  // setTime();

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
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else{
    digitalWrite(LED_BUILTIN, LOW);
  }
  
}