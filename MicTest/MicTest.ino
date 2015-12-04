void setup() {
  Serial.begin(9600);

}

void loop() {
  delayMicroseconds(50);

  int manualValue = analogRead(0);
  //int autoValue = analogRead(2);
  //int memsValue = analogRead(4);

  String output = String(manualValue);// + "," + String(autoValue) + "," + String(memsValue);
  Serial.println(output);

}
