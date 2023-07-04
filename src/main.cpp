#include <Arduino.h>
#include <Variables.h>
#include <PID.h>
#include <MotorDriver.h>
#include <FloodFill.h>
#include <Gyro.h>
#include <Sonic.h>
#include <string.h>

Cell currentCell = {4,0};

MotorDriver driver;
PID pid;
FloodFill floodFill;

Sonic leftSonic(leftSonicPins[0],leftSonicPins[1],maxDistance);
Sonic frontSonic(frontSonicPins[0],frontSonicPins[1],maxDistance);
Sonic rightSonic(rightSonicPins[0],rightSonicPins[1],maxDistance);

Cell end;
const Cell start = {4,0};

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
    if(cell.x < 6 && cell.x >= 0 && cell.y < 6 && cell.y >= 0){return cell;}
    return {-1,-1};
}

bool areCellsEqual(const Cell& p1, const Cell& p2) {
    return (p1.x == p2.x) && (p1.y == p2.y);
}

String maze = "";
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

void goStraight(){

    // int errEncoder = left.read() - right.read();
    // int correction = pid.getEncoderCorrection(errEncoder);
    // driver.applyEncoderPid(correction);

    driver.forward(sonicLeftBase,sonicRightBase);
}

void cellStart(){
    encoderLeftCount = 0;
    encoderRightCount = 0;
    leftEncoder = 0;
    rightEncoder = 0;

    encoderRightCount = encoderRightCount + 200;
    encoderLeftCount = encoderLeftCount + 200;
    int rightBase = 70;
    int leftBase = 70;

    while (rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount){
        int dif = leftEncoder - encoderLeftCount + 200;
        rightBase = 90 + int(dif/10);
        leftBase = 90 + int(dif/20);
        driver.forward(leftBase, rightBase);

        Serial.print(leftEncoder);
        Serial.print("  ");
        Serial.print(rightEncoder);
        Serial.println();
    }

}

void cellBrake(){
    encoderLeftCount = 0;
    encoderRightCount = 0;
    leftEncoder = 0;
    rightEncoder = 0;
    
    encoderRightCount = encoderRightCount + 200;
    encoderLeftCount = encoderLeftCount + 200;
    int rightBase = 110;
    int leftBase = 110;

    while(rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount){
        int dif = leftEncoder - encoderLeftCount + 200;
        rightBase = 110 - int(dif/5);
        leftBase = 110 - int(dif/5);
        driver.forward(leftBase, rightBase);

        Serial.print(leftEncoder);
        Serial.print("  ");
        Serial.print(rightEncoder);
        Serial.println();
    }

    driver.stop();
    
}

