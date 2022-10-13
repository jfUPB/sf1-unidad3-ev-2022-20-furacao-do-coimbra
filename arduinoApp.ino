#include "task1.h"
#include "task2.h"
#include "task3.h"
#include "task4.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;
float roll, pitch,yaw;
float x, y, z;
float gx, gy, gz;

void setup(void) {
  
  Serial.begin(115200);
  while (!Serial)
    delay(10);
  Serial.println("Adafruit MPU6050 test!");
  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  Serial.println("MPU6050 Found!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: RANGE_8_G");
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: RANGE_500_DEG");
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: BAND_5_HZ");
}

void loop() {
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;

  mpu.getEvent(&a, &g, &temp);

  roll = 180 *atan(a.acceleration.x/sqrt(a.acceleration.y*a.acceleration.y+a.acceleration.z*a.acceleration.z))/M_PI;
  pitch = 180 *atan(a.acceleration.y/sqrt(a.acceleration.x*a.acceleration.x+a.acceleration.z*a.acceleration.z))/M_PI;
  //Calcule with yaw
  yaw = 180 *atan(a.acceleration.z/sqrt(a.acceleration.y*a.acceleration.y+a.acceleration.z*a.acceleration.z))/M_PI;
  yaw = yaw - 45;
  //Z acceleration
  z = a.acceleration.z;
  //Z gyro
  gz = g.gyro.z;

  if(Serial.available() > 0)
  {
    if(Serial.read() == 's')
    {
      Serial.write((u_int8_t*)&roll,4);
      Serial.write((u_int8_t*)&pitch,4);
      //Rotation in z axis cannot be calcule with Acceleration values
      //Program would be correct to use Quaternions in the near future to solve this issue
      Serial.write((u_int8_t*)&gz,4);
    }        
  }    
}