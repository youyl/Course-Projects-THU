#ifndef CASTLINGCHECK_H
#define CASTLINGCHECK_H

#include "chessgame.h"
#include <QString>

class CastlingCheck
{
public:
    QString col[10][10];
    QString piece[10][10];
    bool threat[10][10];
    QString myColor;
    CastlingCheck(const ChessGame&);
    bool checkShortCastling();
    bool checkLongCastling();
    void setThreat(int,int);
};

#endif // CASTLINGCHECK_H
