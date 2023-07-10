#include <Arduino.h>
#include <Variables.h>
#include <PID.h>
#include <MotorDriver.h>
#include <FloodFill.h>
#include <Gyro.h>
#include <Sonic.h>
#include <string.h>

MotorDriver driver;
PID pid;
FloodFill floodFill;

Sonic leftSonic(leftSonicPins[0],leftSonicPins[1],maxDistance);
Sonic frontSonic(frontSonicPins[0],frontSonicPins[1],maxDistance);
Sonic rightSonic(rightSonicPins[0],rightSonicPins[1],maxDistance);

Cell end;
const Cell start = {mazeSize - 1,0};

Cell currentCell = start;

int mode = 1;

void buzz()
{
    digitalWrite(buzzer, HIGH);
    delay(200);
    digitalWrite(buzzer, LOW);
    delay(200);
}

Cell getCurrentCell(int orientationKey){
    
    if(orientationKey == 0){
        return {currentCell.x-1,currentCell.y};
    }
    if(orientationKey == 1){
        return {currentCell.x,currentCell.y+1};
    }
    if(orientationKey == 2){
        return {currentCell.x+1,currentCell.y};
    }
    if(orientationKey == 3){
        return {currentCell.x,currentCell.y-1};
    }
    return {0,0};
}

Cell getCell(char dir){
    Cell cell;
    if((dir == 'l' && orientationKey == 0) || (dir == 'f' && orientationKey == 3) || (dir == 'r' && orientationKey == 2) || (dir == 'b' && orientationKey == 1)){
        cell.x = currentCell.x;
        cell.y = currentCell.y-1;
    }
    if((dir == 'l' && orientationKey == 1) || (dir == 'f' && orientationKey == 0) || (dir == 'r' && orientationKey == 3) || (dir == 'b' && orientationKey == 2)){
        cell.x = currentCell.x-1;
        cell.y = currentCell.y;
    }
    if((dir == 'l' && orientationKey == 2) || (dir == 'f' && orientationKey == 1) || (dir == 'r' && orientationKey == 0) || (dir == 'b' && orientationKey == 3)){
        cell.x = currentCell.x;
        cell.y = currentCell.y+1;
    }
    if((dir == 'l' && orientationKey == 3) || (dir == 'f' && orientationKey == 2) || (dir == 'r' && orientationKey == 1) || (dir == 'b' && orientationKey == 0)){
        cell.x = currentCell.x+1;
        cell.y = currentCell.y;
    }
    if(cell.x < mazeSize && cell.x >= 0 && cell.y < mazeSize && cell.y >= 0){return cell;}
    return {-1,-1};
}

bool areCellsEqual(const Cell& p1, const Cell& p2) {
    return (p1.x == p2.x) && (p1.y == p2.y);
}

String maze = "LSLLLRRBLLRSRLLBSRLLRRSBLBLLLRLBSLLRLLRLRLBS";
const String mappings[2][6] = {{"LBL", "LBS", "RBL", "SBL", "SBS", "LBR"},
                               {"S",   "R",   "B",   "R",   "B",   "B"}};

String mazeShort(String maze) {
    while (maze.indexOf("B") != -1) {
        for (int i = 0; i < 6; i++) {
            while (maze.indexOf(mappings[0][i]) != -1) {
                maze.replace(mappings[0][i], mappings[1][i]);
            }
        }
    }
    return maze;
}

String mazeReverse(String maze){
    for (char& c : maze) {
        if (c == 'L') {
            c = 'R'; 
        } else if (c == 'R') {
            c = 'L';
        }
    }
    String reversed;
    
    for (int i = maze.length() - 1; i >= 0; --i) {
        reversed += maze.charAt(i);
    }
    return reversed;
}

bool startCounting = false;
int countLeft = 0;
int countFront = 0;
int countRight = 0;

bool wallLeft(){
    if(countLeft > 0){
        return true;
    }else{
        return false;
    }

}

bool wallFront(){
    if(countFront > 0){
        return true;
    }else{
        return false;
    }

}

bool wallRight(){
    if(countRight > 0){
        return true;
    }else{
        return false;
    }

}

bool shouldBrake(){
    if(mode == 1){
        
        if(wallLeft()){
            return false;
        }else{
           return true;
        }

    }else if(mode == 3){

        Cell frontCell = getCell('f');
        if(grid[frontCell.x][frontCell.y] == grid[currentCell.x][currentCell.y] - 1){
            return false;
        }else{
            return true;
        }
    }
}

void goStraight(){

    int errEncoder = leftEncoder - rightEncoder;
    int correction = pid.getEncoderCorrection(errEncoder);
    driver.applyEncoderPid(correction);

    //driver.forward(sonicLeftBase,sonicRightBase);
}

