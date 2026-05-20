#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Servo.h>

Servo servo1;
Servo servo2;
Servo servo3;

/*
Offset for servo 1 is around 20
Offset for servo 2 is around 10

With servo 3 and 4, we need to play with the speed and duration values

In order to extend the scooper, we need to go clockwise (ie. value from 0-90)
*/
const int SERVO1_PIN = 9;
const int SERVO2_PIN = 10;
const int SERVO3_PIN = 34;

//M4
const int dirA = 13;
const int pwmA = 11;  

//M2
const int dirB = 8;    
const int pwmB = 3;    

//M3 - Used to control servos
const int dirS = 12;    
const int pwmS = 5;   

const int buttonPin = 22;
const int buttonPinTop = 23;

const int driveSpeed = 250;
const int slowSpeed = 80;

int servo1Angle = 135;
int servo2Angle = 135;
int servo3Angle = 90;

void setup() {
  Serial.begin(115200);

  digitalWrite(dirS, HIGH);
  analogWrite(pwmS, 255);

  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);

  pinMode(dirA, OUTPUT);
  pinMode(pwmA, OUTPUT);

  pinMode(dirB, OUTPUT);
  pinMode(pwmB, OUTPUT);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buttonPinTop, INPUT_PULLUP);

  analogWrite(pwmA, 0);
  analogWrite(pwmB, 0);

  delay(500);

  setArmClosed();
  pickUpRocks();
}

void loop() {
  // put your main code here, to run repeatedly:
}

void setArmClosed() {
  setArmPosition(190, 210);
  delay(100);
}

void pickUpRocks() {
  setArmPosition(100, 100); // Change values (Base forward, middle 90)
  delay(100);
  extendScooper(45, 100);
  delay(100);
  // Code to open the scooper
  // openScooper(45, 100)
  setArmPosition(20, 210); // Change values (Base forward, middle forwards)
  extendScooper(135, 100);
  delay(300);
  // Code to close the scooper
  // openScooper(135, 100)
  delay(200);
  setArmClosed();  
}

void turnRight(long time) {
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, HIGH);

  analogWrite(pwmA, driveSpeed);
  analogWrite(pwmB, driveSpeed);

  delay(time);

  stopMotors();
}

void turnLeft(long time) {
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, LOW);

  analogWrite(pwmA, driveSpeed);
  analogWrite(pwmB, driveSpeed);

  delay(time);

  stopMotors();
}

void forward(long time) {
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, LOW);

  analogWrite(pwmA, driveSpeed);
  analogWrite(pwmB, driveSpeed);

  delay(time);

  stopMotors();
}

void backward(long time) {
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, HIGH);

  analogWrite(pwmA, driveSpeed);
  analogWrite(pwmB, driveSpeed);

  delay(time);

  stopMotors();
}

void stopMotors() {
  analogWrite(pwmA, 0);
  analogWrite(pwmB, 0); 

  delay(100);
}

void setArmPosition(int angle1, int angle2) {
  angle1 = constrain(angle1, 0, 270);
  angle2 = constrain(angle2, 0, 270);

  servo1.writeMicroseconds(map(angle1, 0, 270, 500, 2500));
  servo2.writeMicroseconds(map(angle2, 0, 270, 500, 2500));
}

void extendScooper(int speed, int duration) {
  servo3.writeMicroseconds(speed);
  delay(duration);
  servo3.writeMicroseconds(90);
}

/*
void openScooper(int speed, int duration) {
  servo3.write(speed);
  delay(duration);
  servo3.write(90);
}
*/