#define START_BAUD 112500
#define END_BAUD 9600

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Serial.begin(START_BAUD);
  delay(1000); // Let the module self-initialize
  Serial.print("AT+UART_DEF=");
  Serial.print(END_BAUD);
  Serial.print(",8,1,0,3\r\n");
  Serial.flush();
  digitalWrite(13, HIGH);
}

void loop() {
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  delay(1000);
}
