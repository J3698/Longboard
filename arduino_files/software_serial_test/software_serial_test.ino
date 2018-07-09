#include <SoftwareSerial.h>

//int bluetoothTx = 9; // to tx pin of bluetooth
//int bluetoothRx = 8; // to rx pin of bluetooth

//SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  delay(100);
}

void loop() {
  String toBluetooth = "";
  while (Serial.available()) {
    toBluetooth += (char) Serial.read();
  }
  Serial1.print(toBluetooth);

  String toPc = "";
  while (Serial1.available()) {
    toPc += (char) Serial1.read();
  }
  Serial.print(toPc);
}

/*
  This program originally spoke to the
  UNO (Freeduino) with software serial.
  However the Pro Micro uses TX and RX
  with Serial1, leaving Serial to the
  Serial monitor. It works!
 */

