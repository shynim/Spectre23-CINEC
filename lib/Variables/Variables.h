#ifndef ROBOFEST2023_VARIABLES_H
#define ROBOFEST2023_VARIABLES_H

//floodFill
#define mazeSize 6

extern int grid[mazeSize][mazeSize];
extern int verticalWallGrid[mazeSize][mazeSize+1];
extern int horizontalWallGrid[mazeSize+1][mazeSize];

extern int jump;
extern int preState;
extern int state;

//motorDriver
extern const int leftPins[];
extern const int rightPins[];

extern int stopDelay;

//encoder
extern const int leftEncoderPins[];
extern const int rightEncoderPins[];

//Speeds
extern int gyroBase;
extern int gyroMax;

extern int sonicRightBase;
extern int sonicLeftBase;
extern int correctionMax;

extern int turnMax;
extern int turnBase;

//ultraSonic
extern const int leftSonicPins[]; //{trig,echo}
extern const int frontSonicPins[];
extern const int rightSonicPins[];

//PID
extern int prevGyroError;  
extern int prevEncoderError;  
extern int prevSonicError;      
extern int prevTurnError;
extern int prevWallError;

extern const double gP;
extern const double gI;    
extern const double gD;

extern const double eP;
extern const double eI;
extern const double eD;
   
extern const double sI;    
extern const double sD;
extern const double sP;

extern const double tP;
extern const double tI;    
extern const double tD;

extern const double wP;
extern const double wI;    
extern const double wD;
 
extern const int cellDistance; //cm
extern const int sideGap;
extern const int frontGap;

extern const int maxDistance;

extern char orientation[4];
extern int orientationKey;

extern int setTime;

#endif //ROBOFEST2023_VARIABLES_H
