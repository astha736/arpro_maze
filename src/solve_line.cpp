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

// a node is a x-y position, we move from 1 each time
class Position : public Point
{
    typedef std::unique_ptr<Position> PositionPtr;
    typedef enum pose{ right, left, up, down }pose;
    const int NUMBER_OF_POSE = 4;
    const pose arrayPose[4] = {right, left, up, down};

public:
    int distance;
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

    bool isCorridor(const Point& _p, pose _pos){
        // for each cell check if it has corridor opposite to the pose
        // !(left is not free || right is not free) => !(0||0) => 1
        bool corr = false;
        if(_pos == right || _pos == left){
            corr = !( maze.cell(_p.x-1,_p.y) || maze.cell(_p.x+1,_p.y) );
        }
        else{
            corr = !( maze.cell(_p.x,_p.y-1) || maze.cell(_p.x,_p.y+1) );
        }

        return corr;
    }

    Point movePos(const Point& p, pose _pos){
        Point movedPoint = p;
        switch (_pos){
                case right:
                    movedPoint.y = movedPoint.y + 1;
                    break;
                case left:
                    movedPoint.y = movedPoint.y -1;
                    break;
                case up:
                    movedPoint.x = movedPoint.x -1;
                    break;
                case down:
                    movedPoint.x = movedPoint.x + 1;
                    break;
        }

        return movedPoint;
    }

    bool getTrueNeighbour(Point& _validNeighbour, pose _pos){
        /* _x and _y will be changed to position of end the corridor */ 

        Point trueNeighibour = movePos(Point(this->x,this->y),_pos);

        if(maze.cell(trueNeighibour.x,trueNeighibour.y) == 0)return false;
        bool flagCorr = isCorridor(trueNeighibour,_pos);

        while(flagCorr == true){
            Point nextCorrNeigh = movePos(trueNeighibour,_pos);

            // any time, after a corridor there is a wall, then it is a dead end
            if(maze.cell(nextCorrNeigh.x,nextCorrNeigh.y) == 0){
                // dead end nodes
                _validNeighbour = trueNeighibour;
                return true; 
            }
            else{

            }
            trueNeighibour = nextCorrNeigh;
            flagCorr = isCorridor(trueNeighibour,_pos); // checks if this is another corridor cell
        }
        _validNeighbour = trueNeighibour;
        return true;

    }

    std::vector<PositionPtr> children()
    {
        // this method should return  all positions reachable from this one
        std::vector<PositionPtr> generated;

        Point validNeighbour; 

        for ( int neighboursPos = 0; neighboursPos < NUMBER_OF_POSE; neighboursPos++ ){
            if(getTrueNeighbour(validNeighbour,arrayPose[neighboursPos])){
                generated.push_back(PositionPtr(new Position(
                        validNeighbour,
                        abs(this->x - validNeighbour.x)+abs(this->y - validNeighbour.y))));
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
    Position::maze.saveSolution("line");
    cv::waitKey(0);

}
