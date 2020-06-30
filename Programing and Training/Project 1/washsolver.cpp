#include "washsolver.h"
#include <queue>
#include <QDebug>


WashSolver::WashSolver(int _r,int _c,int _sx,int _sy,int _tx,int _ty)
{
    row=_r;
    col=_c;
    sx=_sx;
    sy=_sy;
    tx=_tx;
    ty=_ty;
    for (int i=0;i<=row+1;i++)
    {
        for (int j=0;j<=col+1;j++)
        {
            if(i==0)status[i][j]=1;
            else if(j==0)status[i][j]=1;
            else if(i==row+1)status[i][j]=1;
            else if(j==col+1)status[i][j]=1;
            else status[i][j]=0;
        }
    }
    status[sx][sy]=0;
}

void WashSolver::setBlock(int x,int y)
{
    status[x][y]=1;
}

void WashSolver::setBlockCenter(int x,int y)
{
    for (int i=x-1;i<=x+1;i++)
    {
        for (int j=y-1;j<=y+1;j++)
        {
            status[i][j]=1;
        }
    }
}

void WashSolver::setPollu(int x,int y)
{
    if(status[x][y]==0)
    {
        status[x][y]=2;
    }
}

const int dir[4][2]={{1,0},{-1,0},{0,1},{0,-1}};

bool WashSolver::expand()
{
    queue<pair<int,int> >q;
    q.push(path[path.size()-1]);
    for (int i=0;i<=row+1;i++)
    {
        for (int j=0;j<=col+1;j++)
        {
            fr[i][j]=make_pair(-1,-1);
        }
    }
    fr[q.front().first][q.front().second]=make_pair(0,0);
    while (!q.empty())
    {
        int x=q.front().first;
        int y=q.front().second;
        q.pop();
        if(status[x][y]==2)
        {
            getPath(x,y);
            status[x][y]=0;
            return true;
        }
        for (int i=0;i<4;i++)
        {
            x+=dir[i][0];
            y+=dir[i][1];
            if(x>=0&&y>=0&&x<=row+1&&y<=col+1)
            {
                if(status[x][y]!=1&&fr[x][y]==make_pair(-1,-1))
                {
                    q.push(make_pair(x,y));
                    fr[x][y]=make_pair(x-dir[i][0],y-dir[i][1]);
                }
            }
            x-=dir[i][0];
            y-=dir[i][1];
        }
    }return false;
}

void WashSolver::getPath(int x,int y)
{
    if(fr[x][y]==make_pair(0,0))return;
    getPath(fr[x][y].first,fr[x][y].second);
    path.push_back(make_pair(x,y));
}

vector<pair<int,int> >WashSolver::solve()
{
    path.push_back(make_pair(sx,sy));
    while (expand());
    status[tx][ty]=2;
    qDebug()<<sx<<' '<<sy<<' '<<tx<<' '<<ty;
    expand();
    return path;
}
