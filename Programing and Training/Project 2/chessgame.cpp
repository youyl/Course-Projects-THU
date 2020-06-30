#include "chessgame.h"
#include "castlingcheck.h"
#include <QFile>

ChessGame::ChessGame(QObject *parent) : QObject(parent)
{
    for (int i=1;i<=8;i++)
    {
        for (int j=1;j<=8;j++)
        {
            cord[i][j]=QRect(60*j-10,60*i+10,60,60);
            col[i][j]="idle";
            piece[i][j]="idle";
            status[i][j]=ChessStatus::idle;
        }
    }
    stage=ChessStage::idle;
    pickedx=0;
    pickedy=0;
}

void ChessGame::receMess(int x1,int y1,int x2,int y2,QString str)
{
    col[x2][y2]=col[x1][y1];
    piece[x2][y2]=piece[x1][y1];
    col[x1][y1]="idle";
    piece[x1][y1]="idle";
    if(str!="pawn")
    {
        piece[x2][y2]=str;
    }
}

void ChessGame::iniGame()
{
    for (int i=1;i<=8;i++)
    {
        for (int j=1;j<=8;j++)
        {
            col[i][j]="idle";
            piece[i][j]="idle";
            status[i][j]=ChessStatus::idle;
        }
    }
    stage=ChessStage::idle;
    pickedx=0;
    pickedy=0;
    QString path=":/board/start_stat.txt";
    QFile *file=new QFile(path);
    if(!(file->open(QIODevice::ReadOnly | QIODevice::Text)))
    {
        qDebug()<<"fail to load start states";
        return;
    }
    QTextStream in(file);
    this->loadGame(in.readAll());
    file->close();
}

QRect ChessGame::getCord(int x,int y)
{
    return cord[x][y];
}

QString ChessGame::getColor(int x,int y)
{
    return col[x][y];
}

QString ChessGame::getPiece(int x,int y)
{
    return piece[x][y];
}

int ChessGame::getStatus(int x,int y)
{
    return status[x][y];
}

