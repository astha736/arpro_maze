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

    int checkBlockedCorridor(const Point& _p, pose _pos){
        // for each cell check if it has corridor opposite to the pose
        // !(left is not free || right is not free) => !(0||0) => 1
        // bool corr = false;
        if(_pos == right || _pos == left){
            // corr = !( maze.cell(_p.x-1,_p.y) || maze.cell(_p.x+1,_p.y) );
            if( maze.cell(_p.x-1,_p.y)  == 0 && maze.cell(_p.x+1,_p.y) == 0) return 0;
            if( maze.cell(_p.x-1,_p.y)  == 1 && maze.cell(_p.x+1,_p.y) == 1) return 2;
            else return 1;

        }
        else{
            // corr = !( maze.cell(_p.x,_p.y-1) || maze.cell(_p.x,_p.y+1) );
            if( maze.cell(_p.x,_p.y-1)  == 0 && maze.cell(_p.x,_p.y+1) == 0) return 0;
            if( maze.cell(_p.x,_p.y-1)  == 1 && maze.cell(_p.x,_p.y+1) == 1) return 2;
            else return 1;
        }
    }

    pose whichCorrPosNotBlocked(const Point& _p, pose _pos){
        /* Assumption only one will be blocked */

        Point temp = _p;

        if(_pos == right || _pos == left){
            if(maze.cell(_p.x-1,_p.y) == 1) return pose(up);
            if(maze.cell(_p.x+1,_p.y) == 1) return pose(down);
        }
        else{
            if(maze.cell(_p.x,_p.y-1) == 1) return pose(left);
            if(maze.cell(_p.x,_p.y+1) == 1) return pose(right);
        }
    }

    bool isCorridor(const Point& _p, pose& _pos){
        Point immediateNeighbour = p;
        Point nextfrontNeighbour = movePos(p,_pos);
        if(maze.cell(nextfrontNeighbour.x,nextfrontNeighbour.y) == 0){
        // block in the same direction 
            int temp = checkBlockedCorridor(immediateNeighbour,_pos)
            if(temp == 0){
                // both corridor blocked
                // dead end
                return false;
            }
            if(temp === 1){
                // this one corridor open
                // _pos changes
                Pose tempPos = whichCorrPosNotBlocked(immediateNeighbour,_pos);
                _pos = tempPos;
                return true;
            }
            else{
                // intersection is there
                return false; 

            }

        }
        else{
            int temp = checkBlockedCorridor(immediateNeighbour,_pos)
            if(temp == 0){
                // both corridor blocked
                // straight corridor
                // _pos remains the same
                return true;
            }
            if(temp === 1){
                // 2 branches 
                return false;
            }
            else{
                // 3 branches
                return false

            }
        }

            // check if corridor

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
