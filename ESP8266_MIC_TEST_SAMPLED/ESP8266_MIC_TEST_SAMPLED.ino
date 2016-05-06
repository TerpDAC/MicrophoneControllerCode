#define micTotal 1000

int micValue = 0;
int micPeak = 0;
int micCount = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  micValue = analogRead(A0);

  if (micCount == micTotal) {
    Serial.println(micPeak);
    micPeak = 0;
    micCount = 0;
  } else {
    if (micValue > micPeak) {
      micPeak = micValue;
    }
    micCount++;
  }
}