void ChessGame::setStatus(int x,int y)
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
                    status[i][j]=ChessStatus::canmove;
                }
                else
                {
                    status[i][j]=ChessStatus::cancapture;
                }
            }
        }
        CastlingCheck *pts=new CastlingCheck((*this));
        if(myColor=="white")
        {
            if(pts->checkLongCastling())
            {
                status[8][3]=ChessStatus::castling;
            }
            if(pts->checkShortCastling())
            {
                status[8][7]=ChessStatus::castling;
            }
        }
        else
        {
            if(pts->checkLongCastling())
            {
                status[1][3]=ChessStatus::castling;
            }
            if(pts->checkShortCastling())
            {
                status[1][7]=ChessStatus::castling;
            }
        }
    }
    else if(pic=="queen")
    {
        int sx=x,sy=y;
        sx--;sy--;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
            else
            {
                if(col[sx][sy]==myColor);
                else status[sx][sy]=ChessStatus::cancapture;
                break;
            }
            sx--;sy--;
        }
        sx=x,sy=y;
        sx--;sy++;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
            else
            {
                if(col[sx][sy]==myColor);
                else status[sx][sy]=ChessStatus::cancapture;
                break;
            }
            sx--;sy++;
        }
        sx=x,sy=y;
        sx++;sy--;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
            else
            {
                if(col[sx][sy]==myColor);
                else status[sx][sy]=ChessStatus::cancapture;
                break;
            }
            sx++;sy--;
        }
        sx=x,sy=y;
        sx++;sy++;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
            else
            {
                if(col[sx][sy]==myColor);
                else status[sx][sy]=ChessStatus::cancapture;
                break;
            }
            sx++;sy++;
        }
        sx=x,sy=y;
        sx--;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
            else
            {
                if(col[sx][sy]==myColor);
                else status[sx][sy]=ChessStatus::cancapture;
                break;
            }
            sx--;
        }
        sx=x,sy=y;
        sx++;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
            else
            {
                if(col[sx][sy]==myColor);
                else status[sx][sy]=ChessStatus::cancapture;
                break;
            }
            sx++;
        }
        sx=x,sy=y;
        sy--;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
            else
            {
                if(col[sx][sy]==myColor);
                else status[sx][sy]=ChessStatus::cancapture;
                break;
            }
            sy--;
        }
        sx=x,sy=y;
        sy++;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
            else
            {
                if(col[sx][sy]==myColor);
                else status[sx][sy]=ChessStatus::cancapture;
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
        else if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
        else if(col[sx][sy]!=myColor)status[sx][sy]=ChessStatus::cancapture;
        sx=x+1;sy=y-2;
        if(sx<1||sy<1||sx>8||sy>8);
        else if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
        else if(col[sx][sy]!=myColor)status[sx][sy]=ChessStatus::cancapture;
        sx=x-1;sy=y+2;
        if(sx<1||sy<1||sx>8||sy>8);
        else if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
        else if(col[sx][sy]!=myColor)status[sx][sy]=ChessStatus::cancapture;
        sx=x-1;sy=y-2;
        if(sx<1||sy<1||sx>8||sy>8);
        else if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
        else if(col[sx][sy]!=myColor)status[sx][sy]=ChessStatus::cancapture;
        sx=x+2;sy=y+1;
        if(sx<1||sy<1||sx>8||sy>8);
        else if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
        else if(col[sx][sy]!=myColor)status[sx][sy]=ChessStatus::cancapture;
        sx=x+2;sy=y-1;
        if(sx<1||sy<1||sx>8||sy>8);
        else if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
        else if(col[sx][sy]!=myColor)status[sx][sy]=ChessStatus::cancapture;
        sx=x-2;sy=y+1;
        if(sx<1||sy<1||sx>8||sy>8);
        else if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
        else if(col[sx][sy]!=myColor)status[sx][sy]=ChessStatus::cancapture;
        sx=x-2;sy=y-1;
        if(sx<1||sy<1||sx>8||sy>8);
        else if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
        else if(col[sx][sy]!=myColor)status[sx][sy]=ChessStatus::cancapture;
    }
    else if(pic=="bishop")
    {
        int sx=x,sy=y;
        sx--;sy--;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
            else
            {
                if(col[sx][sy]==myColor);
                else status[sx][sy]=ChessStatus::cancapture;
                break;
            }
            sx--;sy--;
        }
        sx=x,sy=y;
        sx--;sy++;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
            else
            {
                if(col[sx][sy]==myColor);
                else status[sx][sy]=ChessStatus::cancapture;
                break;
            }
            sx--;sy++;
        }
        sx=x,sy=y;
        sx++;sy--;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
            else
            {
                if(col[sx][sy]==myColor);
                else status[sx][sy]=ChessStatus::cancapture;
                break;
            }
            sx++;sy--;
        }
        sx=x,sy=y;
        sx++;sy++;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
            else
            {
                if(col[sx][sy]==myColor);
                else status[sx][sy]=ChessStatus::cancapture;
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
            if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
            else
            {
                if(col[sx][sy]==myColor);
                else status[sx][sy]=ChessStatus::cancapture;
                break;
            }
            sx--;
        }
        sx=x,sy=y;
        sx++;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
            else
            {
                if(col[sx][sy]==myColor);
                else status[sx][sy]=ChessStatus::cancapture;
                break;
            }
            sx++;
        }
        sx=x,sy=y;
        sy--;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
            else
            {
                if(col[sx][sy]==myColor);
                else status[sx][sy]=ChessStatus::cancapture;
                break;
            }
            sy--;
        }
        sx=x,sy=y;
        sy++;
        while (sx>=1&&sx<=8&&sy>=1&&sy<=8)
        {
            if(col[sx][sy]=="idle")status[sx][sy]=ChessStatus::canmove;
            else
            {
                if(col[sx][sy]==myColor);
                else status[sx][sy]=ChessStatus::cancapture;
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
                    status[x-1][y]=ChessStatus::canmove;
                    if(x==7)
                    {
                        if(col[x-2][y]=="idle")
                        {
                            status[x-2][y]=ChessStatus::canmove;
                        }
                    }
                }
                if(y!=1)
                {
                    if(col[x-1][y-1]=="black")status[x-1][y-1]=ChessStatus::cancapture;
                }
                if(y!=8)
                {
                    if(col[x-1][y+1]=="black")status[x-1][y+1]=ChessStatus::cancapture;
                }
            }
        }
        else
        {
            if(x<8)
            {
                if(col[x+1][y]=="idle")
                {
                    status[x+1][y]=ChessStatus::canmove;
                    if(x==2)
                    {
                        if(col[x+2][y]=="idle")
                        {
                            status[x+2][y]=ChessStatus::canmove;
                        }
                    }
                }
                if(y!=1)
                {
                    if(col[x+1][y-1]=="white")status[x+1][y-1]=ChessStatus::cancapture;
                }
                if(y!=8)
                {
                    if(col[x+1][y+1]=="white")status[x+1][y+1]=ChessStatus::cancapture;
                }
            }
        }
    }
    status[x][y]=ChessStatus::picked;
}

