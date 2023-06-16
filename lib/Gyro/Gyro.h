#ifndef ROBOFEST2023_GYRO_H
#define ROBOFEST2023_GYRO_H

#include "Arduino.h"
#include "MPU6050_tockn.h"
#include "Wire.h"

MPU6050 mpu6050(Wire);

void setupGyro() {
    Wire.begin();
    Serial.println("starting");
    mpu6050.begin();
    Serial.println("started");
    mpu6050.calcGyroOffsets(true);
}

double getAngle() {
    mpu6050.update();
    return mpu6050.getGyroAngleZ();
}

#endif //ROBOFEST2023_GYRO_H