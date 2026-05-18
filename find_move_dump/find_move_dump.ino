
//hardcoded finding and moving to the dump zone and then stopping using just functions and setup.
//need to code finding and collecting rocks
//need to hardcode moving back in front of the dumpzone, for which the accurate distance value we will get from how far the dump zone was.
//then use the navigateramp and the continousmotor function to get up the ramp, a certain distance, need to  find the ramp distance and angles and stuff, and speed on ramp as well
//code dropping stuff in and everything else

//maybe write a function for the correctiooffset, but am also hardcoding it

//4 servos, 2 for the arm, 1 for the claw, and 1 for the dumping mechanism.

// Figure out servo pins

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Servo.h>

Servo armServo1;
Servo armServo2;
Servo clawServo;
Servo dumpingServo;

//M1
const int dirA = 7;
const int pwmA = 6;
//const int brakeA = 9;

//M1
const int dirB = 8;
const int pwmB = 3;
//const int brakeB = 8;

//Servos
const int armServo1Pin = 2;
const int armServo2Pin = 3;
const int clawServoPin = 4;
const int dumpingServoPin = 5;

const int buttonPin = 22;

// Ultrasonic
const int FrontTrigPin = 9;
const int FrontEchoPin = 10;
const int LeftTrigPin = 11;
const int LeftEchoPin = 12;

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

const float PITCH_IMPACT_THRESHOLD = 1.5;     //rad/s --- if greater than this, then we have hit the ramp, has to be changed accordingly
const float ROLL_IMPACT_THRESHOLD = 0.8;      //rad/s --- if greater than this, we hit the ramp at an angle, has to be changed accordingly
const float ROLL_CORRECTION_TOLERANCE = 2.0;  //degrees --- degrees of centre, has to be changed accordingly

const unsigned long TIME_FOR_90_DEG = 600;  //Unsure about this
const float TIME_PER_CM = 50.0;             //Unsure about this
const float RAMP_TIME_PER_CM = 70.0;        //also unsure aabout this, this is higher cuz gravity

const float RAMP_LENGTH = 100.0;  //unsure about this

const int driveSpeed = 150;
const int slowSpeed = 80;
const int correctionOffset = 40;

// Hopper finding variables
float duration = 0;
float distance = 0;
const int tolerance;

void setup() {
  pinMode(dirA, OUTPUT);
  pinMode(pwmA, OUTPUT);

  pinMode(dirB, OUTPUT);
  pinMode(pwmB, OUTPUT);

  pinMode(buttonPin, INPUT_PULLUP);

  armServo1.attach(armServo1Pin);
  armServo2.attach(armServo2Pin);
  clawServo.attach(clawServoPin);
  dumpingServo.attach(dumpingServoPin);

  // Ultrasonic pins
  pinMode(LeftTrigPin, INPUT);
  pinMode(LeftEchoPin, OUTPUT);
  pinMode(FrontTrigPin, INPUT);
  pinMode(FrontEchoPin, OUTPUT);

  digitalWrite(LeftTrigPin, LOW);
  digitalWrite(FrontTrigPin, LOW);

  Serial.begin(115200);

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

  //digitalWrite(brakeA, LOW);
  //digitalWrite(brakeB, LOW);
  analogWrite(pwmA, 0);
  analogWrite(pwmB, 0);

  delay(2000);

  turnRight(TIME_FOR_90_DEG);
  delay(500);

  bool hitObstacle = moveForwardUntilBump(50.0, driveSpeed);

  if (hitObstacle == false) {
    moveForwardUntilBump(10.0, slowSpeed);
  }

  pickupRocks();

  //add code to move back in front of the ramp, using the distance to the dumpzone that we will get from the finding rocks code, for now just hardcode it and turn left, ie face ramp
  backward(1000);  //calculate this time using the distance to the dumpzone
  turnLeft(TIME_FOR_90_DEG);


  navigateRamp(RAMP_LENGTH);
}

void loop() {
}


void pickupRocks() {         // initial position is all arms are folded down, ie 0deg, and the claw is open
  doServoStuff(1, 60, 500);  //tilt arm up, change values as needed
  doServoStuff(2, 30, 500);  //extend arm out, change values as needed
  doServoStuff(3, 20, 500);  //close claw, change values
}

void doServoStuff(int servoNo, float degree, int delayTime) {  // Equating it to 0 means the deafult delay is 0, remove if it stops working
  if (servoNo == 1) {
    armServo1.write(degree);
  } else if (servoNo == 2) {
    armServo2.write(degree);
  } else if (servoNo == 3) {
    clawServo.write(degree);
  } else if (servoNo == 4) {
    dumpingServo.write(degree);
  }

  delay(delayTime);
}

