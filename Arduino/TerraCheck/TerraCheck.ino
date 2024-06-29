#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

#define ONE_WIRE_BUS 8

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

SoftwareSerial hc05(4, 3);

float Celsius = 0;
float Fahrenheit = 0;

void setup() {
  sensors.begin();
  Serial.begin(9600);
  hc05.begin(9600);
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

  Serial.println(Fahrenheit); // For debug
}



// #include <SoftwareSerial.h>

// char value;
// int Tx = 4;       // connect BT module TX to 4
// int Rx = 3;       // connect BT module RX to 3

// const int LED = 7;
// const int buttonPin = 4;

// int buttonState = 0;


// // creates a "virtual" serial port/UART
// SoftwareSerial bluetooth(Tx, Rx);

// void setup() {
//   pinMode(LED, OUTPUT);
//   pinMode(buttonPin, INPUT);

//   // start serial communication at default baud rate 9600bps
//   Serial.begin(9600);
//   bluetooth.begin(9600);

// }

// void loop() {

//   if (bluetooth.available()) {

//     value = bluetooth.read();

//     buttonState = digitalRead(buttonPin);

//     // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
//     if (buttonState == HIGH) {  // turn LED on:
//       digitalWrite(LED, HIGH);
//       Serial.println("ON");
//     } else {
//       // turn LED off:
//       digitalWrite(LED, LOW);
//       Serial.println("OFF");
//     }


//     if (value == 1) {  // turn LED on:
//       digitalWrite(LED, HIGH);
//       Serial.println("ON");
//     }

//   }
// }




// void setup() {
//   Serial.begin(9600);
// }

// void loop() { 
//   Serial.println("Test");
//   delay(1000);
// }


// #include <SoftwareSerial.h>

// SoftwareSerial BTSerial(3, 4); // RX | TX

// void setup()
// {
//   pinMode(9, OUTPUT);  // this pin will pull the HC-05 pin 34 (key pin) HIGH to switch module to AT mode
//   digitalWrite(9, HIGH);
//   Serial.begin(9600);
//   Serial.println("Enter AT commands:");
//   BTSerial.begin(38400);  // HC-05 default speed in AT command more
// }

// void loop()
// {
//   // Keep reading from HC-05 and send to Arduino Serial Monitor
//   if (BTSerial.available())
//     Serial.write(BTSerial.read());

//   // Keep reading from Arduino Serial Monitor and send to HC-05
//   if (Serial.available())
//     BTSerial.write(Serial.read());
// }