void wallFollow(){
    if(leftSonic.readDistance() != 0 && rightSonic.readDistance() != 0){
        state = 0;
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

void moveOneCell(){
    
    cellStart();

    encoderLeftCount = 0;
    encoderRightCount = 0;
    leftEncoder = 0;
    rightEncoder = 0;

    encoderLeftCount = encoderLeftCount + 700;
    encoderRightCount = encoderRightCount + 700;  

    while(rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount){
        wallFollow();
    }

    cellBrake();

    currentCell = getCurrentCell(orientationKey);
}

void autoPosition(){
    if(frontSonic.wallFound()){
        int stableTime = 0;

        int setPointSonic = sideGap;
        int setPoint = frontGap; //swap if going backwards

        while(stableTime <= setTime){
            int errSonic = setPointSonic - rightSonic.readDistance();
            int correctionSonic = pid.getSonicCorrection(errSonic);

            //int err = setPoint - frontSonic.readDistance();
            //int correction = pid.getDriveCorrection(err);

            //driver.applySonicDrivePid(correction * -1);
            driver.applySonicPid(correctionSonic * -1);
        }
        driver.stop();    
    }
}

void turn90(char dir){
    int initAngle = getAngle();
    int stableTime = 0;

    int setPoint;
    setPoint = dir == 'l'? (initAngle - 80):(initAngle + 80); //use 'r' if turns in other dir
    while(stableTime <= setTime){
        int err = setPoint - getAngle();
        int correction = pid.getTurnCorrection(err);
        driver.applyGyroTurnPid(correction);
        
        if(err == 0){
            stableTime++;
        }
    }
    driver.stop();
    if(dir == 'r'){
        if(orientationKey++ == 3){
            orientationKey = 0;
        }
    }else{
        if(orientationKey-- == 0){
            orientationKey = 3;
        }
    }    
}

void updateWall(){
    floodFill.updateWall(currentCell,leftSonic.wallFound(),frontSonic.wallFound(),rightSonic.wallFound(),orientation[orientationKey]);
}

bool junctionFound(){
    return !leftSonic.wallFound() || !rightSonic.wallFound();
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

    attachInterrupt(digitalPinToInterrupt(leftEncoderPins[0]), countLeftOut1, RISING);
    attachInterrupt(digitalPinToInterrupt(leftEncoderPins[1]), countLeftOut1, RISING);
    attachInterrupt(digitalPinToInterrupt(rightEncoderPins[0]), countRightOut1, RISING);
    attachInterrupt(digitalPinToInterrupt(rightEncoderPins[1]), countRightOut1, RISING);

    pinMode(LED_BUILTIN,OUTPUT);
}

void spectreLoop(){
        
    while(true){ //left hand rule
        updateWall(); 
        //if(getColour() == 'r'){
            end = currentCell;
            autoPosition();
            turn90('r');
            autoPosition();
            turn90('r');
            maze = mazeReverse(mazeShort(maze));
            break;
        //}

        if(!leftSonic.wallFound()){
            turn90('l');
            maze += 'L';
        }else if(!frontSonic.wallFound()){
            maze += 'S';
        }else if(!rightSonic.wallFound()){
            turn90('r');
            maze += 'R';
        }else{
            maze += 'B';
            turn90('r');
            autoPosition();
            turn90('r');
        }
        moveOneCell();
    } //left hand rule end
    
    while(true){
        if(areCellsEqual(currentCell,start)){ //coming back to start
            autoPosition();
            turn90('r');
            autoPosition();
            turn90('r');
            break;
        }
        if(junctionFound()){
            char turn = maze.charAt(0);
            maze.remove(0);

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
            default:
                break;
            }
        }
        moveOneCell();

    } //at start

    while(!areCellsEqual(currentCell,end)){ //flood fill start
        
        floodFill.floodFill(end); 
        
        while(true){
            int manhattanDistance = grid[currentCell.x][currentCell.y];
            Cell leftCell = getCell('l');
            Cell frontCell = getCell('f');
            Cell rightCell = getCell('r');
            Cell backCell = getCell('b');

            if(!areCellsEqual(leftCell,{-1,-1}) && grid[leftCell.x][leftCell.y] == manhattanDistance - 1 && !leftSonic.wallFound()){
                turn90('l');
            }else if(!areCellsEqual(frontCell,{-1,-1}) && grid[frontCell.x][frontCell.y] == manhattanDistance - 1 && !frontSonic.wallFound()){
                
            }else if(!areCellsEqual(rightCell,{-1,-1}) && grid[rightCell.x][rightCell.y] == manhattanDistance - 1 && !rightSonic.wallFound()){
                turn90('r');
            }else if(!areCellsEqual(backCell,{-1,-1}) && grid[backCell.x][backCell.y] == manhattanDistance - 1){
                autoPosition();
                turn90('r');
                autoPosition();
                turn90('r');
            }else{break;}
            moveOneCell();
        }
    } //food fill end

}

void tempSpectreSetup(){
    while(frontSonic.readDistance() > 5){
        
    }
    spectreSetup();
    setupGyro();
    turn90('r');
    while(frontSonic.readDistance() > 5){
        
    }
    delay(2000);
    Serial.begin(9600);
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
    moveOneCell();
    driver.stop();
    delay(3000);

    // driver.forward(1000);
    
    // Serial.print(leftEncoder);
    // Serial.print("  ");
    // Serial.print(rightEncoder);
    // Serial.println();
    
}