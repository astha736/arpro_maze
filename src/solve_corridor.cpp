#include <a_star.h>
#include <maze.h>
#include <iostream>

using namespace std;
using namespace ecn;

/* 
    right   -> y+1
    left    -> y-1
    up      -> x-1
    down    -> x+1
*/

// const int NUMBER_OF_POSE = 4;

// a node is a x-y position, we move from 1 each time
class Position : public Point
{
    typedef std::unique_ptr<Position> PositionPtr;
    typedef enum pose{ right, left, up, down }pose;
    const int NUMBER_OF_POSE = 4;
    const pose arrayPose[4] = {right, left, up, down};

public:
    int distance;
    Point neighbourList[4];

    // constructor from coordinates
    Position(int _x, int _y, int _distance=1) : Point(_x, _y) {
         distance = _distance;
    }

    // constructor from base ecn::Point
    Position(ecn::Point p,int _distance=1) : Point(p.x, p.y) {
        distance = _distance;
    }

    int distToParent()
    {
        return distance;
    }

    pose checkWhichNeighbour(const Point & parent){
        for(int i =0; i < NUMBER_OF_POSE; i++){
            if(neighbourList[i].x == parent.x && neighbourList[i].y == parent.y) return pose(i);
        }
    }

    void print(const Point &parent)
    {
        pose dirOfParent = checkWhichNeighbour(parent);

        Point pTemp = Point(this->x,this->y);

        maze.passThrough(pTemp.x, pTemp.y);

        pTemp = movePos(pTemp, dirOfParent);

        pose movingPose = dirOfParent;


        bool flagCorr = isCorridorOrCorner(pTemp,movingPose); 
        while(flagCorr == true){
            // keep going the corridor till parent is not found
            maze.passThrough(pTemp.x, pTemp.y);
            if(pTemp.x == parent.x && pTemp.y == parent.y) return;
            pTemp = movePos(pTemp,movingPose);
            flagCorr = isCorridorOrCorner(pTemp,movingPose); // checks if this is another corridor cell
        }
    }

    void show(bool closed, const Point & parent)
    {
        const int b = closed?255:0, r = closed?0:255;
        pose dirOfParent = checkWhichNeighbour(parent);

        Point pTemp = Point(this->x,this->y);
        maze.write(pTemp.x, pTemp.y, r, 0, b);
        pTemp = movePos(pTemp, dirOfParent);

        pose movingPose = dirOfParent;

        bool flagCorr = isCorridorOrCorner(pTemp,movingPose); 
        while(flagCorr == true){
            // keep going the corridor till parent is not found 
            maze.write(pTemp.x, pTemp.y, r, 0, b);
            if(pTemp.x == parent.x && pTemp.y == parent.y) return;
            pTemp = movePos(pTemp,movingPose);
            flagCorr = isCorridorOrCorner(pTemp,movingPose); // checks if this is another corridor cell
        }
        // otherwise it should be an error
        return;

    }


    Point movePos(const Point& p, const pose& _pos){

        Point movedPoint = p;
        switch (_pos){
                case right:
                    movedPoint.y = p.y + 1;
                    break;
                case left:
                    movedPoint.y = p.y -1;
                    break;
                case up:
                    movedPoint.x = p.x -1;
                    break;
                case down:
                    movedPoint.x = p.x + 1;
                    break;
        }
        return movedPoint;
    }

    int checkBlockedCorridor(const Point& _p,const pose& _pos){
        // 0+0 when both are walls, 1+0 when one of them is a wall, 1+1 both direction free
        if(_pos == right || _pos == left) return maze.cell(_p.x-1,_p.y) + maze.cell(_p.x+1,_p.y);
        else return maze.cell(_p.x,_p.y-1) + maze.cell(_p.x,_p.y+1);
    }

    pose whichCorrPosNotBlocked(const Point& _p,const pose& _pos){
        /* Assumption corner case: only one will be open */
        if(_pos == right || _pos == left){
            if(maze.cell(_p.x-1,_p.y) == 1) return pose(up);
            if(maze.cell(_p.x+1,_p.y) == 1) return pose(down);
        }
        else{
            if(maze.cell(_p.x,_p.y-1) == 1) return pose(left);
            if(maze.cell(_p.x,_p.y+1) == 1) return pose(right);
        }
    }

    bool isCorridorOrCorner(const Point& _p, pose& _pos){
        Point currentPosition = _p;
        Point frontNeighbour = movePos(currentPosition,_pos);

        if(maze.cell(frontNeighbour.x,frontNeighbour.y) == 0){
            // block in the moving direction 
            int temp = checkBlockedCorridor(currentPosition,_pos);
            if(temp == 1){
                // this is a corner, function returns the pose of the open corner( the direction :  _pos changes)
                pose tempPos = whichCorrPosNotBlocked(currentPosition,_pos);
                _pos = tempPos;
                return true;
            }
            else return false; // temp = 0: dead end temp= 2 intersection

        }
        else{
            int temp = checkBlockedCorridor(currentPosition,_pos);
            if(temp == 0){
                // straight corridor: _pos remains the same
                return true;
            }
            else return false; // temp = 1: => 2 branches and temp =2:  =>3 branches
        }

    }

    bool getTrueNeighbour(Point& _trueNeighibour, pose _pos, int& _move_distance){
        /* _x and _y will be changed to position of end the corridor */ 

        Point immediateParentNeighbour = movePos(Point(this->x,this->y),_pos);

        if(maze.cell(immediateParentNeighbour.x,immediateParentNeighbour.y) == 0)return false;
        else{
            // immediateParentNeighbour is not a wall
            Point lastValidNeighbour = immediateParentNeighbour;
            pose movingPose = _pos;

            // keep checking for corridor or corner 
            // the movingPose changes when there is a corner and remains same if a corridor
            bool flagCorr = isCorridorOrCorner(lastValidNeighbour,movingPose); 
            _move_distance = 1;

            // a loop for iteratively moving through a passage (corner + corridor)
            while(flagCorr == true){
                _move_distance++;
                lastValidNeighbour = movePos(lastValidNeighbour,movingPose);
                flagCorr = isCorridorOrCorner(lastValidNeighbour,movingPose);
            }

            // the lastValidNeighbour will always contain a true neighbour 
            // a deadend or intersection
            _trueNeighibour = lastValidNeighbour;
            return true;
        }
    }

    std::vector<PositionPtr> children()
    {
        // this method should return  all positions reachable from this one
        std::vector<PositionPtr> generated;

        Point validNeighbour; 

        // loop over  each possible position of the neighbours
        for ( int neighboursPos = 0; neighboursPos < NUMBER_OF_POSE; neighboursPos++ ){
            int move_distance;
            if(getTrueNeighbour(validNeighbour,arrayPose[neighboursPos],move_distance)){
                generated.push_back(PositionPtr(new Position(
                        validNeighbour,
                        move_distance)));
                neighbourList[neighboursPos] = validNeighbour;
            }
        }

        return generated;
    }
};



int main( int argc, char **argv ){
    // load file
    std::string filename = "maze.png";
    if(argc == 2)
        filename = std::string(argv[1]);

    // let Point know about this maze
    Position::maze = ecn::Maze(filename);

    // initial and goal positions as Position's
    Position start = Position::maze.start(),
             goal = Position::maze.end();

    // call A* algorithm
    ecn::Astar(start, goal);

    // save final image
    Position::maze.saveSolution("corridor");
    cv::waitKey(0);

}
