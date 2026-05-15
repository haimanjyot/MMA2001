#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;
//VCC to 5V
//GND to GND
//SDA to I2C pin 20
//SCL to I2C pin 21

void setup() {
  Serial.println("starting");
  Serial.begin(115200);
  

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050.");
    while (1) { 
      delay(10);
      }
  }

  Serial.println("connected");

  // Set sensor ranges
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  

}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.println(abs(g.gyro.y));
  Serial.println(abs(g.gyro.y));

  float absoluteRoll = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;
  Serial.println(absoluteRoll);

}