void navigateRamp(float rampDistance_cm) {
  sensors_event_t a, g, temp;
  bool rampDetected = false;

  //drive straight until ramp detected
  while (!rampDetected) {
    mpu.getEvent(&a, &g, &temp);

    setMotorsContinuous(driveSpeed, driveSpeed);

    if (abs(g.gyro.y) > PITCH_IMPACT_THRESHOLD) {
      //hit the ramp, no need to do anything here

      if (abs(g.gyro.x) > ROLL_IMPACT_THRESHOLD) {
        //hit the ramp at angle, do some corrections
      }

      rampDetected = true;
      delay(300);
    }
    delay(20);
  }

  //climbing now

  unsigned long travelTime = RAMP_TIME_PER_CM * rampDistance_cm;
  unsigned long startTime = millis();

  while (millis() - startTime < travelTime) {
    mpu.getEvent(&a, &g, &temp);
    float absoluteRoll = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;  //THIS DEPEDNS ON HOW WE MOUNT THE SENSOR

    if (absoluteRoll > ROLL_CORRECTION_TOLERANCE) {
      setMotorsContinuous(driveSpeed - correctionOffset, driveSpeed + correctionOffset);
    } else if (absoluteRoll < -ROLL_CORRECTION_TOLERANCE) {
      setMotorsContinuous(driveSpeed + correctionOffset, driveSpeed - correctionOffset);
    } else {
      setMotorsContinuous(driveSpeed, driveSpeed);
    }

    delay(20);
  }

  stopMotors();
}

//this one is used in the loop becuase this has no delays
void setMotorsContinuous(int speedA, int speedB) {
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, HIGH);

  analogWrite(pwmA, speedA);
  analogWrite(pwmB, speedB);
}

void turnRight(long time) {
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, LOW);

  analogWrite(pwmA, driveSpeed);
  analogWrite(pwmB, driveSpeed);

  delay(time);

  stopMotors();
}

void turnLeft(long time) {
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, HIGH);

  analogWrite(pwmA, driveSpeed);
  analogWrite(pwmB, driveSpeed);

  delay(time);

  stopMotors();
}

void forward(long time) {
  digitalWrite(dirA, HIGH);
  digitalWrite(dirB, HIGH);

  analogWrite(pwmA, driveSpeed);
  analogWrite(pwmB, driveSpeed);

  delay(time);

  stopMotors();
}

void backward(long time) {
  digitalWrite(dirA, LOW);
  digitalWrite(dirB, LOW);

  analogWrite(pwmA, driveSpeed);
  analogWrite(pwmB, driveSpeed);

  delay(time);

  stopMotors();
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

void stopMotors() {
  analogWrite(pwmA, 0);
  analogWrite(pwmB, 0);

  delay(100);
}

void get_hopper_centre() {
  // Distance variables
  int front_dist_raw = 0;
  int left_dist_raw = 0;
  int front_dist = 0;
  int left_dist = 0;

  int min_left_dist = 5;
  int min_front_dist = 5;

  bool left_centred = false;
  bool front_centred = false;

  // Cycle through until it's centred
  while (true) {
    // Get distance
    digitalWrite(LeftTrigPin, HIGH);
    digitalWrite(FrontTrigPin, HIGH);
    delay(10);
    digitalWrite(LeftTrigPin, LOW);
    digitalWrite(FrontTrigPin, LOW);

    // Process (Maybe add smoothing?)
    front_dist_raw = pulseIn(FrontTrigPin, HIGH);
    front_dist = (front_dist_raw * 0.034) / 2;

    left_dist_raw = pulseIn(LeftTrigPin, HIGH);
    left_dist = (left_dist_raw * 0.034) / 2;

    // Check front sensor distance
    if (front_dist >= (min_front_dist + tolerance)) {
      Serial.println("Moving forwards");
    } else if (front_dist <= (min_front_dist - tolerance)) {
      Serial.println("Moving backwards");
    } else {
      Serial.println("Y axis centred");
      front_centred = !front_centred;
    }

    // Check left sensor distance
    if (left_dist >= (min_left_dist + tolerance)) {
      Serial.println("Moving right");
    } else if (left_dist <= (min_left_dist - tolerance)) {
      Serial.println("Moving left");
    } else {
      Serial.println("x axis centred");
      left_centred = !left_centred;
    }
  }
}