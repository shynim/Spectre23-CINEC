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
const int rightPins[] = {7, 11, 10};

int stopDelay = 2000;

//Speeds
int gyroBase = 60;
int gyroMax = 110;

int sonicRightBase = 55;
int sonicLeftBase = 50;
int correctionMax = 15;

int turnMax = 80;
int turnBase = 60;

int driveMax = 70;
int driveBase = 50;

//ultraSonic
const int leftSonicPins[] = {32,33}; //{trig,echo}
const int frontSonicPins[] = {52,53};
const int rightSonicPins[] = {30,31};

//PID
int prevGyroError = 0;    
int prevSonicError = 0;      
int prevDriveError = 0;
int prevTurnError = 0;
int prevWallError = 0;

const double gP = 0.137;
const double gI = 0;    
const double gD = 1.9;

const double sP = 4;   
const double sI = 0;    
const double sD = 15;

const double wP = 2;
const double wI = 0;    
const double wD = 7.5;

const double dP = 20;
const double dI = 0;    
const double dD = 80;
 
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