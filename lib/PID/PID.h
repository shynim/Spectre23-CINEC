#ifndef ROBOFEST2023_PID_H
#define ROBOFEST2023_PID_H

class PID{
    public: 
        int getGyroCorrection(int err);

        int getSonicCorrection(int err);

        int getDriveCorrection(int err);

        int getTurnCorrection(int err);
        
    private:
        int totalGyroError = 0;        
        int prevGyroError = 0;
                
        int totalSonicError = 0;
        int prevSonicError = 0;
                    
        int totalDriveError = 0;
        int prevDriveError = 0;
                    
        int totalTurnError = 0;
        int prevTurnError = 0;
    
};

#endif //ROBOFEST2023_PID_H
