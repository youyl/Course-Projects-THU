#include "query.h"

Query::Query()
{

}

QString Query::translateString()
{
    QString str;
    str=type;
    str+=" ";
    if(type=="Merge"||type=="Split"||type=="Expand"||type=="Shrink")
    {
        str+="(";
        str+=QString::number(x1);
        str+=",";
        str+=QString::number(y1);
        str+=") ";
        str+="(";
        str+=QString::number(x2);
        str+=",";
        str+=QString::number(y2);
        str+=") ";
        str+="(";
        str+=QString::number(x3);
        str+=",";
        str+=QString::number(y3);
        str+=")";

    }
    else if(type=="Mix"||type=="Move")
    {
        str+="(";
        str+=QString::number(x1);
        str+=",";
        str+=QString::number(y1);
        str+=") ";
        str+="(";
        str+=QString::number(x2);
        str+=",";
        str+=QString::number(y2);
        str+=")";
    }
    else
    {
        str+="(";
        str+=QString::number(x1);
        str+=",";
        str+=QString::number(y1);
        str+=")";
    }
    return str;
}
