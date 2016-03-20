#define BAUD 112500

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Serial.begin(BAUD);
  delay(1000); // Let the module self-initialize
  Serial.print("AT\r\n");
  Serial.flush();
  digitalWrite(13, HIGH);
}

void loop() {
  digitalWrite(13, HIGH);
  delay(1000);
  
  Serial.print("AT\r\n");
  Serial.flush();
  delay(1000);
  
  Serial.readString();// read the incoming data as string
  
  digitalWrite(13, LOW);
  delay(1000);
}
