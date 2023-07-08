#include <Arduino.h>
#include <FloodFill.h>
#include <Variables.h>

//Cell startPos = {4,0};

void FloodFill::floodFill(Cell endPos) {
    Cell queue[mazeSize * mazeSize];
    bool visited[mazeSize][mazeSize] = {};
    int index = -1;
    int top = 0;

    queue[++index] = endPos;
    visited[endPos.x][endPos.y] = true;

    while (index >= top) {
        Cell selectedCell = queue[top++];
        int x = selectedCell.x;
        int y = selectedCell.y;

        Cell neighbouringCells[4] = {
            {x + 1, y},
            {x - 1, y},
            {x, y + 1},
            {x, y - 1}
        };

        for (int i = 0; i < 4; i++) {
            int xC = neighbouringCells[i].x;
            int yC = neighbouringCells[i].y;

            if (xC >= 0 && xC < mazeSize && yC >= 0 && yC < mazeSize && !visited[xC][yC] && isCellAccesible(selectedCell, neighbouringCells[i])) {
                queue[++index] = neighbouringCells[i];
                grid[xC][yC] = grid[x][y] + 1;
                visited[xC][yC] = true;
            }
        }
    }
}

void FloodFill::updateWall(Cell currentCell, bool leftWall, bool frontWall, bool rightWall, char orientation){
    int x = currentCell.x;
    int y = currentCell.y;

    
    if(orientation == 'f'){
        horizontalWallGrid[x][y] = frontWall;
        verticalWallGrid[x][y] = leftWall;
        verticalWallGrid[x][y+1] = rightWall;

    }else if(orientation == 'r'){
        verticalWallGrid[x][y+1] = frontWall;
        horizontalWallGrid[x][y] = leftWall;
        horizontalWallGrid[x+1][y] = rightWall;

    }else if(orientation == 'b'){
        horizontalWallGrid[x+1][y] = frontWall;
        verticalWallGrid[x][y+1] = leftWall;
        verticalWallGrid[x][y] = rightWall;

    }else if(orientation == 'l'){
        verticalWallGrid[x][y] = frontWall;
        horizontalWallGrid[x+1][y] = leftWall;
        horizontalWallGrid[x][y] = rightWall;
    }
    
}

bool FloodFill::isCellAccesible(Cell parentCell, Cell childCell){
    int xP = parentCell.x, yP = parentCell.y;
    int xC = childCell.x, yC = childCell.y;

    if(xP == xC){
        if(yP < yC){
            return !verticalWallGrid[xP][yP+1];
        }else{
            return !verticalWallGrid[xP][yP];
        }
    }else if(yP == yC){
        if(xP < xC){
            return !horizontalWallGrid[xP+1][yP];
        }else{
            return !horizontalWallGrid[xP][yP];
        }
    }
    return false; 
}

void FloodFill::clearGrid(){

    for (int i = 0; i < mazeSize; i++) {
        for (int j = 0; j < mazeSize; j++) {
            grid[i][j] = 0;
        }
    }

}