#ifndef ROBOFEST2023_MOTORDRIVER_H
#define ROBOFEST2023_MOTORDRIVER_H

class MotorDriver{
    public:
        void init(int* leftPins,int *rightPins);

        void forward(int speed);
        
        void backward(int speed);

        void turnLeft(int speed);
        
        void turnRight(int speed);

        void reverseRight(int speed);

        void reverseLeft(int speed);

        void forward(int leftSpeed, int rightSpeed);

        void backward(int leftSpeed, int rightSpeed);

        void stop();

        void applySonicDrivePid(int correction);

        void applyGyroPid(int correction);

        void applyGyroTurnPid(int correction);

        void applyWallPid(int correction);

        void applySonicPid(int correction);
        
    private:
        int leftPWM;
        int rightPWM;
        int leftDirection[2];
        int rightDirection[2];

};

#endif //ROBOFEST2023_MOTORDRIVER_H