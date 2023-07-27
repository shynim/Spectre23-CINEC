#ifndef ROBOFEST2023_FLOODFILL_H
#define ROBOFEST2023_FLOODFILL_H

struct Cell {
    int x;
    int y;
};

class FloodFill{
    public:
        void floodFill(Cell endPos);
        
        void updateWall(Cell currentCell, bool leftWall, bool frontWall, bool RightWall, char orientation);
 
        void clearGrid();
        
    private:
        bool isCellAccesible(Cell parentCell, Cell childCell);
    
    
};

#endif //ROBOFEST2023_FLOODFILL_H
