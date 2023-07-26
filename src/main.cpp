#include <Arduino.h>
#include <Variables.h>
#include <PID.h>
#include <MotorDriver.h>
#include <FloodFill.h>
#include <Gyro.h>
#include <Sonic.h>
#include <string.h>
#include <Colour.h>

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
int foundEnd = 0;

void buzz(){
    digitalWrite(buzzer, HIGH);
    delay(200);
    digitalWrite(buzzer, LOW);
    delay(200);
}

void lightRed(){
    digitalWrite(red,HIGH);
    digitalWrite(green,LOW);
    digitalWrite(blue,LOW);
}
void lightGreen(){
    digitalWrite(red,LOW);
    digitalWrite(green,HIGH);
    digitalWrite(blue,LOW);
}
void lightBlue(){
    digitalWrite(red,LOW);
    digitalWrite(green,LOW);
    digitalWrite(blue,HIGH);
}
void lightOff(){
    digitalWrite(red,LOW);
    digitalWrite(green,LOW);
    digitalWrite(blue,LOW);
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

bool junctionFound(){
    return !wallLeft() || !wallRight();
}

bool deadEnd(){
    return wallLeft() && wallRight() && wallFront();
}

int mazeIndex = 0;
bool shouldBrake(){
    if(mode == 1){
        
        if(wallLeft()){
            return false;
        }else{
           return true;
        }

    }else if(mode == 2){

        if(junctionFound()){
            if(maze[mazeIndex] == 'S'){
                return false;
            }else{
                return true;
            }
        }else{
            return false;
        }

    }else if(mode == 3){

        Cell frontCell = getCell('f');
        Cell leftCell = getCell('l');
        Cell rightCell = getCell('r');
        if(grid[frontCell.x][frontCell.y] != grid[currentCell.x][currentCell.y] - 1){
            return true;
        }else{
            if(grid[leftCell.x][leftCell.y] == grid[currentCell.x][currentCell.y] - 1 && !wallLeft()){
                return true;
            }else{
                return false;
            }
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
        if(frontSonic.readDistanceFront() != 0){
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
          int err = leftSonic.readDistance() - rightSonic.readDistance() - 0;
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
          int err = sideGapRight - rightSonic.readDistance();
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
          int err = sideGapLeft - leftSonic.readDistance();
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

    encoderRightCount = encoderRightCount + 200;
    encoderLeftCount = encoderLeftCount + 200;

    while (rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount){
        int dif = leftEncoder - encoderLeftCount + 200;
        sonicRightBase = (rightBase - 20) + int(dif/10);
        sonicLeftBase = (leftBase - 20) + int(dif/10);
        wallFollow();

    }

}

void cellBrake(){

    encoderLeftCount = 0;
    encoderRightCount = 0;
    leftEncoder = 0;
    rightEncoder = 0;
    
    encoderRightCount = encoderRightCount + 150;
    encoderLeftCount = encoderLeftCount + 150;

    while(rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount){
        int dif = leftEncoder - encoderLeftCount + 150;
        sonicRightBase = rightBase - int(dif/7.5);
        sonicLeftBase = leftBase - int(dif/7.5);
        wallFollow();

    }

    sonicLeftBase = leftBase;
    sonicRightBase = rightBase;
    driver.stop();
    
}

void autoPosition(){
    int stableTime = 0;
    // encoderLeftCount = 0;
    // encoderRightCount = 0;
    // leftEncoder = 0;
    // rightEncoder = 0;
    int setPoint = frontGap; 

    while(stableTime <= setTime){

        int err = setPoint - frontSonic.readDistanceFront();
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
    sonicLeftBase = leftBase;
    sonicRightBase = rightBase;
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

        encoderRightCount = encoderRightCount + 200;
        encoderLeftCount = encoderLeftCount + 200;

        while (rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount){
            wallFollow();

        }
    }
    
    encoderLeftCount = 0;
    encoderRightCount = 0;
    leftEncoder = 0;
    rightEncoder = 0;

    encoderLeftCount = encoderLeftCount + 620;
    encoderRightCount = encoderRightCount + 620;  

    while(rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount){
        wallFollow();
        
    }
    encoderLeftCount = encoderLeftCount + 100;
    encoderRightCount = encoderRightCount + 100; 
    
    startCounting = true; 
    foundEnd = 0;
    countLeft = 0;
    countFront = 0;
    countRight = 0;
    while(rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount){
        wallFollow();
        if(getColour() == 1){
            foundEnd++;
        }else{
            foundEnd--;
        }

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

            encoderLeftCount = encoderLeftCount + 150;
            encoderRightCount = encoderRightCount + 150;  

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

void turnLeft(){
    encoderLeftCount = 0;
    encoderRightCount = 0;
    leftEncoder = 0;
    rightEncoder = 0;
    encoderRightCount= encoderRightCount + 100;
    encoderLeftCount= encoderLeftCount + 100;
    while (rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount){

        int dif = leftEncoder - encoderLeftCount + 100;

        turnRightBase = int(70+50/(1+pow(2.73,((50-dif)*0.05))));
        turnLeftBase = int(120+50/(1+pow(2.73,((50-dif)*0.05))));
        driver.turnLeft(turnLeftBase, turnRightBase);
        
    }
    turnRightBase=120;
    turnLeftBase=180;
    encoderRightCount= encoderRightCount + 120;
    encoderLeftCount= encoderLeftCount + 120;
    while(rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount)
    {
        driver.turnLeft(turnLeftBase, turnRightBase);

    }
    encoderRightCount= encoderRightCount + 100;
    encoderLeftCount= encoderLeftCount + 100;
    while (rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount)
    {
        int dif = leftEncoder - encoderLeftCount + 100;
        turnRightBase = int(120-50/(1+pow(2.73,((50-dif)*0.05))));
        turnLeftBase = int(180-50/(1+pow(2.73,((50-dif)*0.05))));
        driver.turnLeft(turnLeftBase, turnRightBase);

    }
    driver.brake();

    if(orientationKey-- == 0){
        orientationKey = 3;
    }

    turnLeftBase = 120;
    turnRightBase = 130;
    encoderLeftCount = 0;
    encoderRightCount = 0;
    leftEncoder = 0;
    rightEncoder = 0;

}

void turnRight(){
    encoderLeftCount = 0;
    encoderRightCount = 0;
    leftEncoder = 0;
    rightEncoder = 0;
    encoderRightCount= encoderRightCount + 100;
    encoderLeftCount= encoderLeftCount + 100;
    while (rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount){

        int dif = leftEncoder - encoderLeftCount + 100;

        turnLeftBase = int(70+50/(1+pow(2.73,((50-dif)*0.05))));
        turnRightBase = int(120+50/(1+pow(2.73,((50-dif)*0.05))));
        driver.turnRight(turnLeftBase, turnRightBase);
        
    }
    turnLeftBase=120;
    turnRightBase=180; 
    encoderRightCount= encoderRightCount + 120;
    encoderLeftCount= encoderLeftCount + 120;
    while(rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount)
    {
        driver.turnRight(turnLeftBase, turnRightBase);

    }
    encoderRightCount= encoderRightCount + 100;
    encoderLeftCount= encoderLeftCount + 100;
    while (rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount)
    {
        int dif = leftEncoder - encoderLeftCount + 100;
        turnLeftBase = int(120-50/(1+pow(2.73,((50-dif)*0.05))));
        turnRightBase = int(180-50/(1+pow(2.73,((50-dif)*0.05))));
        driver.turnRight(turnLeftBase, turnRightBase);

    }
    driver.brake();

    if(orientationKey++ == 3){
        orientationKey = 0;
    }

    turnLeftBase = 120;
    turnRightBase = 130;
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
        turnRightBase = int(80+50/(1+pow(2.73,((50-dif)*0.05))));
        turnLeftBase = int(80+50/(1+pow(2.73,((50-dif)*0.05))));
        driver.turnRight(turnLeftBase, turnRightBase);

    }
    turnRightBase=rightTurnBase;
    turnLeftBase=leftTurnBase;
    encoderRightCount= encoderRightCount + 550;
    encoderLeftCount= encoderLeftCount + 550;
    while(rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount)
    {
        driver.turnRight(turnLeftBase, turnRightBase);

    }
    encoderRightCount= encoderRightCount + 100;
    encoderLeftCount= encoderLeftCount + 100;
    while (rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount)
    {
        int dif = leftEncoder - encoderLeftCount + 100;
        turnRightBase = int(130-50/(1+pow(2.73,((50-dif)*0.05))));
        turnLeftBase = int(130-50/(1+pow(2.73,((50-dif)*0.05))));
        driver.turnRight(turnLeftBase, turnRightBase);

    }
    driver.brake();

    orientationKey += 2;
    if(orientationKey > 3){
        orientationKey = orientationKey - 4;
    }

    turnLeftBase = rightTurnBase;
    turnRightBase = leftTurnBase;
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

void countLeftOut1(){
    leftEncoder += 1;
}
void countRightOut1(){
    rightEncoder += 1;
}

void spectreSetup(){

    pinMode(leftSonicPins[0], OUTPUT);
    pinMode(leftSonicPins[1], INPUT);
    pinMode(rightSonicPins[0], OUTPUT);
    pinMode(rightSonicPins[1], INPUT);
    pinMode(frontSonicPins[0], OUTPUT);
    pinMode(frontSonicPins[1], INPUT);

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

    // Set S0 - S3 as outputs
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);
    
    // Set Sensor output as input
    pinMode(sensorOut, INPUT);
    
    // Set Frequency scaling to 20%
    digitalWrite(S0,HIGH);
    digitalWrite(S1,LOW);

    pinMode(red,OUTPUT);
    pinMode(green,OUTPUT);
    pinMode(blue,OUTPUT);

    pinMode(LED_BUILTIN,OUTPUT);

}

void printManhattan(){
    for(int i = 0; i < mazeSize; i++){
        for(int j = 0; j < mazeSize; j++){
            if(grid[i][j] >= 10){
                Serial.print(grid[i][j]);
                Serial.print(" ");

            }else{
                Serial.print(grid[i][j]);
                Serial.print("  ");
            }
            
        }
        Serial.println();
    }

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
                turnLeft();
            }else if(!areCellsEqual(frontCell,{-1,-1}) && grid[frontCell.x][frontCell.y] == manhattanDistance - 1 && !wallFront()){
                
            }else if(!areCellsEqual(rightCell,{-1,-1}) && grid[rightCell.x][rightCell.y] == manhattanDistance - 1 && !wallRight()){
                turnRight();
            }else if(!areCellsEqual(backCell,{-1,-1}) && grid[backCell.x][backCell.y] == manhattanDistance - 1){
                turnBack();
            }else{break;}
            moveOneCell();
        }
    } //food fill end

}

void spectreLoop(){
    
    mode = 1;
    lightBlue();
    while(true){ //left hand rule

        updateWall(); 
        if(areCellsEqual(currentCell, end)){
            end = currentCell;
            lightRed();
            buzz();
        }
        if(areCellsEqual(currentCell, start)){
            turnBack();
            break;
        }

        if(!wallLeft()){
            turnLeft();
            maze += 'L';
        }else if(!wallFront()){
            if(junctionFound()){
                maze += 'S'; 
            }
        }else if(!wallRight()){
            turnRight();
            maze += 'R';
        }else{
            maze += 'B';
            turnBack();
        }
        moveOneCell();

    } //left hand rule end
    
    // mode = 2;
    // while(true){
    
    //     moveOneCell();

    //     if(areCellsEqual(currentCell,start)){ //coming back to start
    //         turnBack();
    //         break;
    //     }

    //     if(junctionFound() || deadEnd()){
    //         char turn = maze[mazeIndex++];

    //         switch (turn)
    //         {
    //         case 'L':
    //             turnLeft();
    //             break;
    //         case 'R':
    //             turnRight();
    //             break;
    //         case 'S':
    //             break;
    //         case 'B':
    //             turnBack();
    //             break;
    //         default:
    //             break;
    //         }
    //     }

    // } //at start

    mode = 3;
    lightGreen();
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
    lightOff();
}

void loop(){ 
    end = {3, 0};

    mazeStart();
    spectreLoop();

    // turnBack();
    // delay(1000);
    // turnLeft();
    // delay(2000);
    // turnRight();
    // delay(2000);
    // printManhattan();
    // delay(10000);

    // Serial.print(leftSonic.readDistance());
    // Serial.print(" ");
    // Serial.print(frontSonic.readDistance());
    // Serial.print(" ");
    // Serial.print(rightSonic.readDistance());
    // Serial.println();
    // getRedPW();
    // getGreenPW();
    // getBluePW();
    // serialPrint(getColour());

}
