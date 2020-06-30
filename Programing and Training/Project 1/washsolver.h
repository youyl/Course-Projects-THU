#ifndef WASHSOLVER_H
#define WASHSOLVER_H

#include<vector>
#include<utility>
using namespace std;

class WashSolver
{
private:
    int row,col,sx,sy,tx,ty;
    int status[20][20];
    vector<pair<int,int> >res;
    vector<pair<int,int> >path;
    pair<int,int> fr[20][20];
public:
    WashSolver(int,int,int,int,int,int);
    void setBlock(int,int);
    void setBlockCenter(int,int);
    void setPollu(int,int);
    vector<pair<int,int> >solve();
    void getPath(int,int);
    bool expand();
};

#endif // WASHSOLVER_H
