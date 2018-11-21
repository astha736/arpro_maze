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

    bool isCorridorLine(const Point& _p, pose _pos){
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

    

    int calculateCorridor(
        int _ypos=2, 
        int _yneg=2, 
        int _xpos=2, 
        int _xneg=2){

        int temp = _ypos + _yneg + _xpos + _xneg;
        temp = temp/2;
        if(temp == 0) return 0;
        if(temp == 1) return 1;


        dir1 = maze.cell(_p.x-1,_p.y);
        dir2 = maze.cell(_p.x+1,_p.y);
        dir3 = maze.cell(_p.x,_p.y+1);

        if((dir1 || dir2 || dir3 ) == 0){
            return 0;
            // this is a dead end
        }
        else{
            if((dir1 + dir2 + dir3 ) == 1){
                // this a corridor 
                if(dir1 == 1) return _pos = pose(up);
                if(dir2 == 1) return _pos = pose(down);
                if(dir3 == 1) return _pos = pose(right);
                return 1;

            }
            else return 2;
        }
        return _ypos + _yneg + _xpos + _xneg;

    }
    int isCorridor(const Point& _p, pose& _pos){
        // for each cell check if it has corridor opposite to the pose
        // !(left is not free || right is not free) => !(0||0) => 1
        bool corr = false;

        int dir1, dir2, dir3, dir4;

       switch (_pos){
                case right:{
                    calculateCorridor(2,0,2,2);
                    // calculateCorridor(_yneg=0);
                    dir1 = maze.cell(_p.x-1,_p.y);
                    dir2 = maze.cell(_p.x+1,_p.y);
                    dir3 = maze.cell(_p.x,_p.y+1);

                    if((dir1 || dir2 || dir3 ) == 0){
                        return 0;
                        // this is a dead end
                    }
                    else{
                        if((dir1 + dir2 + dir3 ) == 1){
                            // this a corridor 
                            if(dir1 == 1) return _pos = pose(up);
                            if(dir2 == 1) return _pos = pose(down);
                            if(dir3 == 1) return _pos = pose(right);
                            return 1;

                        }
                        else return 2;
                    }
                    break;

                }

                case left:{
                    dir1 = maze.cell(_p.x-1,_p.y);
                    dir2 = maze.cell(_p.x+1,_p.y);
                    dir4 = maze.cell(_p.x,_p.y-1);
                    if((dir1 || dir2 || dir4 ) == 0){
                        return 0;
                        // this is a dead end
                    }
                    else{
                        if((dir1 + dir2 + dir4 ) == 1){
                            // this a corridor 
                            if(dir1 == 1) return _pos = pose(up);
                            if(dir2 == 1) return _pos = pose(down);
                            if(dir4 == 1) return _pos = pose(left);
                            return 1;
                        }
                        else return 2;
                    }
                    break;
                }
                case up:{
                    dir1 = maze.cell(_p.x-1,_p.y);
                    dir3 = maze.cell(_p.x,_p.y+1);
                    dir4 = maze.cell(_p.x,_p.y-1);
                    if((dir1 || dir3 || dir4 ) == 0){
                        return 0;
                        // this is a dead end
                    }
                    else{
                        if((dir1 + dir3 + dir4 ) == 1){
                            // this a corridor 
                            if(dir1 == 1) return _pos = pose(up);
                            if(dir3 == 1) return _pos = pose(right);
                            if(dir4 == 1) return _pos = pose(left);
                            return 1;
                        }
                        else return 2;
                    }
                    break;
                }
                case down:{
                    dir2 = maze.cell(_p.x+1,_p.y);
                    dir3 = maze.cell(_p.x,_p.y+1);
                    dir4 = maze.cell(_p.x,_p.y-1);
                    if((dir2 || dir3 || dir4 ) == 0){
                        return 0;
                    }
                    else{
                        if((dir2 + dir3 + dir4 ) == 1){
                            // this a corridor 
                            if(dir2 == 1) return _pos = pose(down);
                            if(dir3 == 1) return _pos = pose(right);
                            if(dir4 == 1) return _pos = pose(left);
                            return 1;
                        }
                        else{
                            return 2;
                        }
                    }
                    break;
                }
        }
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

    // in one particular immediate direction from the parent
    bool getTrueNeighbour(Point& _validNeighbour, pose _pos){
        /* _x and _y will be changed to position of end the corridor */ 

        Point trueNeighibour = movePos(Point(this->x,this->y),_pos);
        std::cout << "NeighboursPos: "  << _pos << std::endl;
        std::cout << "NeighboursPos: "  << trueNeighibour.x <<":"  <<trueNeighibour.y << std::endl;

        if(maze.cell(trueNeighibour.x,trueNeighibour.y) == 0)return false;
        // bool flagCorr = isCorridor(trueNeighibour,_pos);
        pose newPose = _pos;
        int caseCorr = isCorridor(trueNeighibour,newPose);
        std::cout << "NeighboursPos: caseCorr: "  << caseCorr << std::endl;
        Point nextCorrNeigh;


        // while(caseCorr != 2){

        //     if(caseCorr == 0){
        //         // dead end 
        //         _validNeighbour = trueNeighibour;
        //     }
        //     else if(caseCorr == 1){
        //         // one corridor
        //         nextCorrNeigh = movePos(trueNeighibour,newPose);
        //     }
        //     trueNeighibour = nextCorrNeigh;
        //     caseCorr = isCorridor(trueNeighibour,newPose);; // checks if this is another corridor cel
        // }


        _validNeighbour = trueNeighibour;
        return true;

    }

    std::vector<PositionPtr> children()
    {
        // this method should return  all positions reachable from this one
        std::vector<PositionPtr> generated;

        Point validNeighbour; 

        std::cout << "********** for each parent ***************" << std::endl;

        for ( int neighboursPos = 0; neighboursPos < NUMBER_OF_POSE; neighboursPos++ ){
            std::cout << "********** for each neighboursPos "  << arrayPose[neighboursPos]<< std::endl;
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