void wallFollow(){

    if(startCounting){
        if(frontSonic.readDistance() != 0){
            countFront++;
        }else{
            countFront--;
        }
    }

    if(leftSonic.readDistance() != 0 && rightSonic.readDistance() != 0){
        state = 0;

        if(startCounting){countLeft++;countRight++;}
        
        if (state != preState)
        {
          jump = 5;
          prevWallError = 0;
        }
        if (jump>0)
        {
          goStraight();
          jump = jump-1;
        }
        else
        {
          int err = leftSonic.readDistance() - rightSonic.readDistance();
          int correction = pid.getWallCorrection(err);
          driver.applyWallPid(correction * -1);
        }

    }else if(leftSonic.readDistance() == 0 && rightSonic.readDistance() != 0){
        state = 1;

        if(startCounting){countLeft--;countRight++;}

        if (state != preState)
        {
          jump = 5;
          prevSonicError = 0;
        }
        if (jump>0)
        {
          goStraight();
          jump = jump-1;
        }
        else
        {
          int err = sideGap - rightSonic.readDistance();
          int correction = pid.getSonicCorrection(err);
          driver.applySonicPid(correction * -1);
        }

    }else if(leftSonic.readDistance() != 0 && rightSonic.readDistance() == 0){
        state = 2;

        if(startCounting){countLeft++;countRight--;}

        if (state != preState)
        {
          jump = 5;
          prevSonicError = 0;
        }
        if (jump>0)
        {
          goStraight();
          jump = jump-1;
        }
        else
        {
          int err = sideGap - leftSonic.readDistance();
          int correction = pid.getSonicCorrection(err);
          driver.applySonicPid(correction);
        }

    }else if(leftSonic.readDistance() == 0 && rightSonic.readDistance() == 0){
        state = 3;

        if(startCounting){countLeft--;countRight--;}

        goStraight();

        prevWallError = 0;
    }
    /*Serial.print(state);
    Serial.print("  ");
    Serial.print(leftSonic.readDistance());
    Serial.print("  ");
    Serial.print(rightSonic.readDistance());
    Serial.println();*/
    preState = state;
}

void cellStart(){
    encoderLeftCount = 0;
    encoderRightCount = 0;
    leftEncoder = 0;
    rightEncoder = 0;

    encoderRightCount = encoderRightCount + 100;
    encoderLeftCount = encoderLeftCount + 100;

    while (rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount){
        int dif = leftEncoder - encoderLeftCount + 100;
        sonicRightBase = 90 + int(dif/10);
        sonicLeftBase = 90 + int(dif/10);
        wallFollow();

    }

}

void cellBrake(){

    encoderLeftCount = 0;
    encoderRightCount = 0;
    leftEncoder = 0;
    rightEncoder = 0;
    
    encoderRightCount = encoderRightCount + 100;
    encoderLeftCount = encoderLeftCount + 100;

    while(rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount){
        int dif = leftEncoder - encoderLeftCount + 100;
        sonicRightBase = 110 - int(dif/5);
        sonicLeftBase = 110 - int(dif/5);
        wallFollow();

    }

    sonicLeftBase = 110;
    sonicRightBase = 110;
    driver.stop();
    
}

void autoPosition(){
    int stableTime = 0;

    int setPoint = frontGap; 

    while(stableTime <= setTime){

        int err = setPoint - frontSonic.readDistance();
        int correction = pid.getDriveCorrection(err);
        driver.applySonicDrivePid(correction * -1);

        int errEncoder = leftEncoder - rightEncoder;
        int correctionEncoder = pid.getEncoderCorrection(errEncoder);
        driver.applyEncoderPid(correctionEncoder);
        
        if(err == 0){
            stableTime++;
        }
        
    }

    driver.stop();    
    sonicLeftBase = 110;
    sonicRightBase = 110;
}

boolean braked = true;
void moveOneCell(){
    
    currentCell = getCurrentCell(orientationKey);
    if(braked){
        cellStart();
    }else{
        encoderLeftCount = 0;
        encoderRightCount = 0;
        leftEncoder = 0;
        rightEncoder = 0;

        encoderRightCount = encoderRightCount + 100;
        encoderLeftCount = encoderLeftCount + 100;

        while (rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount){
            wallFollow();

        }
    }
    
    encoderLeftCount = 0;
    encoderRightCount = 0;
    leftEncoder = 0;
    rightEncoder = 0;

    encoderLeftCount = encoderLeftCount + 700;
    encoderRightCount = encoderRightCount + 700;  

    while(rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount){
        wallFollow();

    }
    encoderLeftCount = encoderLeftCount + 100;
    encoderRightCount = encoderRightCount + 100; 
    
    startCounting = true; 
    countLeft = 0;
    countFront = 0;
    countRight = 0;
    while(rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount){
        wallFollow();

    }
    startCounting = false;
    

    if(!wallFront()){
        if(shouldBrake()){
            cellBrake();
            braked = true;
        }else{
            encoderLeftCount = 0;
            encoderRightCount = 0;
            leftEncoder = 0;
            rightEncoder = 0;

            encoderLeftCount = encoderLeftCount + 100;
            encoderRightCount = encoderRightCount + 100;  

            while(rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount){
                wallFollow();

            }
            braked = false;
        }
    }else{

        autoPosition();
        braked = true;
    }
    
    
}

