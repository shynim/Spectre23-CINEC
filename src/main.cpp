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

void goStraight(){

    int errEncoder = leftEncoder - rightEncoder;
    int correction = pid.getEncoderCorrection(errEncoder);
    driver.applyEncoderPid(correction);

    //driver.forward(sonicLeftBase,sonicRightBase);
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

        Serial.print(leftEncoder);
        Serial.print("  ");
        Serial.print(rightEncoder);
        Serial.println();
    }

}

void cellBrake(){
    if(frontSonic.readDistance() > 7){

        encoderLeftCount = 0;
        encoderRightCount = 0;
        leftEncoder = 0;
        rightEncoder = 0;
        
        encoderRightCount = encoderRightCount + 200;
        encoderLeftCount = encoderLeftCount + 200;

        while(rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount){
            int dif = leftEncoder - encoderLeftCount + 200;
            sonicRightBase = 110 - int(dif/5);
            sonicLeftBase = 110 - int(dif/5);
            wallFollow();

            Serial.print(leftEncoder);
            Serial.print("  ");
            Serial.print(rightEncoder);
            Serial.println();
        }

        sonicLeftBase = 110;
        sonicRightBase = 110;
        driver.stop();

    }else{

        sonicLeftBase = 110;
        sonicRightBase = 110;
        driver.stop();

    }
    
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
        
        stableTime++;
    }

    driver.stop();    
    sonicLeftBase = 110;
    sonicRightBase = 110;
}

void moveOneCell(){
    
    cellStart();

    encoderLeftCount = 0;
    encoderRightCount = 0;
    leftEncoder = 0;
    rightEncoder = 0;

    encoderLeftCount = encoderLeftCount + 900;
    encoderRightCount = encoderRightCount + 900;  

    while(rightEncoder <= encoderRightCount || leftEncoder <= encoderLeftCount){
        wallFollow();

        Serial.print(leftEncoder);
        Serial.print("  ");
        Serial.print(rightEncoder);
        Serial.println();
    }

    if(frontSonic.readDistance() == 0){
        cellBrake();
    }else{
        autoPosition();
    }
    
    currentCell = getCurrentCell(orientationKey);
    
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

}

void updateWall(){
    floodFill.updateWall(currentCell,leftSonic.wallFound(),frontSonic.wallFound(),rightSonic.wallFound(),orientation[orientationKey]);
}

bool junctionFound(){
    return !leftSonic.wallFound() || !rightSonic.wallFound();
}

bool deadEnd(){
    return leftSonic.wallFound() && rightSonic.wallFound() && frontSonic.wallFound();
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

            if(!areCellsEqual(leftCell,{-1,-1}) && grid[leftCell.x][leftCell.y] == manhattanDistance - 1 && !leftSonic.wallFound()){
                turn90('l');
            }else if(!areCellsEqual(frontCell,{-1,-1}) && grid[frontCell.x][frontCell.y] == manhattanDistance - 1 && !frontSonic.wallFound()){
                
            }else if(!areCellsEqual(rightCell,{-1,-1}) && grid[rightCell.x][rightCell.y] == manhattanDistance - 1 && !rightSonic.wallFound()){
                turn90('r');
            }else if(!areCellsEqual(backCell,{-1,-1}) && grid[backCell.x][backCell.y] == manhattanDistance - 1){
                turnBack();
            }else{break;}
            moveOneCell();
        }
    } //food fill end

}

void spectreLoop(){
        
    while(true){ //left hand rule

        // for(int i = 0;i < currentCell.x; i++){
        //     buzz();
        // }
        // digitalWrite(buzzer, HIGH);
        // delay(1000);
        // digitalWrite(buzzer, LOW);
        // delay(200);
        // for(int i = 0;i < currentCell.y; i++){
        //     buzz();
        // }

        updateWall(); 
        if(areCellsEqual(currentCell, end)){
            end = currentCell;
            turnBack();
            maze = mazeReverse(mazeShort(maze));
            break;
        }

        if(!leftSonic.wallFound()){
            turn90('l');
            maze += 'L';
        }else if(!frontSonic.wallFound()){
            if(junctionFound()){
                maze += 'S'; 
            }
        }else if(!rightSonic.wallFound()){
            turn90('r');
            maze += 'R';
        }else{
            maze += 'B';
            turnBack();
        }
        moveOneCell();

    } //left hand rule end
    
    int i = 0;
    while(true){
        if(areCellsEqual(currentCell,start)){ //coming back to start
            turnBack();
            break;
        }

        moveOneCell();

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

    while(true){
        loopFloodFill(end);
        floodFill.clearGrid();
        loopFloodFill(start);
        floodFill.clearGrid();
    }

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
    end = {5, 3};
    //Serial.print(mazeReverse(mazeShort(maze)));
    
    spectreLoop();
}