#ifndef ROBOFEST2023_VARIABLES_H
#define ROBOFEST2023_VARIABLES_H

//floodFill
#define mazeSize 6

extern int grid[mazeSize][mazeSize];
extern int verticalWallGrid[mazeSize][mazeSize+1];
extern int horizontalWallGrid[mazeSize+1][mazeSize];

//motorDriver
extern const int leftPins[];
extern const int rightPins[];

extern int stopDelay;

//Speeds
extern int gyroBase;
extern int gyroMax;

extern int sonicBase;
extern int sonicMax; 
extern int sonicMin;

extern int turnMax;
extern int turnBase;

extern int driveMax;
extern int driveBase; 

//ultraSonic
extern const int leftSonicPins[]; //{trig,echo}
extern const int frontSonicPins[];
extern const int rightSonicPins[];

//PID
extern const double gP ;
extern const double gI;    
extern const double gD;
   
extern const double sI;    
extern const double sD;
extern const double sP;

extern const double dP;
extern const double dI;    
extern const double dD;
 
extern const double tP;
extern const double tI;    
extern const double tD;
 
extern const int cellDistance; //cm
extern const int sideGap;
extern const int frontGap;

extern const int maxDistance;

extern char orientation[4];
extern int orientationKey;

extern int setTime;

#endif //ROBOFEST2023_VARIABLES_H
