#include <Arduino.h>
#include <Sonic.h>
#include <Variables.h>

int Sonic::readDistance(){
    delay(5);
    return sonic.ping_cm();
}

bool Sonic::wallFound(){
    int i = 10;
    int found = 0;

    while(i-- >= 0){
        int distance = readDistance();
        if(distance < 10 && distance != 0){
            found++;
        }else{
            found--;
        }
    }
    return found > 0 ? true:false;
}

