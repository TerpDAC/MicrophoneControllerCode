int micValue = 0;
int maxAbs = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  micValue = analogRead(A0);
  //Serial.print(micValue);
  //Serial.print(" ");
  micValue = abs(micValue - 512);
  Serial.println(micValue);
  if (micValue > maxAbs) {
    maxAbs = micValue;
    //Serial.println(maxAbs);
  }
}
