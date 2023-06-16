#ifndef ROBOFEST2023_SONIC_H
#define ROBOFEST2023_SONIC_H

#include <NewPing.h>

class Sonic{
    public:
        Sonic(int trig, int echo, int maxDistance): sonic(trig, echo, maxDistance){}

        bool wallFound();

        int readDistance();

    private:
        NewPing sonic;
        
};

#endif //ROBOFEST2023_SONIC_H
