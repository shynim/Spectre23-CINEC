#include <MotorDriver.h>
#include <Arduino.h>
#include <Variables.h>

void MotorDriver::init(int* leftPins,int* rightPins){
    leftPWM = leftPins[0];
    leftDirection[0] = leftPins[1];
    leftDirection[1] = leftPins[2];

    rightPWM = rightPins[0];
    rightDirection[0] = rightPins[1];
    rightDirection[1] = rightPins[2];

    pinMode(leftPWM, OUTPUT);
    pinMode(rightPWM, OUTPUT);
    pinMode(leftDirection[0], OUTPUT);
    pinMode(leftDirection[1], OUTPUT);
    pinMode(rightDirection[0], OUTPUT);
    pinMode(rightDirection[1],OUTPUT);
}

void MotorDriver::forward(int speed) {
    digitalWrite(leftDirection[0], HIGH);
    digitalWrite(leftDirection[1], LOW);
    digitalWrite(rightDirection[0], HIGH);
    digitalWrite(rightDirection[1], LOW);

    analogWrite(leftPWM, speed);
    analogWrite(rightPWM, speed);
}

void MotorDriver::stop() {
    digitalWrite(leftDirection[0], HIGH);
    digitalWrite(leftDirection[1], HIGH);
    digitalWrite(rightDirection[0], HIGH);
    digitalWrite(rightDirection[1], HIGH);

    analogWrite(leftPWM, 255);
    analogWrite(rightPWM, 255);
    delay(stopDelay);
}

void MotorDriver::backward(int speed) {
    digitalWrite(leftDirection[0], LOW);
    digitalWrite(leftDirection[1], HIGH);
    digitalWrite(rightDirection[0], LOW);
    digitalWrite(rightDirection[1], HIGH);

    analogWrite(leftPWM, speed);
    analogWrite(rightPWM, speed);
}

void MotorDriver::turnLeft(int speed) {
    digitalWrite(leftDirection[0], LOW);
    digitalWrite(leftDirection[1], HIGH);
    digitalWrite(rightDirection[0], HIGH);
    digitalWrite(rightDirection[1], LOW);

    analogWrite(leftPWM, speed);
    analogWrite(rightPWM, speed);
}

void MotorDriver::turnRight(int speed) {
    digitalWrite(leftDirection[0], HIGH);
    digitalWrite(leftDirection[1], LOW);
    digitalWrite(rightDirection[0], LOW);
    digitalWrite(rightDirection[1], HIGH);

    analogWrite(leftPWM, speed);
    analogWrite(rightPWM, speed);
}
void MotorDriver::reverseRight(int speed) {
    digitalWrite(leftDirection[0], LOW);
    digitalWrite(leftDirection[1], LOW);
    digitalWrite(rightDirection[0], LOW);
    digitalWrite(rightDirection[1], HIGH);

    analogWrite(leftPWM, speed);
    analogWrite(rightPWM, speed);
}

void MotorDriver::reverseLeft(int speed) {
    digitalWrite(leftDirection[0], LOW);
    digitalWrite(leftDirection[1], HIGH);
    digitalWrite(rightDirection[0], LOW);
    digitalWrite(rightDirection[1], LOW);

    analogWrite(leftPWM, speed);
    analogWrite(rightPWM, speed);
}

void MotorDriver::forward(int leftSpeed, int rightSpeed) {
    digitalWrite(leftDirection[0], HIGH);
    digitalWrite(leftDirection[1], LOW);
    digitalWrite(rightDirection[0], HIGH);
    digitalWrite(rightDirection[1], LOW);

    analogWrite(leftPWM, leftSpeed);
    analogWrite(rightPWM, rightSpeed);
}

void MotorDriver::backward(int leftSpeed, int rightSpeed) {
    digitalWrite(leftDirection[0], LOW);
    digitalWrite(leftDirection[1], HIGH);
    digitalWrite(rightDirection[0], LOW);
    digitalWrite(rightDirection[1], HIGH);

    analogWrite(leftPWM, leftSpeed);
    analogWrite(rightPWM, rightSpeed);
}

void MotorDriver::applyGyroPid(int correction){
    int leftSpeed = abs(gyroBase) + correction;
    int rightSpeed = abs(gyroBase) - correction;

    if (leftSpeed < 0) {
        leftSpeed = 0;
    }

    if (rightSpeed < 0) {
        rightSpeed = 0;
    }

    if (leftSpeed >= gyroMax) {
        leftSpeed = gyroMax;
    }

    if (rightSpeed >= gyroMax) {
        rightSpeed = gyroMax;
    }

    gyroBase >= 0 ? forward(leftSpeed, rightSpeed):backward(rightSpeed, leftSpeed);

}

void MotorDriver::applyGyroTurnPid(int correction){
    if(correction >= turnMax){
        correction = turnMax;
    }

    if(correction <= turnBase && correction >= 0){
        correction = turnBase;
    }

    if(correction <= -turnMax){
        correction = -turnMax;
    }

    if(correction >= -turnBase && correction <= 0){
        correction = -turnBase;
    }

    if(correction >= 0){
        turnRight(correction);
    }else{
        turnLeft(-correction);
    }
    
}

void MotorDriver::applySonicDrivePid(int correction){
    if(correction >= driveMax){
        correction = driveMax;
    }
    if(correction <= -driveMax){
        correction = -driveMax;
    }
    
    if(correction <= driveBase && correction >= 0){
        correction = driveBase;
    }
    if(correction >= -driveBase && correction <= 0){
        correction = -driveBase;
    }
    sonicBase = correction;
}

void MotorDriver::applySonicPid(int correction){
    int leftSpeed = abs(sonicBase) + correction;
    int rightSpeed = abs(sonicBase) - correction;

    if (leftSpeed < sonicMin) {
        leftSpeed = sonicMin;
    }

    if (rightSpeed < sonicMin) {
        rightSpeed = sonicMin;
    }

    if (leftSpeed >= sonicMax) {
        leftSpeed = sonicMax;
    }

    if (rightSpeed >= sonicMax) {
        rightSpeed = sonicMax;
    }

    sonicBase >= 0 ? forward(leftSpeed, rightSpeed):backward(rightSpeed, leftSpeed);
}
