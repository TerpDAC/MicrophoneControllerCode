#include <ESP8266.h>
#include <doxygen.h>

#include <SoftwareSerial.h>
#include "WiFiConfig.h"

#define TXPIN 9
#define RXPIN 8

SoftwareSerial mySerial(RXPIN, TXPIN); /* RX:D3, TX:D2 */
ESP8266 wifi(mySerial, 9600);

#define HOST_NAME   "www.example.com"
#define HOST_PORT   (80)

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.print("setup begin\r\n");

    Serial.print("Kicking ESP8266...\r\n");

    if (wifi.kick()) {
      Serial.println("It's on!");
    } else {
      Serial.println("It's OFF!");
    }
    
    Serial.print("FW Version:");
    Serial.println(wifi.getVersion().c_str());

    if (wifi.setOprToStationSoftAP()) {
        Serial.print("to station + softap ok\r\n");
    } else {
        Serial.print("to station + softap err\r\n");
    }

    if (wifi.joinAP(ssid, password)) {
        Serial.print("Join AP success\r\n");

        Serial.print("IP:");
        Serial.println( wifi.getLocalIP().c_str());       
    } else {
        Serial.print("Join AP failure\r\n");
    }
    
    if (wifi.disableMUX()) {
        Serial.print("single ok\r\n");
    } else {
        Serial.print("single err\r\n");
    }
    
    Serial.print("setup end\r\n");
}

void loop() {
    uint8_t buffer[1024] = {0};

    if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
        Serial.print("create tcp ok\r\n");
    } else {
        Serial.print("create tcp err\r\n");
    }

    char *hello = "GET / HTTP/1.1\r\nHost: www.example.com\r\nConnection: close\r\n\r\n";
    wifi.send((const uint8_t*)hello, strlen(hello));

    uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
    }

    if (wifi.releaseTCP()) {
        Serial.print("release tcp ok\r\n");
    } else {
        Serial.print("release tcp err\r\n");
    }
    
    while(1);
}
