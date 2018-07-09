#include <SoftwareSerial.h>

// int bluetoothTx = TX-O pin of bluetooth mate, Arbnmjzduino D2
// int bluetoothRx = RX-I pin of bluetooth mate, Arduino D3
// SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

void setup() {
  String out = "";

  Serial.begin(115200);  // Begin the serial monitor at 9600bps
  delay(1000);

  Serial.print(out);

  Serial.print("$$$");
  delay(100);

  Serial.print("x\r");
  delay(300);

  while(Serial.available()) {
    out += (char) Serial.read();
  }

 Serial.print("---\r");
  delay(100);

  while (Serial.available()) {
    out += (char) Serial.read();
  }

  Serial.print("\nsaid: \"" + out + "\"");

}

void loop() {
  delay(1000);
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
}

/*

A program to send the RN-41 into 
command mode and retrieve some device
data. Device must be programmed while
RX/TX are disconnected.

*/
