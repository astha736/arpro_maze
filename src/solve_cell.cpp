#include <a_star.h>
#include <maze.h>

using namespace std;
using namespace ecn;

/*

Assignment for ARPRO: maze 
By: Astha

Approach:   check if the immediate neighbours are not wall, 
            if not, then create a node, add them into a vector 
            and return it as possible children of the given 
            Point/Position


Output: A image of solved maze  mazes/maze_cell.png 
that shows the taken path by the A* algorithm

*/

// a node is a x-y position, we move from 1 each time
class Position : public Point
{
    typedef std::unique_ptr<Position> PositionPtr;

public:
    // constructor from coordinates
    Position(int _x, int _y) : Point(_x, _y) {}

    // constructor from base ecn::Point
    Position(ecn::Point p) : Point(p.x, p.y) {}

    int distToParent()
    {
        // in cell-based motion, the distance to the parent is always 1
        return 1;
    }

    std::vector<PositionPtr> children()
    {
        // this method should return  all positions reachable from this one
        std::vector<PositionPtr> generated;

        /* 
            check if the neighbours left,right,up and down are free space(1)
            if yes, then create and push back the pointer for that location
        */
        if (maze.cell(this->x,this->y-1))
            generated.push_back(PositionPtr(new Position(this->x,this->y-1)));
        if (maze.cell(this->x,this->y+1))
            generated.push_back(PositionPtr(new Position(this->x,this->y+1)));
        if (maze.cell(this->x+1,this->y))
            generated.push_back(PositionPtr(new Position(this->x+1,this->y)));
        if (maze.cell(this->x-1,this->y))
            generated.push_back(PositionPtr(new Position(this->x-1,this->y)));

        
        return generated;
    }
};



int main( int argc, char **argv )
{
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
    Position::maze.saveSolution("cell");
    cv::waitKey(0);

}
