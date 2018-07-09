void setup() {
  String out = "";

  //Serial.begin(115200);  // Begin the serial monitor at 115200bps
  Serial.begin(9600);  // Begin the serial monitor at 9600bps
  delay(1000);

  Serial.print(out);

  Serial.print("$$$");
  delay(100);

  //Serial.print("SU,96\r");  // changes baud rate
  Serial.print("D\r");
  delay(100);

  while(Serial.available()) {
    out += (char) Serial.read();
  }

/* (not used to change baud rate)
  Serial.flush();
  Serial.begin(9600);
  delay(100);

  Serial.print("x\r");
  delay(100);
  Serial.print("$$$\r");
  delay(100);
  Serial.print("d\r");
  delay(100);

  while(Serial.available()) {
    out += (char) Serial.read();
  }
*/
  Serial.print("\nsaid: \"" + out + "\"");
}

void loop() {
  delay(1000);
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
}

/*

This program was used to change the
baud rate of the RN-41 to 9600.
This program currently just tests in
9600, the commands to change to
9600 are commented out.
*/