void turn90(char dir){
    encoderLeftCount = 0;
    encoderRightCount = 0;
    leftEncoder = 0;
    rightEncoder = 0;
    encoderRightCount= encoderRightCount + 100;
    encoderLeftCount= encoderLeftCount + 100;
    while (rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount){

        int dif = leftEncoder - encoderLeftCount + 100;

        turnRightBase = int(70+110/(1+pow(2.73,((50-dif)*0.05))));
        turnLeftBase = int(70+110/(1+pow(2.73,((50-dif)*0.05))));
        dir == 'r' ? driver.turnRight(turnLeftBase, turnRightBase) : driver.turnLeft(turnLeftBase, turnRightBase);

        Serial.print(leftEncoder);
        Serial.print("  ");
        Serial.print(rightEncoder);
        Serial.println();
        
    }
    turnRightBase=180;
    turnLeftBase=170;
    encoderRightCount= encoderRightCount + 100;
    encoderLeftCount= encoderLeftCount + 100;
    while(rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount)
    {
        dir == 'r' ? driver.turnRight(turnLeftBase, turnRightBase) : driver.turnLeft(turnLeftBase, turnRightBase);

        Serial.print(leftEncoder);
        Serial.print("  ");
        Serial.print(rightEncoder);
        Serial.println();
    }
    encoderRightCount= encoderRightCount + 100;
    encoderLeftCount= encoderLeftCount + 100;
    while (rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount)
    {
        int dif = leftEncoder - encoderLeftCount + 100;
        turnRightBase = int(180-110/(1+pow(2.73,((50-dif)*0.05))));
        turnLeftBase = int(170-110/(1+pow(2.73,((50-dif)*0.05))));
        dir == 'r' ? driver.turnRight(turnLeftBase, turnRightBase) : driver.turnLeft(turnLeftBase, turnRightBase);

        Serial.print(leftEncoder);
        Serial.print("  ");
        Serial.print(rightEncoder);
        Serial.println();
    }
    driver.brake();

    if(dir == 'r'){
        if(orientationKey++ == 3){
            orientationKey = 0;
        }
    }else{
        if(orientationKey-- == 0){
            orientationKey = 3;
        }
    } 

    turnLeftBase = 170;
    turnRightBase = 180;
    encoderLeftCount = 0;
    encoderRightCount = 0;
    leftEncoder = 0;
    rightEncoder = 0;

}

void turnBack(){
    encoderLeftCount = 0;
    encoderRightCount = 0;
    leftEncoder = 0;
    rightEncoder = 0;
    encoderRightCount= encoderRightCount + 100;
    encoderLeftCount= encoderLeftCount + 100;
    while (rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount)
    {
        int dif = leftEncoder - encoderLeftCount + 100;
        turnRightBase = int(70+110/(1+pow(2.73,((50-dif)*0.05))));
        turnLeftBase = int(70+110/(1+pow(2.73,((50-dif)*0.05))));
        driver.turnRight(turnLeftBase, turnRightBase);

        Serial.print(leftEncoder);
        Serial.print("  ");
        Serial.print(rightEncoder);
        Serial.println();
    }
    turnRightBase=180;
    turnLeftBase=170;
    encoderRightCount= encoderRightCount + 480;
    encoderLeftCount= encoderLeftCount + 480;
    while(rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount)
    {
        driver.turnRight(turnLeftBase, turnRightBase);

        Serial.print(leftEncoder);
        Serial.print("  ");
        Serial.print(rightEncoder);
        Serial.println();
    }
    encoderRightCount= encoderRightCount + 100;
    encoderLeftCount= encoderLeftCount + 100;
    while (rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount)
    {
        int dif = leftEncoder - encoderLeftCount + 100;
        turnRightBase = int(180-110/(1+pow(2.73,((50-dif)*0.05))));
        turnLeftBase = int(170-110/(1+pow(2.73,((50-dif)*0.05))));
        driver.turnRight(turnLeftBase, turnRightBase);

        Serial.print(leftEncoder);
        Serial.print("  ");
        Serial.print(rightEncoder);
        Serial.println();
    }
    driver.brake();

    orientationKey += 2;
    if(orientationKey > 3){
        orientationKey = orientationKey - 4;
    }

    turnLeftBase = 170;
    turnRightBase = 180;
    encoderLeftCount = 0;
    encoderRightCount = 0;
    leftEncoder = 0;
    rightEncoder = 0;

    int temp = countLeft;
    countLeft = countRight;
    countRight = temp;
    countFront = -10;

}

