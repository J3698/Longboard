void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    Serial.write(Serial.read());
  }
}

/*

Only works if stuff isn't on RX/TX. If there's
stuff pulling RX a certain way, RX will be
pulled that way.

*/
