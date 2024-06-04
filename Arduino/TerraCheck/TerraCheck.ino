#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

#define ONE_WIRE_BUS 8

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

SoftwareSerial hc05(3, 4);

float Celsius = 0;
float Fahrenheit = 0;

void setup() {
  sensors.begin();
  Serial.begin(9600);
}

void loop() {
  sensors.requestTemperatures();

  Celsius = sensors.getTempCByIndex(0);
  Fahrenheit = sensors.toFahrenheit(Celsius);
  
  if(Celsius> -127){
  //Serial.print(Celsius);
  //Serial.print(" C  ");
  Serial.println(Fahrenheit); // For debug
  //Serial.println(" F");
  hc05.println(Fahrenheit);

  delay(1000);
  }
}
