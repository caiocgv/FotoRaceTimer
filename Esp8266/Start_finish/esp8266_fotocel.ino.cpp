/*********
  Descrição do hardware

    conexão LDR
  -pino 3v3 ligado direto ao LDR
  -saída do LDR ligado ao pino A0
  -resistor 10k pulldown da saída LDR para o GND
  
    conexão RTC
  -pino Vin ao VCC
  -GND ao GND
  -SCL ao pino d1 com resistor de 10k pullup para 3.3v
  -SDA ao pino D2 com resistor de 10k pullup para 3.3v

  SD card foi substituido pelo SPIFFS
  Nenhuma conexão externa é necessaria.
  

   LED
  -pin16 > catodo
  -anodo > resistor 330
  -resistor > GND
  
*********/

#include <Arduino.h>
#include "FS.h"
String filename = "/TESTE.txt";
const unsigned int MAX_MESSAGE_LENGTH = 12;

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "Wire.h"
#define ledpin 16
#define DS1307_ADDRESS 0x68
AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "JSA@junior";
const char* password = "tenere18";
const char* PARAM_INPUT_1 = "Numero do atleta";
int sensorPin = A0;
int sensorValue, i, last, minutos, horas;
long starmil, segundos, endmil;
String inputMessage;

// HTML web page
String index_html = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Leituras da Foto Celula</title>
  <h1>Registro de tempos</h1>
  <p>INSIRA O NUMERO DO ULTIMO ATLETA APOS A PASSAGEM NA FOTOCELULA</p>
  <hr>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    Numero do atleta: <input type="number" name="Numero do atleta">
    <input type="submit" value="Submit">
  </form><br>
  <hr>
  <h2>LISTA DE REGISTROS</h2>
</body></html>
)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void recalibrar() {
  sensorValue = 0;
  for (i = 0; i < 10; i++) {
    //Serial.println(analogRead(sensorPin));
    sensorValue = sensorValue + analogRead(sensorPin);
    delay(5);
  }
  sensorValue = sensorValue / 10 - 40;
  //Serial.println(sensorValue);
  i = 0;
}

byte ConverteParaBCD(byte val)
{
  // Converte o número de decimal para BCD
  return ( (val / 10 * 16) + (val % 10) );
}

byte ConverteparaDecimal(byte val)
{
  // Converte de BCD para decimal
  return ( (val / 16 * 10) + (val % 16) );
}

void gettime()
{
  //delay(10);
  byte zero = 0x00;
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 1);
  endmil = ConverteparaDecimal(Wire.read());
  if ( segundos != endmil) {
    starmil = millis()  ;
  }
  endmil = millis() - starmil;
  /*endmil=endmil/15;
    endmil=endmil*15;*/

  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 3);
  segundos = ConverteparaDecimal(Wire.read());
  minutos = ConverteparaDecimal(Wire.read());
  horas = ConverteparaDecimal(Wire.read());
}

void READ(){
  Serial.println("\n\n---CONTEUDO DO ARQUIVO---\n");
  File file = SPIFFS.open(filename,"r");
  while(file.available()){
    Serial.write(file.read());
  }
  file.close();
  Serial.println("\n---FIM DO ARQUIVO---\n");
}
 
void DELETE(){
  READ();
  SPIFFS.remove(filename);
  Serial.println("\n\n---Arquivo excluido com sucesso!---\n\n");
}

void Read_msg(){
    static char message[MAX_MESSAGE_LENGTH];
    static unsigned int message_pos = 0;
    char inByte = Serial.read();

    if ( inByte != '\n' && (message_pos < MAX_MESSAGE_LENGTH - 1) )
    {
      message[message_pos] = inByte;
      message_pos++;
    }
    else
    {
      message[message_pos] = '\0';      
      //Serial.println(message);
      String msg = message;
      //Serial.println(msg);
      if (msg == "Read") {
        READ();
      }
      else if (msg == "Delete") {
        DELETE();
      }
      else {
        File file = SPIFFS.open(filename, "a+");
        file.println(message);
        file.close();
        Serial.println("\nPassagem computada com sucesso!\n");
      }
      message_pos = 0;
    }
}

void setup() {
  Serial.begin(9600);
  
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  File file = SPIFFS.open("/TESTE.txt", "a+");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }
  else{
    Serial.println("O sistema está pronto!");
  }
  file.close();

//DELETE();
//READ();
}
void loop() {

  while (Serial.available() > 0)
  {
    Read_msg();
  }
}