#ifndef ROBOFEST2023_PID_H
#define ROBOFEST2023_PID_H

class PID{
    public: 
        int getGyroCorrection(int err);

        int getEncoderCorrection(int err);

        int getSonicCorrection(int err);

        int getWallCorrection(int err);

        int getDriveCorrection(int err);

        int getTurnCorrection(int err);
        
    private:
    
};

#endif //ROBOFEST2023_PID_H