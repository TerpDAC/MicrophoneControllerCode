#include <SoftwareSerial.h>

#define BAUD 9600

#define TXPIN 9
#define RXPIN 8

SoftwareSerial mySerial(RXPIN, TXPIN); /* RX:D3, TX:D2 */
String a;

void setup() {
  Serial.begin(9600);
  Serial.println("ESP8266 SOFTWARE SERIAL TESTER");
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  mySerial.begin(BAUD);
  delay(1000); // Let the module self-initialize
  digitalWrite(13, HIGH);
}

void loop() {
  digitalWrite(13, HIGH);
  mySerial.print("AT\r\n");
  mySerial.flush();
  delay(1000);
  
  a = mySerial.readString();// read the incoming data as string
  Serial.println("OUTPUT: " + a);
  
  digitalWrite(13, LOW);
  delay(1000);
}
