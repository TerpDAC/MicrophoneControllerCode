void setup() {
  Serial.begin(9600);
}

int smax = 0;
int cmax = 0;
int samp = 0;
float avg = 0.0;

bool datasend = false;

void loop() {
  if (!datasend) {
    Serial.println("TERPDAC");
    if (Serial.find("GOGOGO")) {
      datasend = true;
    }
  } else {
    cmax = analogRead(0);
    if (cmax>smax) smax = cmax;
    samp += 1;
    //int autoValue = analogRead(2);
    //int memsValue = analogRead(4);
  
    //String output = String(manualValue);// + "," + String(autoValue) + "," + String(memsValue);
    //Serial.println(output);
    
    if (samp == 10000) {
      Serial.println(smax);
      smax = 0;
      samp = 0;
    }
  }
}
