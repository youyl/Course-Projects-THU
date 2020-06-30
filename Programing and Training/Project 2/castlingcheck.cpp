#include "castlingcheck.h"


CastlingCheck::CastlingCheck(const ChessGame &tmp)
{
    myColor=tmp.myColor;
    for (int i=1;i<=8;i++)
    {
        for (int j=1;j<=8;j++)
        {
            col[i][j]=tmp.col[i][j];
            piece[i][j]=tmp.piece[i][j];
            threat[i][j]=0;
        }
    }
    for (int i=1;i<=8;i++)
    {
        for (int j=1;j<=8;j++)
        {
            if(col[i][j]!="idle"&&col[i][j]!=myColor)
            {
                if(myColor=="black")myColor="white";
                else myColor="black";
                setThreat(i,j);
                if(myColor=="black")myColor="white";
                else myColor="black";
            }
        }
    }
}

void CastlingCheck::setThreat(int x,int y)
{
    QString pic=piece[x][y];
    if(pic=="king")
    {
        for (int i=x-1;i<=x+1;i++)
        {
            for (int j=y-1;j<=y+1;j++)
            {
                if(i<1||i>8||j<1||j>8)continue;
                if(col[i][j]==myColor)continue;
                if(col[i][j]=="idle")
                {
                    threat[i][j]=1;
                }
                else
                {
                    threat[i][j]=1;
                }
            }
        }
    }
    else if(pic=="queen")
    {
        int sx=x,sy=y;
        sx--;sy--;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")threat[sx][sy]=1;
            else
            {
                if(col[sx][sy]==myColor);
                else threat[sx][sy]=1;
                break;
            }
            sx--;sy--;
        }
        sx=x,sy=y;
        sx--;sy++;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")threat[sx][sy]=1;
            else
            {
                if(col[sx][sy]==myColor);
                else threat[sx][sy]=1;
                break;
            }
            sx--;sy++;
        }
        sx=x,sy=y;
        sx++;sy--;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")threat[sx][sy]=1;
            else
            {
                if(col[sx][sy]==myColor);
                else threat[sx][sy]=1;
                break;
            }
            sx++;sy--;
        }
        sx=x,sy=y;
        sx++;sy++;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")threat[sx][sy]=1;
            else
            {
                if(col[sx][sy]==myColor);
                else threat[sx][sy]=1;
                break;
            }
            sx++;sy++;
        }
        sx=x,sy=y;
        sx--;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")threat[sx][sy]=1;
            else
            {
                if(col[sx][sy]==myColor);
                else threat[sx][sy]=1;
                break;
            }
            sx--;
        }
        sx=x,sy=y;
        sx++;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")threat[sx][sy]=1;
            else
            {
                if(col[sx][sy]==myColor);
                else threat[sx][sy]=1;
                break;
            }
            sx++;
        }
        sx=x,sy=y;
        sy--;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")threat[sx][sy]=1;
            else
            {
                if(col[sx][sy]==myColor);
                else threat[sx][sy]=1;
                break;
            }
            sy--;
        }
        sx=x,sy=y;
        sy++;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")threat[sx][sy]=1;
            else
            {
                if(col[sx][sy]==myColor);
                else threat[sx][sy]=1;
                break;
            }
            sy++;
        }
    }
    else if(pic=="knight")
    {
        int sx=x,sy=y;
        sx=x+1;sy=y+2;
        if(sx<1||sy<1||sx>8||sy>8);
        else if(col[sx][sy]=="idle")threat[sx][sy]=1;
        else if(col[sx][sy]!=myColor)threat[sx][sy]=1;
        sx=x+1;sy=y-2;
        if(sx<1||sy<1||sx>8||sy>8);
        else if(col[sx][sy]=="idle")threat[sx][sy]=1;
        else if(col[sx][sy]!=myColor)threat[sx][sy]=1;
        sx=x-1;sy=y+2;
        if(sx<1||sy<1||sx>8||sy>8);
        else if(col[sx][sy]=="idle")threat[sx][sy]=1;
        else if(col[sx][sy]!=myColor)threat[sx][sy]=1;
        sx=x-1;sy=y-2;
        if(sx<1||sy<1||sx>8||sy>8);
        else if(col[sx][sy]=="idle")threat[sx][sy]=1;
        else if(col[sx][sy]!=myColor)threat[sx][sy]=1;
        sx=x+2;sy=y+1;
        if(sx<1||sy<1||sx>8||sy>8);
        else if(col[sx][sy]=="idle")threat[sx][sy]=1;
        else if(col[sx][sy]!=myColor)threat[sx][sy]=1;
        sx=x+2;sy=y-1;
        if(sx<1||sy<1||sx>8||sy>8);
        else if(col[sx][sy]=="idle")threat[sx][sy]=1;
        else if(col[sx][sy]!=myColor)threat[sx][sy]=1;
        sx=x-2;sy=y+1;
        if(sx<1||sy<1||sx>8||sy>8);
        else if(col[sx][sy]=="idle")threat[sx][sy]=1;
        else if(col[sx][sy]!=myColor)threat[sx][sy]=1;
        sx=x-2;sy=y-1;
        if(sx<1||sy<1||sx>8||sy>8);
        else if(col[sx][sy]=="idle")threat[sx][sy]=1;
        else if(col[sx][sy]!=myColor)threat[sx][sy]=1;
    }
    else if(pic=="bishop")
    {
        int sx=x,sy=y;
        sx--;sy--;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")threat[sx][sy]=1;
            else
            {
                if(col[sx][sy]==myColor);
                else threat[sx][sy]=1;
                break;
            }
            sx--;sy--;
        }
        sx=x,sy=y;
        sx--;sy++;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")threat[sx][sy]=1;
            else
            {
                if(col[sx][sy]==myColor);
                else threat[sx][sy]=1;
                break;
            }
            sx--;sy++;
        }
        sx=x,sy=y;
        sx++;sy--;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")threat[sx][sy]=1;
            else
            {
                if(col[sx][sy]==myColor);
                else threat[sx][sy]=1;
                break;
            }
            sx++;sy--;
        }
        sx=x,sy=y;
        sx++;sy++;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")threat[sx][sy]=1;
            else
            {
                if(col[sx][sy]==myColor);
                else threat[sx][sy]=1;
                break;
            }
            sx++;sy++;
        }
    }
    else if(pic=="rook")
    {
        int sx=x,sy=y;
        sx--;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")threat[sx][sy]=1;
            else
            {
                if(col[sx][sy]==myColor);
                else threat[sx][sy]=1;
                break;
            }
            sx--;
        }
        sx=x,sy=y;
        sx++;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")threat[sx][sy]=1;
            else
            {
                if(col[sx][sy]==myColor);
                else threat[sx][sy]=1;
                break;
            }
            sx++;
        }
        sx=x,sy=y;
        sy--;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")threat[sx][sy]=1;
            else
            {
                if(col[sx][sy]==myColor);
                else threat[sx][sy]=1;
                break;
            }
            sy--;
        }
        sx=x,sy=y;
        sy++;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")threat[sx][sy]=1;
            else
            {
                if(col[sx][sy]==myColor);
                else threat[sx][sy]=1;
                break;
            }
            sy++;
        }
    }
    else if(pic=="pawn")
    {
        if(myColor=="white")
        {
            if(x>1)
            {
                if(col[x-1][y]=="idle")
                {
                    threat[x-1][y]=1;
                    if(x==7)
                    {
                        if(col[x-2][y]=="idle")
                        {
                            threat[x-2][y]=1;
                        }
                    }
                }
                if(y!=1)
                {
                    if(col[x-1][y-1]=="black")threat[x-1][y-1]=1;
                }
                if(y!=8)
                {
                    if(col[x-1][y+1]=="black")threat[x-1][y+1]=1;
                }
            }
        }
        else
        {
            if(x<8)
            {
                if(col[x+1][y]=="idle")
                {
                    threat[x+1][y]=1;
                    if(x==2)
                    {
                        if(col[x+2][y]=="idle")
                        {
                            threat[x+2][y]=1;
                        }
                    }
                }
                if(y!=1)
                {
                    if(col[x+1][y-1]=="white")threat[x+1][y-1]=1;
                }
                if(y!=8)
                {
                    if(col[x+1][y+1]=="white")threat[x+1][y+1]=1;
                }
            }
        }
    }
    threat[x][y]=1;
}

bool CastlingCheck::checkShortCastling()
{
    int xcord;
    if(myColor=="white")xcord=8;
    else xcord=1;
    if(col[xcord][8]!=myColor)return false;
    if(col[xcord][5]!=myColor)return false;
    if(piece[xcord][8]!="rook")return false;
    if(piece[xcord][5]!="king")return false;
    if(threat[xcord][6])return false;
    if(threat[xcord][7])return false;
    if(threat[xcord][5])return false;
    if(col[xcord][6]!="idle")return false;
    if(col[xcord][7]!="idle")return false;
    return true;
}

bool CastlingCheck::checkLongCastling()
{
    int xcord;
    if(myColor=="white")xcord=8;
    else xcord=1;
    if(col[xcord][1]!=myColor)return false;
    if(col[xcord][5]!=myColor)return false;
    if(piece[xcord][1]!="rook")return false;
    if(piece[xcord][5]!="king")return false;
    if(threat[xcord][3])return false;
    if(threat[xcord][4])return false;
    if(threat[xcord][5])return false;
    if(col[xcord][2]!="idle")return false;
    if(col[xcord][3]!="idle")return false;
    if(col[xcord][4]!="idle")return false;
    return true;
}
