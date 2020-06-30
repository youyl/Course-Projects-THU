#include "droplet.h"


Droplet::Droplet()
{
    isExpand=false;
    alongRow=false;
    isdisabled=false;
}

Droplet::Droplet(int _id,int _x,int _y)
{
    idx=_id;
    x=_x;
    y=_y;
    isExpand=false;
    alongRow=false;
    isdisabled=false;
}

void Droplet::dropDebug()
{
    qDebug()<<"id = "<<idx<<"  pos = "<<x<<" , "<<y<<endl;
}

//https://stackoverflow.com/questions/470690/how-to-automatically-generate-n-distinct-colors
const QColor presetC[20]={
    QColor(255, 179, 0),
    QColor(129, 112, 102),
    QColor(255, 122, 92),
    QColor(83, 55, 122),
    QColor(128, 62, 117),
    QColor(0, 125, 52),
    QColor(35, 44, 22),
    QColor(255, 142, 0),
    QColor(89, 51, 21),
    QColor(246, 118, 142),
    QColor(241, 58, 19),
    QColor(206, 162, 98),
    QColor(147, 170, 0),
    QColor(127, 24, 13),
    QColor(193, 0, 32),
    QColor(0, 83, 138),
    QColor(166, 189, 215),
    QColor(244, 200, 0),
    QColor(255, 104, 0),
    QColor(179, 40, 81)
};

QColor presetColor(int x)
{
    return presetC[x%20];
}
