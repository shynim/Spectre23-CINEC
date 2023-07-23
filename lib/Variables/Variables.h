#ifndef ROBOFEST2023_VARIABLES_H
#define ROBOFEST2023_VARIABLES_H

#define buzzer 53

//floodFill
#define mazeSize 6

extern int grid[mazeSize][mazeSize];
extern int verticalWallGrid[mazeSize][mazeSize+1];
extern int horizontalWallGrid[mazeSize+1][mazeSize];

extern int jump;
extern int preState;
extern int state;

//LED
extern const int red;
extern const int green;
extern const int blue;

//colourSensor
extern const int S0;
extern const int S1;
extern const int S2;
extern const int S3;
extern const int sensorOut;

//motorDriver
extern const int leftPins[];
extern const int rightPins[];

extern int stopDelay;

//encoder
extern const int leftEncoderPins[];
extern const int rightEncoderPins[];

extern unsigned volatile long leftEncoder;
extern unsigned volatile long rightEncoder;
extern unsigned long encoderLeftCount;
extern unsigned long encoderRightCount;

//Speeds
extern const int rightBase;
extern const int leftBase;

extern const int rightTurnBase;
extern const int leftTurnBase;

extern int sonicRightBase;
extern int sonicLeftBase;
extern int correctionMax;

extern int turnMax;
extern int turnRightBase;
extern int turnLeftBase;

extern int driveMax;
extern int driveBase;

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
extern int prevDriveError;

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

extern const double dP;
extern const double dI;    
extern const double dD;
 
extern const int cellDistance; //cm
extern const int sideGapLeft;
extern const int sideGapRight;
extern const int frontGap;

extern const int maxDistance;

extern char orientation[4];
extern int orientationKey;

extern int setTime;

#endif //ROBOFEST2023_VARIABLES_H
