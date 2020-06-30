#include "grid.h"

Grid::Grid()
{
    blocked=false;
}

void Grid::reset()
{
    blocked=false;
}

int abs_dmfb(int x)
{
    if(x>0)return x;
    return -x;
}

int distPnts(int x1,int y1,int x2,int y2)
{
    return abs_dmfb(x1-x2)+abs_dmfb(y1-y2);
}
