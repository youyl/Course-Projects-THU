#ifndef CHESSGAME_H
#define CHESSGAME_H

#include <QObject>
#include <QDebug>
#include "promotedialog.h"
#include <QTextStream>
#include <string>
#include <QRect>

using namespace std;

class ChessGame : public QObject
{
    Q_OBJECT
public:
    QString myColor;
    QString col[10][10];
    QString piece[10][10];

    explicit ChessGame(QObject *parent = 0);

    QRect getCord(int,int);

    QString getColor(int,int);

    QString getPiece(int,int);

    int getStatus(int,int);

    void clickAction(int,int);

    void concedeAction();

    QString loadGame(QString);

    QString saveGame(bool);

    void setColor(QString);

    void iniGame();

    void receMess(int,int,int,int,QString);

private:
    int stage;
    int pickedx,pickedy;
    QRect cord[10][10];
    int status[10][10];
    PromoteDialog *promote;

    void setStatus(int,int);

signals:
    void movePiece(int,int,int,int,QString);
    void winThis();
    void loseThis();

public slots:
};

namespace ChessStage
{
    const int idle=0;
    const int pick=1;
}

namespace ChessStatus
{
    const int idle=0;
    const int picked=1;
    const int canmove=2;
    const int cancapture=3;
    const int castling=4;
}

namespace ChessCord
{
    QString trans(int,int);
    int getX(QString);
    int getY(QString);
}

#endif // CHESSGAME_H
