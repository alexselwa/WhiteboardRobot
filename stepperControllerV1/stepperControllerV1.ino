#include <stdio.h> // for function sprintf
#include <math.h>

int pin_stepR = 13;
int pin_dirR = 12;
int pin_stepL = 1;
int pin_dirL = 0;

int pin_speedR1 = 9;
int pin_speedR2 = 10;
int pin_speedR3 = 11;
int pin_speedL1 = 5;
int pin_speedL2 = 6;
int pin_speedL3 = 7;

double SPEED = 0.00015; // Desired speed in mm/us
double RADIUS = 15; // Estimate of spool radius in mm
double WIDTH = 1840; // Estimate of whiteboard width in mm
double HEIGHT = 1000; // Estimate of whiteboard height in mm
float ANGLE = 1.8*(M_PI/180); // Angle for one full step in rads
float stepFraction;

// the setup routine runs once when you press reset:
void setup() {                

  // initialize pins as outputs and initialize direction bits
  pinMode(pin_stepL, OUTPUT);  
  pinMode(pin_dirL, OUTPUT);
  pinMode(pin_stepR, OUTPUT);
  pinMode(pin_dirR, OUTPUT);     
  digitalWrite(pin_dirL, LOW); // Shorter
  digitalWrite(pin_dirR, LOW); // shorter
  
  // set speed to 16th steps
  pinMode(pin_speedR1, OUTPUT);
  pinMode(pin_speedR2, OUTPUT);
  pinMode(pin_speedR3, OUTPUT);
  
  pinMode(pin_speedL1, OUTPUT);
  pinMode(pin_speedL2, OUTPUT);
  pinMode(pin_speedL3, OUTPUT);
  
  digitalWrite(pin_speedR1, HIGH); 
  digitalWrite(pin_speedR2, HIGH);  
  digitalWrite(pin_speedR3, HIGH);
  
  digitalWrite(pin_speedL1, HIGH);  
  digitalWrite(pin_speedL2, HIGH);  
  digitalWrite(pin_speedL3, HIGH);
  
  stepFraction = 16;
}

// Main loop
void loop() {
  
  segment(900, 1200, 1000, 1000);
  segment(1200, 1200, 1000, 800);
  segment(1200, 900, 800, 800);
  segment(900, 900, 800, 1000);
  delay(10000);

}

// Function that moves the motors for one segment
void segment(double x1, double x2, double y1, double y2) {
  
  // Initialize counter to know when segment is done
  unsigned long stepsDoneL = 0;
  unsigned long stepsDoneR = 0;
  
  // Figure out lengths
  double leftLength1 = sqrt(x1*x1+y1*y1); // in mm
  double leftLength2 = sqrt(x2*x2+y2*y2);
  double rightLength1 = sqrt((WIDTH-x1)*(WIDTH-x1)+y1*y1);
  double rightLength2 = sqrt((WIDTH-x2)*(WIDTH-x2)+y2*y2);
  double deltaLeft = leftLength2 - leftLength1;
  double deltaRight = rightLength2 - rightLength1;
  
  // Set motor speeds (dTime)
  double distance = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)); // distance of segment in x-y plane
  double time = distance/SPEED; // in us: time it should take each motor to complete the segment
  double speedLeft = fabs(deltaLeft/time); // in mm/us: speed in length of wire
  double speedRight = fabs(deltaRight/time);
  // delay time is (step_angle*radius)/speed
  unsigned long dTimeL = ((ANGLE/stepFraction)*(RADIUS))/speedLeft;
  unsigned long dTimeR = ((ANGLE/stepFraction)*(RADIUS))/speedRight;
  
  // Figure out total number of steps to do so we know when to stop
  unsigned long stepL = fabs(deltaLeft/((ANGLE/stepFraction)*RADIUS));
  unsigned long stepR = fabs(deltaRight/((ANGLE/stepFraction)*RADIUS));
  
  // Set direction bits
  if (deltaLeft < 0) {
    digitalWrite(pin_dirL, LOW); //shorter
  } else {
    digitalWrite(pin_dirL, HIGH); //longer
  }
  if (deltaRight < 0) {
    digitalWrite(pin_dirR, LOW); //shorter
  } else {
    digitalWrite(pin_dirR, HIGH); //longer
  }

  // Start the marked times for each of the motors
  unsigned long markedTimeL = micros();
  unsigned long markedTimeR = micros();
  
  // Loop and keep stepping until the proper number of steps has been done
  while ((stepsDoneL <= stepL) && (stepsDoneR <= stepR))  {  
     
    // LEFT MOTOR
    unsigned long testTime = micros();
    if (testTime > markedTimeL+dTimeL) {
      // Step the motor
      digitalWrite(pin_stepL, HIGH);
      delayMicroseconds(20);  
      digitalWrite(pin_stepL, LOW); 
      
      // Increase steps done and reset the time placeholder for the left motor
      stepsDoneL++;
      markedTimeL = micros();
      }
     
     // RIGHT MOTOR
     if (micros() > markedTimeR+dTimeR) {
       // Step the motor
      digitalWrite(pin_stepR, HIGH);
      delayMicroseconds(20);  
      digitalWrite(pin_stepR, LOW); 
      
      // Increase steps done and reset the time placeholder for the right motor
      stepsDoneR++;
      markedTimeR = micros();      
     }
  } 
}
