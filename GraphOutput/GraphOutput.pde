import processing.serial.*;  // add the serial library

int x = 0;
int deltaTime = 2;

Serial myPort; // define a serial port object to monitor
int screenX = 1024; // define screen dimensions
int screenY = 1024;
void setup() {
 size(1024, 1024); // set the window size
 println(Serial.list()); // list all available serial ports
 myPort = new Serial(this, Serial.list()[1], 9600); // define input port
 myPort.clear(); // clear the port of any initial junk
 fill(254, 0, 0); // pick the fill color (r,g,b)
}
void draw () {
 while (myPort.available () > 0) { // make sure port is open
 String inString = myPort.readStringUntil('\n'); // read input string
 if (inString != null) { // ignore null strings
 inString = trim(inString); // trim off any whitespace
String[] inputs = splitTokens(inString, ",");

if (inputs.length == 3) {
 x = x + deltaTime;
 int manualV = int(inputs[0]);
 int autoV = int(inputs[1]);
 int memsV = int(inputs[2]);
 //x = int((float)x*((float)screenX/1024.0)); // note the type conversions
 //autoV = int((float)autoV*((float)screenY/1024.0));
 //manualV = int((float)manualV*((float)screenY/1024.0));
 if (x >= 1024) {
  x = 0;
  stroke(0,0,0);
  background(128,128,128);
 }
 noStroke();
 
   fill(0, 254, 0);
ellipse(x, autoV, 3, 3); // draw a circle
 
 fill(0, 0, 254);
 ellipse(x, manualV, 3, 3); // draw a circle
 
 fill(254, 0, 0);
 ellipse(x, memsV, 3, 3); // draw a circle
 
}
 }
 }
}