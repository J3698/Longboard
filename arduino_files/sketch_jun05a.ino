/*
  Example Bluetooth Serial Passthrough Sketch
 by: Jim Lindblom
 SparkFun Electronics
 date: February 26, 2013
 license: Public domain

 This example sketch converts an RN-42 bluetooth module to
 communicate at 9600 bps (from 115200), and passes any serial
 data between Serial Monitor and bluetooth module.
 */
#include <SoftwareSerial.h>  

int bluetoothTx = 2;  // TX-O pin of bluetooth mate, Arduino D2
int bluetoothRx = 3;  // RX-I pin of bluetooth mate, Arduino D3

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

void setup()
{
  Serial.begin(9600);  // Begin the serial monitor at 9600bps

  bluetooth.begin(115200);  // The Bluetooth Mate defaults to 115200bps
  bluetooth.print("$");  // Print three times individually
  bluetooth.print("$");
  bluetooth.print("$");  // Enter command mode
  delay(100);  // Short delay, wait for the Mate to send back CMD
  bluetooth.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
  // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
  bluetooth.begin(9600);  // Start bluetooth serial at 9600
}

void loop()
{
  if(bluetooth.available())  // If the bluetooth sent any characters
  {
    // Send any characters the bluetooth prints to the serial monitor
    Serial.print((char)bluetooth.read());  
  }
  if(Serial.available())  // If stuff was typed in the serial monitor
  {
    // Send any characters the Serial monitor prints to the bluetooth
    bluetooth.print((char)Serial.read());
  }
  // and loop forever and ever!
}



/*
#include <SoftwareSerial.h> 

int bluetoothTx = 2;  // TX-O pin of bluetooth mate, Arduino D2
int bluetoothRx = 3;  // RX-I pin of bluetooth mate, Arduino D3
int LEDpin = 13;

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

void setup()
{
  Serial.begin(115200);  // Begin the serial monitor at 9600bps
  bluetooth.begin(115200);  // The Bluetooth Mate defaults to 115200bps
}

void loop()
{
  if(bluetooth.available())  // If the bluetooth sent any characters
  {
    Serial.println("bluetooth");
    char data = (char)bluetooth.read();
    Serial.print(data);
    if(data == 'h'){
      digitalWrite(LEDpin, HIGH);
    }
    if(data == 'l'){
       digitalWrite(LEDpin, LOW);
    }
  }

  if(Serial.available())  // If stuff was typed in the serial monitor
  {
    Serial.println("serial");
    char data1 = (char)Serial.read();
    bluetooth.print(data1);
    if(data1 == 'h'){
      Serial.println(data1);
      digitalWrite(LEDpin, HIGH);
    }
    if(data1 == 'l'){
      Serial.println(data1);
       digitalWrite(LEDpin, LOW);
    }
  }
}
*/


/*
#include <Servo.h>
#include <Adafruit_NeoPixel.h>
// #include <SoftwareSerial.h>

#define RGB_PIN 6
#define ESC_PIN 9

Servo mESC;
Adafruit_NeoPixel mStrip;

int bluetoothTx = 3;
int bluetoothRx = 4;
// SoftwareSerial bluetooth(bluetoothTx,bluetoothRx);

void setup() {
  mESC.attach(ESC_PIN);
  arm(mESC);
  mStrip = Adafruit_NeoPixel(60, RGB_PIN);
  mStrip.begin();
  mStrip.show();

  Serial.begin(115200);  // Begin the serial monitor at 9600bps
}

void loop() {
  for (int speed = 0; speed <= 70; speed += 5) {
    setSpeed(mESC, speed);
    delay(100);
  }
  setSpeed(mESC, 0);
  for (int b = 0; b < 20; b++) {
    for (int led = 0; led < 60; led++) {
     mStrip.setPixelColor(led, 0, 0, b);
    }
    mStrip.show();
    delay(30);
  }
  if(Serial.available())  // If stuff was ty21ped in the serial monitor
  {
    char data = Serial.read();        //Read the incoming data & store into data
    Serial.print(data);          //Print Value inside data in Serial monitor
    Serial.print("\n");
    if(data == '1')              // Checks whether value of data is equal to 1
      digitalWrite(13, HIGH);   //If value is 1 then LED turns ON
    else if(data == '0')         //  Checks whether value of data is equal to 0
      digitalWrite(13, LOW);    //If value is 0 then LED turns OFF
  
    // Send any characters the Serial monitor prints to the bluetooth
//    bluetooth.print((char)Serial.read());
  }
}

void setSpeed(Servo ESC, int speed) {
  int a = map(speed, 0, 100, 0, 180);
  ESC.write(a);
}

void arm(Servo ESC) {
  setSpeed(ESC, 0);
  delay(100);
}

*/