void ChessGame::clickAction(int x,int y)
{
    if(stage==ChessStage::idle)
    {
        if(col[x][y]==myColor)
        {
            pickedx=x;
            pickedy=y;
            stage=ChessStage::pick;
            setStatus(x,y);
        }
    }
    else if(stage==ChessStage::pick)
    {
        bool fllg=false;
        QString str="pawn";
        if(status[x][y]==ChessStatus::cancapture||status[x][y]==ChessStatus::canmove)
        {
            col[x][y]=col[pickedx][pickedy];
            if(piece[x][y]=="king")
            {
                piece[x][y]=piece[pickedx][pickedy];
                col[pickedx][pickedy]="idle";
                emit winThis();
                return;
            }
            piece[x][y]=piece[pickedx][pickedy];
            col[pickedx][pickedy]="idle";
            piece[pickedx][pickedy]="idle";
            fllg=true;
            if(piece[x][y]=="pawn")
            {
                if(x==1&&col[x][y]=="white")
                {
                    promote=new PromoteDialog;
                    promote->exec();
                    str=promote->answer;
                    qDebug()<<"promote res: "<<str;
                    delete promote;
                }
                if(x==8&&col[x][y]=="black")
                {
                    promote=new PromoteDialog;
                    promote->exec();
                    str=promote->answer;
                    qDebug()<<"promote res: "<<str;
                    delete promote;
                }
                if(str=="concede")
                {
                    concedeAction();
                    return;
                }
                else
                {
                    piece[x][y]=str;
                }
            }
        }
        else if(status[x][y]==ChessStatus::castling)
        {
            fllg=true;
            if(y==3)
            {
                receMess(x,5,x,3,"pawn");
                receMess(x,1,x,4,"pawn");
            }
            else
            {
                receMess(x,5,x,7,"pawn");
                receMess(x,8,x,6,"pawn");
            }
            str="castling";
        }
        for (int i=1;i<=8;i++)
        {
            for (int j=1;j<=8;j++)
            {
                status[i][j]=ChessStatus::idle;
            }
        }
        if(fllg)
        {
            emit movePiece(pickedx,pickedy,x,y,str);
        }
        pickedx=0;pickedy=0;
        stage=ChessStage::idle;
    }
}

void ChessGame::setColor(QString str)
{
    myColor=str;
}

void ChessGame::concedeAction()
{
    emit loseThis();
}

QString ChessGame::loadGame(QString str)
{
    qDebug()<<str;
    QString curColor,curPiece,curCord;
    for (int i=1;i<=8;i++)
    {
        for (int j=1;j<=8;j++)
        {
            col[i][j]="idle";
            piece[i][j]="idle";
            status[i][j]=ChessStatus::idle;
        }
    }
    stage=ChessStage::idle;
    pickedx=0;
    pickedy=0;
    int curNum;
    QTextStream readstr(str.toUtf8());
    while (!readstr.atEnd())
    {
        readstr>>str;
        if(str=="white")curColor="white";
        else if(str=="black")curColor="black";
        else if(str=="king"||str=="queen"||str=="rook"||str=="knight"||str=="bishop"||str=="pawn")
        {
            curPiece=str;
            readstr>>curNum;
            for (int i=0;i<curNum;i++)
            {
                readstr>>curCord;
                int x=ChessCord::getX(curCord);
                int y=ChessCord::getY(curCord);
                col[x][y]=curColor;
                piece[x][y]=curPiece;
            }
        }
    }
    if(curColor=="white")return "black";
    if(curColor=="black")return "white";
}

