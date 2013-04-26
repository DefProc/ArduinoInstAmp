/*
  ReadScalesToDial
  
  Read a scales on analogPin and output to a dial on pwmPin
  */

// Global Variables
const int analogPin = A2;
const int pwmPin = 6;
int zeroPoint = 300; // Change the zero position of the range. default = 0
int maxValue = 255; // Maximum range value. default = 255
int sensorValue = 3;
int outputValue = 0;
unsigned long serialDelayCount = 1000;
unsigned long milliRef = 0;

// Set the size of the digital bracketing.
const int useBracketing = 1;
int positiveBracket = 5;
int negativeBracket = 2;
int currentSensorValue = -100.0;

void setup() {
  // Start the 
  Serial.begin(9600);
  delay(500);
  Serial.println("Arduino Weight Measurement");
  Serial.println("==========================");

  // Set an analogue voltage reference of the return from the V_{in} pin.
  // This means you use the full range of the Atmega's A2D converter.
  analogReference(EXTERNAL);
  
  // Initialise the dial to show it's working with a nice sweep.
  sweepDial(2, 2);
  delay(250);
}

void loop() {
  // Read the amplified output from the strain gauges.
  int sensorValue = analogRead(analogPin);
  
  // Stop the sensorValue being negative.
  if (sensorValue - zeroPoint <= 0) {
    sensorValue = 0;
  } else {
    sensorValue = sensorValue - zeroPoint;
  }
  
  // Apply bracketing as a noise filter.
  if (useBracketing == 1) {
    if (currentSensorValue - sensorValue > negativeBracket || sensorValue - currentSensorValue > positiveBracket) {
      currentSensorValue = sensorValue;
    }
  }
  
  // Convert the input range to an appropriate output range for display.
  // Analog input is 0-1023, Digital output is 0-255
  if (useBracketing == 1) {
    outputValue = currentSensorValue * 255.0 / (1023.0 - zeroPoint) * (255.0 / maxValue);
  } else {
    outputValue = sensorValue * 255.0 / (1023.0 - zeroPoint) * (255.0 / maxValue);
  }
  if (outputValue >= 255) outputValue = 255;
  
  // And write it to the pwmPin.
  analogWrite(pwmPin, outputValue);
  
  // Delayed serial output that doesn't interfere with the gauge refresh rate
  if (millis() >= milliRef + serialDelayCount) {
    Serial.print("sensorValue: ");
    Serial.print(sensorValue);
  
    Serial.print(" currentSensorValue: ");
    Serial.print(currentSensorValue);
    
    Serial.print(" outputValue: ");
    Serial.print(outputValue);
    
    Serial.println();
    
    milliRef = millis();
  }
  
}

void sweepDial(int sweepSpeed, int repeats) {
  for (int n = 1; n <= repeats; n++) {
    int x = 1;
    for (int i = 0; i > -1; i = i + x) {
        analogWrite(pwmPin, i);
        if (i == 255) {
          x = -1;             // switch direction at peak
          delay(250);
        }
        delay(sweepSpeed);
     }
     delay(250);
  }
}

