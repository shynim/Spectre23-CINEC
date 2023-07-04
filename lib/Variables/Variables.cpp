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

int stopDelay = 2000;

//encoder
const int leftEncoderPins[] = {2,3};
const int rightEncoderPins[] = {18,19};

unsigned long leftEncoder = 0;
unsigned long rightEncoder = 0;
unsigned long encoderLeftCount = 0;
unsigned long encoderRightCount = 0;

//Speeds
int gyroBase = 60;
int gyroMax = 110;

int sonicRightBase = 110;
int sonicLeftBase = 110;
int correctionMax = 25;

int turnMax = 80;
int turnBase = 60;

//ultraSonic
const int leftSonicPins[] = {32,33}; //{trig,echo}
const int frontSonicPins[] = {52,53};
const int rightSonicPins[] = {30,31};

//PID
int prevGyroError = 0;    
int prevEncoderError = 0;
int prevSonicError = 0;      
int prevTurnError = 0;
int prevWallError = 0;

const double gP = 0.137;
const double gI = 0;    
const double gD = 1.9;

const double eP = 0.1;
const double eI = 0;
const double eD = 1;

const double sP = 8;   
const double sI = 0;    
const double sD = 25;

const double wP = 2;
const double wI = 0;    
const double wD = 7.5;
 
const double tP = 1;
const double tI = 0;    
const double tD = 1;

//Maze
const int cellDistance = 23; //cm
const int sideGap = 7;
const int frontGap = 5;

const int maxDistance = 20;

//Orientation
char orientation[4] = {'f', 'r', 'b', 'l'};
int orientationKey = 0;

int setTime = 40;