/* reads a MAX 4466 micrphone connected to 5V, GND, and A0
   averages the readings using an averaging window specified by numReadings
   and then outputs the value to serial
   
   copied/stolen by samim manizade
    */

int sensorPin = 0;    //Microphone Sensor Pin on analog 0
int sensorValue = A0; // microphone

//smoothing variables
const int numReadings = 15;
int readings[numReadings]; // the readings from the analog input
int readIndex = 0;         // the index of the current reading
int total = 0;             // the running total
int average = 0;           // the average



void setup() {
  
  Serial.begin(9600);
}

void loop() {
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(sensorPin);
  // add the reading to the total:
 total = total + readings[readIndex];
 // advance to the next position in the array:
  readIndex = readIndex + 1;
 
 // if we're at the end of the array...
  if(readIndex >= numReadings){
   // ..wrap around to the beginning
     readIndex = 0;
  } 
  
  // calculate the average:
  average = total/numReadings;
  //send it over serial
  Serial.println(average-509); // 339 appears to be a baseline for 3.3
  delay(1);   
}   
