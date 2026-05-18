#include <Adafruit_MPU6050.h>

#include <Adafruit_Sensor.h>

#include <Wire.h>

#include <Servo.h>



Servo servo1;

Servo servo2;

Servo servo3;

//Servo servo3;

 

const int SERVO1_PIN = 9;

const int SERVO2_PIN = 10;

const int SERVO3_PIN = 34;

//const int SERVO3_PIN = 11;



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



Adafruit_MPU6050 mpu;

//VCC to 5V

//GND to GND

//SDA to I2C pin 20

//SCL to I2C pin 21



enum RobotPhase {

  APPROACHING,

  CLIMBING

};

RobotPhase currentPhase = APPROACHING;



const int driveSpeed = 250;

const int slowSpeed = 80;

const int correctionOffset = 40;



int servo1Angle = 135;

int servo2Angle = 135;

int servo3Angle = 90;



const float PITCH_IMPACT_THRESHOLD = 1.5; //rad/s --- if greater than this, then we have hit the ramp, has to be changed accordingly

const float ROLL_IMPACT_THRESHOLD = 0.8;  //rad/s --- if greater than this, we hit the ramp at an angle, has to be changed accordingly

const float ROLL_CORRECTION_TOLERANCE = 2.0; //degrees --- degrees of centre, has to be changed accordingly



const unsigned long TIME_FOR_90_DEG = 300; //Unsure about this

const float TIME_PER_CM = 50.0; //Unsure about this

const float RAMP_TIME_PER_CM = 70.0; //also unsure aabout this, this is higher cuz gravity



const float RAMP_LENGTH = 120.0;



// Hopper finding variables

//const int tolerance;



void setup() {
  
  Serial.begin(115200);

  digitalWrite(dirS, HIGH);
  analogWrite(pwmS, 255);

  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);
  
  /*

  if (!mpu.begin()) {

    Serial.println("Failed to find MPU6050.");

    while (1) { 

      delay(10);

      }

  }



  // Set sensor ranges

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);

  mpu.setGyroRange(MPU6050_RANGE_250_DEG);

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  */

  pinMode(dirA, OUTPUT);

  pinMode(pwmA, OUTPUT);

  

  pinMode(dirB, OUTPUT);

  pinMode(pwmB, OUTPUT);



  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buttonPinTop, INPUT_PULLUP);


  analogWrite(pwmA, 0);

  analogWrite(pwmB, 0);
  
  while (digitalRead(buttonPin) == HIGH){

  }
  

  forward(6000);
  delay(500);
  backward(6000);
  delay(500);


  
  turnRight(TIME_FOR_90_DEG);

  delay(500);

  /*
  bool hitObstacle = moveForwardUntilBump(50.0, driveSpeed);



  if (hitObstacle == false) {

    moveForwardUntilBump(10.0, slowSpeed);

  }
  */
  delay(300);
  setArmPosition(180, 180);
  delay(300);
  runHopper(100);
  delay(10000);
  stopHopper();

  while (digitalRead(buttonPinTop) == HIGH){

  }

  setArmPosition(180, 200);
  delay(300);
  setArmPosition(180, 180);
  
}



void loop() {

  // put your main code here, to run repeatedly:



}





void setMotorsContinuous(int speedA, int speedB) {

  digitalWrite(dirA, HIGH); 

  digitalWrite(dirB, LOW);

  

  analogWrite(pwmA, speedA);

  analogWrite(pwmB, speedB);

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



bool moveForwardUntilBump(float distance_cm, int speed) {

  digitalWrite(dirA, HIGH);

  digitalWrite(dirB, HIGH);

  

  analogWrite(pwmA, speed);

  analogWrite(pwmB, speed);



  unsigned long travelTime = TIME_PER_CM * distance_cm;

  unsigned long startTime = millis();



  while (millis() - startTime < travelTime) {

    

    if (digitalRead(buttonPin) == LOW) {

      stopMotors();

      return true;

    }

  }

  

  stopMotors();

  return false; 

}



void setArmPosition(int angle1, int angle2) {

  angle1 = constrain(angle1, 0, 270);

  angle2 = constrain(angle2, 0, 270);

  

  servo1.writeMicroseconds(map(angle1, 0, 270, 500, 2500));

  servo2.writeMicroseconds(map(angle2, 0, 270, 500, 2500));

}

void runHopper(int speedPercent) {
  // Constrain to prevent invalid inputs
  speedPercent = constrain(speedPercent, -100, 100);
  
  // Map the percentage (-100 to 100) to standard servo pulse widths (1000us to 2000us)
  // 1500 is the exact center (stop)
  int pulseWidth = map(speedPercent, -100, 100, 1000, 2000);
  
  servo3.writeMicroseconds(pulseWidth);
}

void stopHopper() {
  // Sending exactly 1500 microseconds tells a continuous servo to stop moving
  servo3.writeMicroseconds(1500); 
}