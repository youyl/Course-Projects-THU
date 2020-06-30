#ifndef GRID_H
#define GRID_H

#include <set>

using namespace std;

class Grid
{
public:
    const static int len=37;
    bool blocked;
    int cordup,cordleft;
    Grid();
    Grid(int,int,int);
    void reset();
};

int abs_dmfb(int x);

int distPnts(int x1,int y1,int x2,int y2);

#endif // GRID_H
