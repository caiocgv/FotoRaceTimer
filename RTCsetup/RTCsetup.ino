

#include "Wire.h"


#define DS1307_ADDRESS 0x68


byte zero = 0x00;
long starmil, segundos;
float endmil;
void setup()
{
  // Define o LCD com 20 colunas e 4 linhas
  Wire.begin();
  Serial.begin(9600);
  // A linha abaixo pode ser retirada apos setar a data e hora
  //SelecionaDataeHora();
}

void loop()
{
  Mostrarelogio();
}

void SelecionaDataeHora() // Seta a data e a hora do DS1307
{
  byte segundos = 0; // Valores de 0 a 59
  byte minutos = 48; // Valores de 0 a 59
  byte horas = 18; // Valores de 0 a 23
  byte diadasemana = 0; // Valores de 0 a 6 (0=Domingo, 1 = Segunda...)
  byte diadomes = 1; // Valores de 1 a 31
  byte mes = 5; // Valores de 1 a 12
  byte ano = 22; // Valores de 0 a 99
  Wire.beginTransmission(DS1307_ADDRESS);
  // Stop no CI para que o mesmo possa receber os dados
  Wire.write(zero);

  // As linhas abaixo escrevem no CI os valores de
  // data e hora que foram colocados nas variaveis acima
  Wire.write(ConverteParaBCD(segundos));
  Wire.write(ConverteParaBCD(minutos));
  Wire.write(ConverteParaBCD(horas));
  Wire.write(ConverteParaBCD(diadasemana));
  Wire.write(ConverteParaBCD(diadomes));
  Wire.write(ConverteParaBCD(mes));
  Wire.write(ConverteParaBCD(ano));
  Wire.write(zero); //Start no CI
  Wire.endTransmission();
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

void Mostrarelogio()
{
  //delay(10);
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
  int minutos = ConverteparaDecimal(Wire.read());
  int horas = ConverteparaDecimal(Wire.read());

  // Imprime mensagem na primeira linha do display
  Serial.print(horas);
  Serial.print(":");
  Serial.print(minutos);
  Serial.print(":");
  Serial.print(segundos);
  Serial.print(":");
  Serial.println(endmil,0);
  //delay(100);
}
