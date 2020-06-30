#ifndef DROPLET_H
#define DROPLET_H

#include <QPainter>
#include <QDebug>

class Droplet
{
public:
    int idx,x,y;
    bool isExpand,alongRow,isdisabled;
    Droplet();
    Droplet(int,int,int);
    void dropDebug();
};

QColor presetColor(int x);

#endif // DROPLET_H
