#define ledpin 10
#define sensorPin A3
#include <SPI.h>
#include <SD.h>

File myFile;

int i, sensorValue;
long starmil, segundos;
float endmil1, endmil2;
const unsigned int MAX_MESSAGE_LENGTH = 12;
const int ledPin = 10;
char str[50];

void recalibrar() {
  sensorValue = 0;
  for (i = 0; i < 10; i++) {
    //Serial.println(analogRead(sensorPin));
    sensorValue = sensorValue + analogRead(sensorPin);
  }
  sensorValue = sensorValue / 10 - 20;
  Serial.println(sensorValue);
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
}

void Delete() {
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("READING FILE test.txt:");
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
  }
  Serial.println(" ");
  SD.remove("test.txt");
  Serial.println("File test.txt DELETED");

}

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(A3, INPUT);
  Serial.begin(9600);
  if (!SD.begin(5)) {
    Serial.println("initialization failed!");
    while (1);
  }
  myFile = SD.open("test.txt", FILE_WRITE);
  //  if (myFile) {
  //    Serial.println("Writing to test.txt...");
  myFile.println("Nome,Volta 1,Volta 2,Total");
  //    // close the file:
  myFile.close();

  recalibrar();
  Serial.println("System Ready!");
}

void loop() {

  if (analogRead(A3) < sensorValue) {
    if (i == 1) {
      starmil = millis();
      digitalWrite(ledpin, LOW);
      delay(500);
      recalibrar();

      while (analogRead(A3) > sensorValue) {
        digitalWrite(ledpin, HIGH);
        /*endmil1 = millis() - starmil;
          endmil1 = endmil1 / 1000;
          //sprintf(str, "Tempo atual: %s s", endmil);
          //Serial.println(str);
          Serial.print("Primeira volta: ");
          Serial.print(endmil1, 3);
          Serial.println(" s");*/
      }
      endmil1 = millis() - starmil;
      endmil1 = endmil1 / 1000;
      /*if (endmil1 < 5) {
        if (endmil1 < 2) {
          myFile = SD.open("test.txt");
          if (myFile) {
            Serial.println("READING FILE test.txt:");
            while (myFile.available()) {
              Serial.write(myFile.read());
            }
          }
          Serial.println(" ");
          SD.remove("test.txt");
          Serial.println("File test.txt DELETED");
        }
        else {
          myFile = SD.open("test.txt");
          if (myFile) {
            Serial.println("READING FILE test.txt:");
            while (myFile.available()) {
              Serial.write(myFile.read());
            }
            myFile.close();
          }
        }
        }
        else {*/
      digitalWrite(ledpin, LOW);
      delay(500);
      Serial.print("Tempo da volta 1: ");
      Serial.print(endmil1, 3);
      Serial.println(" s");
      myFile = SD.open("test.txt", FILE_WRITE);
      myFile.print(",");
      myFile.print(endmil1, 3);
      myFile.print(",");
      myFile.close();
      delay(5000);
      while (analogRead(A3) > sensorValue) {
        digitalWrite(ledpin, HIGH);
        /*endmil2 = millis() - (starmil + endmil1 * 1000);
          endmil2 = endmil2 / 1000;
          //sprintf(str, "Tempo atual: %s s", endmil);
          //Serial.println(str);
          Serial.print("Segunda volta: ");
          Serial.print(endmil2, 3);
          Serial.println(" s");*/
      }
      endmil2 = millis() - (starmil + endmil1 * 1000);
      endmil2 = endmil2 / 1000;
      recalibrar();
      digitalWrite(ledpin, LOW);
      delay(500);
      Serial.print("Tempo da volta 2: ");
      Serial.print(endmil2, 3);
      Serial.println(" s");
      delay(5000);
      myFile = SD.open("test.txt", FILE_WRITE);
      //myFile.print(",Tempo da volta 1,");
      myFile.print(endmil2, 3);
      endmil2 = endmil2 + endmil1;
      myFile.print(",");
      myFile.println(endmil2, 3);
      myFile.close();
      Serial.print("Tempo Final: ");
      Serial.print(endmil2, 3);
      Serial.println(" s");

    }
  }
  else {

    digitalWrite(ledPin, HIGH);
    i = 1;
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
        myFile.print(message);
        myFile.close();
      }
      message_pos = 0;
    }
  }
}
