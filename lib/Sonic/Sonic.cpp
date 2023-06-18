#include <Arduino.h>
#include <Sonic.h>
#include <Variables.h>

int Sonic::readDistance(){
    delay(5);
    return sonic.ping_cm();
}

bool Sonic::wallFound(){
    int distance = readDistance();
    return distance < sideGap ? true:false;
}