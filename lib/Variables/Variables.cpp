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
    {1, 0, 0, 0, 0, 0, 1},   //   | | | | | | |
    {1, 0, 0, 0, 0, 0, 1},   //   | | | | | | | 
    {1, 0, 0, 0, 0, 0, 1},   //   | | | | | | |
    {1, 0, 0, 0, 0, 0, 1},   //   | | | | | | |
    {1, 0, 0, 0, 0, 0, 1},   //   | | | | | | |
    {1, 0, 0, 0, 0, 0, 1}    //   | | | | | | |
}; 
int horizontalWallGrid[mazeSize+1][mazeSize] = {
    {1, 1, 1, 1, 1, 1},    //  - - - - - -
    {0, 0, 0, 0, 0, 0},    //  - - - - - -
    {0, 0, 0, 0, 0, 0},    //  - - - - - -
    {0, 0, 0, 0, 0, 0},    //  - - - - - -
    {0, 0, 0, 0, 0, 0},    //  - - - - - -
    {0, 0, 0, 0, 0, 0},    //  - - - - - -
    {1, 1, 1, 1, 1, 1}     //  - - - - - -
};

int jump = 0;
int preState = 0;
int state = 0;

//LED
const int red = 36;
const int blue = 37;
const int green = 38;

//colourSensor
const int S0 = 48;
const int S1 = 49;
const int S2 = 50;
const int S3 = 51;
const int sensorOut = 52;

//motorDriver
const int leftPins[] = {6, 9, 8};
const int rightPins[] = {7, 10, 11};

//encoder
const int leftEncoderPins[] = {2,3};
const int rightEncoderPins[] = {18,19};

unsigned volatile long leftEncoder = 0;
unsigned volatile long rightEncoder = 0;
unsigned long encoderLeftCount = 0;
unsigned long encoderRightCount = 0;

//Speeds
int rightBase = 110;
int leftBase = 120;

int sonicRightBase = rightBase;
int sonicLeftBase = leftBase;
int correctionMax = 40;

const int rightTurnBase = 160;
const int leftTurnBase = 120;

int turnRightBase = rightTurnBase;
int turnLeftBase = leftTurnBase;

int driveMax = 120;
int driveBase = 100;

//ultraSonic
const int leftSonicPins[] = {32,33}; //{trig,echo}
const int frontSonicPins[] = {35,34};
const int rightSonicPins[] = {30,31};

//PID
int prevEncoderError = 0;
int prevSonicError = 0;      
int prevWallError = 0;
int prevDriveError = 0;

const double eP = 0.4;
const double eI = 0;
const double eD = 2;

const double sP = 0.8;   
const double sI = 0;    
const double sD = 3;

const double dP = 60;
const double dI = 0;    
const double dD = 30;

const double wP = 0.8;
const double wI = 0;    
const double wD = 3;

//Maze
const int cellDistance = 238; //mm
const int sideGapLeft = 79;
const int sideGapRight = 74;
const int frontGap = 3; //cm

const int maxDistance = 20;

//Orientation
char orientation[4] = {'f', 'r', 'b', 'l'};
int orientationKey = 0;

int setTime = 10;
int stopDelay = 50;
