#ifndef QUERY_H
#define QUERY_H

//Move Input Output Expand Split Shrink Merge

#include <QString>

class Query
{
public:
    int x1,y1,x2,y2,x3,y3,alongRow;
    QString type;
    Query();
    QString translateString();
};

#endif // QUERY_H
