#include <SPI.h>
#include <SD.h>

File myFile;

#include "Wire.h"


#define DS1307_ADDRESS 0x68
#define sensorPin A3

int i = 0, sensorValue;
byte zero = 0x00;
long starmil, segundos;
float endmil;
const unsigned int MAX_MESSAGE_LENGTH = 12;

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

void recalibrar() {
  sensorValue = 0;
  for (i = 0; i < 10; i++) {
    //Serial.println(analogRead(sensorPin));
    sensorValue = sensorValue + analogRead(sensorPin);
  }
  sensorValue = sensorValue / 10 - 50;
  //Serial.println(sensorValue);
  i = 0;
}

void Read() {
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("READING FILE test.txt:");
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
  }
  else Serial.println("Nenhum arquivo disponível.");
}

void Delete() {
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("READING FILE test.txt:");
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    Serial.println(" ");
    SD.remove("test.txt");
    Serial.println("File test.txt DELETED");
  }
  else Serial.println("Nenhum arquivo disponível.");


}


void setup() {
  pinMode(10, OUTPUT);
  pinMode(A3, INPUT);
  Serial.begin(9600);
  Wire.begin();
  if (!SD.begin(5)) {
    Serial.println("initialization failed!");
    while (1);
  }
  myFile = SD.open("test.txt", FILE_WRITE);
  //  if (myFile) {
  //    Serial.println("Writing to test.txt...");
  // myFile.println("STARTING THE SYSTEM");
  //    // close the file:
  myFile.close();
  //    Serial.println("done.");
  //  } else {
  //    // if the file didn't open, print an error:
  //    Serial.println("error opening test.txt");
  //  }
  recalibrar();
  Serial.println("SYSTEM READY!");
}

void loop() {
  // delay(5);
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 1);
  endmil = ConverteparaDecimal(Wire.read());
  if ( segundos != endmil) {
    starmil = millis()  ;
  }
  endmil = millis() - starmil;
  /*endmil = endmil / 15;
    endmil = endmil * 15;*/

  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 3);
  segundos = ConverteparaDecimal(Wire.read());
  int minutos = ConverteparaDecimal(Wire.read());
  int horas = ConverteparaDecimal(Wire.read());

  //Serial.println(cont);
  if (analogRead(A3) < sensorValue) {
    if (i == 1) {

      i = 0;
      digitalWrite(10, LOW);
      myFile = SD.open("test.txt", FILE_WRITE);
      myFile.print(horas);
      myFile.print(":");
      myFile.print(minutos);
      myFile.print(":");
      myFile.print(segundos);
      myFile.print(":");
      myFile.println(endmil, 0);
      myFile.close();
      Serial.print(horas);
      Serial.print(":");
      Serial.print(minutos);
      Serial.print(":");
      Serial.print(segundos);
      Serial.print(":");
      Serial.println(endmil, 0);

      //escolha o delay necessário de acordo com a aplicação. (largada ou chegada)
      //delay(500);
      delay(5000);
      recalibrar();
    }
  }
  else {
    i = 1;
    digitalWrite(10, HIGH);
  }
  //delay(500);





  while (Serial.available() > 0)
  {
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
      Serial.println(message);

      String msg = message;
      //Serial.println(msg);
      if (msg == "Read") {
        Read();
      }
      else if (msg == "Delete") {
        Delete();
      }
      else {
        myFile = SD.open("test.txt", FILE_WRITE);
        myFile.println(message);
        myFile.close();
        Serial.println("Passagem computada com sucesso!");
      }
      message_pos = 0;
    }
  }
}
