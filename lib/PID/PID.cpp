#include <PID.h>
#include <Arduino.h>
#include <Variables.h>

int PID::getGyroCorrection(int error){
    totalGyroError += error;

    double p = error * gP;
    double i = totalGyroError * gI;
    double d = (error - prevGyroError) * gD;

    prevGyroError = error;

    int correction = (int)(p + i + d);

    return correction;
}

int PID::getSonicCorrection(int error){
    totalSonicError += error;

    double p = error * sP;
    double i = totalSonicError *sI;
    double d = (error - prevSonicError) *sD;

    prevSonicError = error;

    int correction = (int)(p + i + d);

    return correction;
}

int PID::getDriveCorrection(int error){
    totalDriveError += error;

    double p = error * dP;
    double i = totalDriveError * dI;
    double d = (error - prevDriveError) * dD;

    prevDriveError = error;

    int correction = (int)(p + i + d);

    return correction;
}

int PID::getTurnCorrection(int error){
    totalTurnError += error;

    double p = error * tP;
    double i = totalTurnError * tI;
    double d = (error - prevTurnError) * tD;

    prevTurnError = error;
    int correction = (int)(p + i + d);

    return correction;
}