QString ChessGame::saveGame(bool isrunning)
{
    QString str;
    QTextStream writestr(&str);
    if((isrunning==true&&myColor=="white")||(isrunning==false&&myColor=="black"))
    {
        writestr<<"white"<<endl;
        int cnt=0;
        //white king
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="king"&&col[i][j]=="white")cnt++;
        if(cnt!=0)
        {
            writestr<<"king "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="king"&&col[i][j]=="white")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //white queen
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="queen"&&col[i][j]=="white")cnt++;
        if(cnt!=0)
        {
            writestr<<"queen "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="queen"&&col[i][j]=="white")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //white rook
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="rook"&&col[i][j]=="white")cnt++;
        if(cnt!=0)
        {
            writestr<<"rook "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="rook"&&col[i][j]=="white")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //white knight
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="knight"&&col[i][j]=="white")cnt++;
        if(cnt!=0)
        {
            writestr<<"knight "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="knight"&&col[i][j]=="white")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //white pawn
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="pawn"&&col[i][j]=="white")cnt++;
        if(cnt!=0)
        {
            writestr<<"pawn "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="pawn"&&col[i][j]=="white")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //white bishop
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="bishop"&&col[i][j]=="white")cnt++;
        if(cnt!=0)
        {
            writestr<<"bishop "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="bishop"&&col[i][j]=="white")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //black king
        writestr<<"black"<<endl;
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="king"&&col[i][j]=="black")cnt++;
        if(cnt!=0)
        {
            writestr<<"king "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="king"&&col[i][j]=="black")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //black queen
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="queen"&&col[i][j]=="black")cnt++;
        if(cnt!=0)
        {
            writestr<<"queen "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="queen"&&col[i][j]=="black")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //black rook
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="rook"&&col[i][j]=="black")cnt++;
        if(cnt!=0)
        {
            writestr<<"rook "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="rook"&&col[i][j]=="black")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //black knight
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="knight"&&col[i][j]=="black")cnt++;
        if(cnt!=0)
        {
            writestr<<"knight "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="knight"&&col[i][j]=="black")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //black pawn
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="pawn"&&col[i][j]=="black")cnt++;
        if(cnt!=0)
        {
            writestr<<"pawn "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="pawn"&&col[i][j]=="black")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //black bishop
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="bishop"&&col[i][j]=="black")cnt++;
        if(cnt!=0)
        {
            writestr<<"bishop "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="bishop"&&col[i][j]=="black")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
    }
    else
    {
        int cnt=0;
        //black king
        writestr<<"black"<<endl;
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="king"&&col[i][j]=="black")cnt++;
        if(cnt!=0)
        {
            writestr<<"king "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="king"&&col[i][j]=="black")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //black queen
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="queen"&&col[i][j]=="black")cnt++;
        if(cnt!=0)
        {
            writestr<<"queen "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="queen"&&col[i][j]=="black")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //black rook
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="rook"&&col[i][j]=="black")cnt++;
        if(cnt!=0)
        {
            writestr<<"rook "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="rook"&&col[i][j]=="black")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //black knight
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="knight"&&col[i][j]=="black")cnt++;
        if(cnt!=0)
        {
            writestr<<"knight "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="knight"&&col[i][j]=="black")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //black pawn
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="pawn"&&col[i][j]=="black")cnt++;
        if(cnt!=0)
        {
            writestr<<"pawn "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="pawn"&&col[i][j]=="black")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //black bishop
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="bishop"&&col[i][j]=="black")cnt++;
        if(cnt!=0)
        {
            writestr<<"bishop "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="bishop"&&col[i][j]=="black")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //white king
        writestr<<"white"<<endl;
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="king"&&col[i][j]=="white")cnt++;
        if(cnt!=0)
        {
            writestr<<"king "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="king"&&col[i][j]=="white")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //white queen
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="queen"&&col[i][j]=="white")cnt++;
        if(cnt!=0)
        {
            writestr<<"queen "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="queen"&&col[i][j]=="white")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //white rook
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="rook"&&col[i][j]=="white")cnt++;
        if(cnt!=0)
        {
            writestr<<"rook "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="rook"&&col[i][j]=="white")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //white knight
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="knight"&&col[i][j]=="white")cnt++;
        if(cnt!=0)
        {
            writestr<<"knight "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="knight"&&col[i][j]=="white")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //white pawn
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="pawn"&&col[i][j]=="white")cnt++;
        if(cnt!=0)
        {
            writestr<<"pawn "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="pawn"&&col[i][j]=="white")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
        //white bishop
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                if(piece[i][j]=="bishop"&&col[i][j]=="white")cnt++;
        if(cnt!=0)
        {
            writestr<<"bishop "<<cnt<<" ";
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if(piece[i][j]=="bishop"&&col[i][j]=="white")writestr<<ChessCord::trans(i,j)<<" ";
            writestr<<endl;cnt=0;
        }
    }
    return str;
}

QString ChessCord::trans(int x,int y)
{
    QString str="  ";
    str[0]=('a'+y-1);
    str[1]=('9'-x);
    return str;
}

int ChessCord::getX(QString str)
{
    string stt=str.toStdString();
    return ('9'-stt[1]);
}

int ChessCord::getY(QString str)
{
    string stt=str.toStdString();
    return (stt[0]-'a'+1);
}