void updateWall(){
    floodFill.updateWall(currentCell,wallLeft(),wallFront(),wallRight(),orientation[orientationKey]);
}

bool junctionFound(){
    return !wallLeft() || !wallRight();
}

bool deadEnd(){
    return wallLeft() && wallRight() && wallFront();
}

void countLeftOut1(){
    leftEncoder += 1;
}
void countRightOut1(){
    rightEncoder += 1;
}

void spectreSetup(){
    driver.init(const_cast<int *>(leftPins), const_cast<int *>(rightPins));

    pinMode(leftEncoderPins[0], INPUT);
    pinMode(leftEncoderPins[1], INPUT);
    pinMode(rightEncoderPins[0], INPUT);
    pinMode(rightEncoderPins[1], INPUT);

    pinMode(buzzer,OUTPUT);

    attachInterrupt(digitalPinToInterrupt(leftEncoderPins[0]), countLeftOut1, RISING);
    attachInterrupt(digitalPinToInterrupt(leftEncoderPins[1]), countLeftOut1, RISING);
    attachInterrupt(digitalPinToInterrupt(rightEncoderPins[0]), countRightOut1, RISING);
    attachInterrupt(digitalPinToInterrupt(rightEncoderPins[1]), countRightOut1, RISING);

    pinMode(LED_BUILTIN,OUTPUT);
}

void loopFloodFill(Cell end){

    while(!areCellsEqual(currentCell,end)){ //flood fill start
        
        floodFill.floodFill(end); 
        
        while(true){
            updateWall();
            int manhattanDistance = grid[currentCell.x][currentCell.y];
            Cell leftCell = getCell('l');
            Cell frontCell = getCell('f');
            Cell rightCell = getCell('r');
            Cell backCell = getCell('b');

            if(!areCellsEqual(leftCell,{-1,-1}) && grid[leftCell.x][leftCell.y] == manhattanDistance - 1 && !wallLeft()){
                turn90('l');
            }else if(!areCellsEqual(frontCell,{-1,-1}) && grid[frontCell.x][frontCell.y] == manhattanDistance - 1 && !wallFront()){
                
            }else if(!areCellsEqual(rightCell,{-1,-1}) && grid[rightCell.x][rightCell.y] == manhattanDistance - 1 && !wallRight()){
                turn90('r');
            }else if(!areCellsEqual(backCell,{-1,-1}) && grid[backCell.x][backCell.y] == manhattanDistance - 1){
                turnBack();
            }else{break;}
            moveOneCell();
        }
    } //food fill end

}

void spectreLoop(){
    
    mode = 1;
    while(true){ //left hand rule

        updateWall(); 
        if(areCellsEqual(currentCell, end)){
            end = currentCell;
            turnBack();
            maze = mazeReverse(mazeShort(maze));
            break;
        }

        if(!wallLeft()){
            turn90('l');
            maze += 'L';
        }else if(!wallFront()){
            if(junctionFound()){
                maze += 'S'; 
            }
        }else if(!wallRight()){
            turn90('r');
            maze += 'R';
        }else{
            maze += 'B';
            turnBack();
        }
        moveOneCell();

    } //left hand rule end
    
    mode = 2;
    int i = 0;
    while(true){
    
        moveOneCell();

        if(areCellsEqual(currentCell,start)){ //coming back to start
            turnBack();
            break;
        }

        if(junctionFound() || deadEnd()){
            char turn = maze[i++];

            switch (turn)
            {
            case 'L':
                turn90('l');
                break;
            case 'R':
                turn90('r');
                break;
            case 'S':
                break;
            case 'B':
                turnBack();
                break;
            default:
                break;
            }
        }

    } //at start

    mode = 3;

    while(true){
        loopFloodFill(end);
        floodFill.clearGrid();
        loopFloodFill(start);
        floodFill.clearGrid();
    }

}

void mazeStart(){
    floodFill.updateWall(currentCell,leftSonic.wallFound(),frontSonic.wallFound(),rightSonic.wallFound(),orientation[orientationKey]);
    moveOneCell();
}

void serialPrint(int data){
    Serial.print(data);
    Serial.println();
}

void setup(){
    spectreSetup();
    Serial.begin(9600);
}

void loop(){ 
    end = {1, 0};
    //Serial.print(mazeReverse(mazeShort(maze)));
    
    mazeStart();
    spectreLoop();

    
}