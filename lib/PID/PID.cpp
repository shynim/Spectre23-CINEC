#include <PID.h>
#include <Arduino.h>
#include <Variables.h>

int PID::getEncoderCorrection(int error){
    double p = error * eP;
    double d = (error - prevEncoderError) *eD;

    prevEncoderError = error;

    int correction = (int)(p + d);

    return correction;
}

int PID::getSonicCorrection(int error){
    double p = error * sP;
    double d = (error - prevSonicError) *sD;

    prevSonicError = error;

    int correction = (int)(p + d);

    return correction;
}

int PID::getWallCorrection(int error){
    double p = error * wP;
    double d = (error - prevWallError) *wD;

    prevWallError = error;

    int correction = (int)(p + d);

    return correction;
}

int PID::getDriveCorrection(int error){
    double p = error * dP;
    double d = (error - prevDriveError) * dD;

    prevDriveError = error;

    int correction = (int)(p + d);

    return correction;
}



