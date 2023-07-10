#include <Variables.h>

//floodFill
int grid[mazeSize][mazeSize] = {
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0}
};

int verticalWallGrid[mazeSize][mazeSize+1] = {
    {1, 0, 0, 0, 1, 0, 1},   //   | | | | | | |
    {1, 0, 0, 1, 1, 1, 1},   //   | | | | | | | 
    {1, 0, 0, 1, 0, 1, 1},   //   | | | | | | |
    {1, 0, 1, 1, 1, 0, 1},   //   | | | | | | |
    {1, 0, 1, 0, 0, 1, 1},   //   | | | | | | |
    {1, 0, 0, 1, 0, 0, 1}    //   | | | | | | |
};
int horizontalWallGrid[mazeSize+1][mazeSize] = {
    {1, 1, 1, 1, 1, 1},    //  - - - - - -
    {1, 1, 0, 0, 0, 0},    //  - - - - - -
    {0, 1, 1, 0, 0, 0},    //  - - - - - -
    {0, 1, 0, 0, 1, 0},    //  - - - - - -
    {1, 0, 0, 1, 1, 0},    //  - - - - - -
    {1, 1, 0, 1, 0, 1},    //  - - - - - -
    {1, 1, 1, 1, 1, 1}     //  - - - - - -
};

int jump = 0;
int preState = 0;
int state = 0;

//motorDriver
const int leftPins[] = {6, 9, 8};
const int rightPins[] = {7, 10, 11};

int stopDelay = 200;

//encoder
const int leftEncoderPins[] = {2,3};
const int rightEncoderPins[] = {18,19};

unsigned volatile long leftEncoder = 0;
unsigned volatile long rightEncoder = 0;
unsigned long encoderLeftCount = 0;
unsigned long encoderRightCount = 0;

//Speeds
int gyroBase = 60;
int gyroMax = 110;

int sonicRightBase = 120;
int sonicLeftBase = 120;
int correctionMax = 30;

int turnMax = 120;
int turnRightBase = 110;
int turnLeftBase = 110;

int driveMax = 110;
int driveBase = 90;

//ultraSonic
const int leftSonicPins[] = {32,33}; //{trig,echo}
const int frontSonicPins[] = {34,35};
const int rightSonicPins[] = {30,31};

//PID
int prevGyroError = 0;    
int prevEncoderError = 0;
int prevSonicError = 0;      
int prevTurnError = 0;
int prevWallError = 0;
int prevDriveError = 0;

const double gP = 0.137;
const double gI = 0;    
const double gD = 1.9;

const double eP = 0.4;
const double eI = 0;
const double eD = 2;

const double sP = 10;   
const double sI = 0;    
const double sD = 50; //25

const double dP = 60;
const double dI = 0;    
const double dD = 15;

const double wP = 5;
const double wI = 0;    
const double wD = 25;
 
const double tP = 1;
const double tI = 0;    
const double tD = 1;

//Maze
const int cellDistance = 23; //cm
const int sideGap = 7;
const int frontGap = 3;

const int maxDistance = 20;

//Orientation
char orientation[4] = {'f', 'r', 'b', 'l'};
int orientationKey = 1;

int setTime = 10;