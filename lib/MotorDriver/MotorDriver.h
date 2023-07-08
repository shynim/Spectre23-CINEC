#ifndef ROBOFEST2023_MOTORDRIVER_H
#define ROBOFEST2023_MOTORDRIVER_H

class MotorDriver{
    public:
        void init(int* leftPins,int *rightPins);

        void forward(int speed);
        
        void backward(int speed);

        void turnLeft(int leftSpeed, int rightSpeed);
        
        void turnRight(int leftSpeed, int rightSpeed);

        void reverseRight(int speed);

        void reverseLeft(int speed);

        void forward(int leftSpeed, int rightSpeed);

        void backward(int leftSpeed, int rightSpeed);

        void stop();

        void brake();

        void applyEncoderPid(int correction);

        void applyWallPid(int correction);

        void applySonicPid(int correction);

        void applySonicDrivePid(int correction);
        
        
    private:
        int leftPWM;
        int rightPWM;
        int leftDirection[2];
        int rightDirection[2];

};

#endif //ROBOFEST2023_MOTORDRIVER_H