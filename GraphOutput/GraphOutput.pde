import processing.serial.*;  // add the serial library

int x = 0;
int deltaTime = 1;

Serial myPort; // define a serial port object to monitor
int screenX = 1024; // define screen dimensions
int screenY = 1024;
void setup() {
 size(1024, 1024); // set the window size
 println(Serial.list()); // list all available serial ports
 myPort = new Serial(this, Serial.list()[0], 9600); // define input port
 myPort.clear(); // clear the port of any initial junk
 fill(254, 0, 0); // pick the fill color (r,g,b)
 background(255,255,255);
}
void draw () {
 while (myPort.available () > 0) { // make sure port is open
 String inString = myPort.readStringUntil('\n'); // read input string
 if (inString != null) { // ignore null strings
 inString = trim(inString); // trim off any whitespace
String[] inputs = splitTokens(inString, ",");

if (inputs.length == 1) {
 x = x + deltaTime;
 int manualV = int(inputs[0]) - 335;
 manualV = 10 * abs(manualV);
 //int autoV = int(inputs[1]);
 //int memsV = int(inputs[2]);

 if (x >= 1024) {
  x = 0;
  stroke(0,0,0);
  background(255,255,255);
 }
 noStroke();
 
//   fill(0, 254, 0);
//ellipse(x, autoV, 3, 3); // draw a circle
 
 fill(0, 0, 254);
 ellipse(x, manualV, 3, 3); // draw a circle
 
// fill(254, 0, 0);
 //ellipse(x, memsV, 3, 3); // draw a circle
 
}
 }
 }
}