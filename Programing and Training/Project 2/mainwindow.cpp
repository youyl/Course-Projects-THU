#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <sstream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    startGame=false;
    game=new ChessGame(this);
    setFixedSize(this->size());
    for (int i=1;i<=8;i++)
    {
        for (int j=1;j<=8;j++)
        {
            pos[i][j]=new QLabel(this);
            pos[i][j]->setGeometry(game->getCord(i,j));
        }
    }
    localtimer=-1;
    socket=new NetworkModule();
    hostdia=new HostDialog(socket->getHostIP(),this);
    qDebug()<<"socket ini finished";
    clientdia=new ClientDialog(this);
    connect(hostdia,SIGNAL(stopListen()),this,SLOT(stoplisten()));
    connect(hostdia,SIGNAL(startListen()),this,SLOT(startlisten()));
    connect(clientdia,SIGNAL(startConnect()),this,SLOT(startconnect()));
    qDebug()<<"dialog ini finished";
    connect(game,SIGNAL(loseThis()),this,SLOT(youLose()));
    connect(game,SIGNAL(winThis()),this,SLOT(youWin()));
    connect(game,SIGNAL(movePiece(int,int,int,int,QString)),this,SLOT(controlMessageSend(int,int,int,int,QString)));
    qDebug()<<"game ini finished";
    connect(socket,SIGNAL(getMessage(QString)),this,SLOT(controlMessageRecv(QString)));
    connect(socket,SIGNAL(connectionSuccess()),this,SLOT(startChess()));
    qDebug()<<"connect ini finished";
    update();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *evnet)
{
    QPainter pter(this);
    pter.setPen(Qt::NoPen);
    for (int i=1;i<=8;i++)
    {
        for (int j=1;j<=8;j++)
        {
            if(i%2==j%2)pter.setBrush(QBrush(QColor(240,218,181)));
            else pter.setBrush(QBrush(QColor(185,131,99)));
            pter.drawRect(game->getCord(i,j));

            pos[i][j]->clear();
            if(game->getColor(i,j)!="idle")
            {
                QPixmap mp(":/piece/"+(game->getColor(i,j))+(game->getPiece(i,j)));
                mp=mp.scaled(60,60,Qt::KeepAspectRatio,Qt::SmoothTransformation);
                pos[i][j]->setPixmap(mp);
            }

            int cursta=game->getStatus(i,j);
            if(cursta==ChessStatus::idle)continue;
            if(cursta==ChessStatus::picked)pter.setBrush(QBrush(QColor(255,0,0,70)));
            if(cursta==ChessStatus::canmove)pter.setBrush(QBrush(QColor(0,0,255,70)));
            if(cursta==ChessStatus::cancapture)pter.setBrush(QBrush(QColor(0,0,255,70)));
            if(cursta==ChessStatus::castling)pter.setBrush(QBrush(QColor(0,255,0,70)));
            pter.drawRect(game->getCord(i,j));
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(!startGame)return;
    if(!isrunning)return;
    if(event->button()==Qt::LeftButton)
    {
        for (int i=1;i<=8;i++)
        {
            for (int j=1;j<=8;j++)
            {
                if((game->getCord(i,j)).contains(event->x(),event->y()))
                {
                    if(localtimer!=-1){killTimer(localtimer);localtimer=-1;}
                    game->clickAction(i,j);
                    if(isrunning)localtimer=startTimer(1000);
                    update();
                    return;
                }
            }
        }
    }
}

void MainWindow::on_clientbtn_clicked()
{
    ishost=false;
//    socket->connectToHost("127.0.0.1",12925);
    clientdia->reconstruct();
    clientdia->show();
}

void MainWindow::on_serverbtn_clicked()
{
    ishost=true;
//    socket->startHost(12925);
    hostdia->show();
}

void MainWindow::startlisten()
{
    socket->startHost(hostdia->getPort());
}

void MainWindow::stoplisten()
{
    socket->stopHost();
}

void MainWindow::startconnect()
{
    socket->connectToHost(clientdia->getIP(),clientdia->getPort());
}

void MainWindow::youWin()
{
    if(setWin)return;
    setWin=true;
    controlMessageSend(-1,-1,-1,-1,"win");
    QMessageBox::information(this,"Congragulation","You win this one.");
    startChess();
    setWin=false;
}

void MainWindow::youLose()
{
    if(setLose)return;
    setLose=true;
    controlMessageSend(0,0,0,0,"lose");
    QMessageBox::information(this,"Sorry","You lose this one.");
    startChess();
    setLose=false;
}

void MainWindow::startChess()
{
    game->iniGame();
    hostdia->close();
    clientdia->close();
    ui->textBrowser->append("Game started!");
    if(ishost)ui->textBrowser->append("As a White!");
    else ui->textBrowser->append("As a Black!");
    startGame=true;
    if(ishost)
    {
        game->setColor("white");
        curtime=60;
        isrunning=true;
        localtimer=startTimer(1000);
        ui->timedisplay->display(curtime);
    }
    else
    {
        game->setColor("black");
        curtime=0;
        isrunning=false;
        ui->timedisplay->display(curtime);
    }
    ui->clientbtn->setDisabled(true);
    ui->serverbtn->setDisabled(true);
    setLose=false;
    setWin=false;
    update();
}

void MainWindow::controlMessageSend(int x1,int y1,int x2,int y2,QString str)
{
    if(localtimer!=-1){killTimer(localtimer);localtimer=-1;}
    isrunning=false;
    str=QString::number(y2)+" "+str;
    str=QString::number(x2)+" "+str;
    str=QString::number(y1)+" "+str;
    str=QString::number(x1)+" "+str;
    socket->sendMessage(str);
    str="Send "+str;
    ui->textBrowser->append(str);
}

void MainWindow::controlMessageRecv(QString str)
{
    ui->textBrowser->append("Receive "+str);
    int x1,x2,y1,y2;
    string strp;
    string stt=str.toStdString();
    stringstream strstr(stt);
    strstr>>x1>>y1>>x2>>y2>>strp;
    if(strp=="concede"||strp=="lose")youWin();
    else if(strp=="win")youLose();
    else if(strp=="castling")
    {
        if(y2==3)
        {
            game->receMess(x1,5,x2,3,"pawn");
            game->receMess(x1,1,x2,4,"pawn");
        }
        else
        {
            game->receMess(x1,5,x2,7,"pawn");
            game->receMess(x1,8,x2,6,"pawn");
        }
        curtime=60;
        ui->timedisplay->display(curtime);
        localtimer=startTimer(1000);
        isrunning=true;
        update();
    }
    else
    {
        game->receMess(x1,y1,x2,y2,QString::fromStdString(strp));
        curtime=60;
        ui->timedisplay->display(curtime);
        localtimer=startTimer(1000);
        isrunning=true;
        update();
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if(event->timerId()!=localtimer)return;
    curtime--;
    if(curtime==-1)
    {
        if(localtimer!=-1){killTimer(localtimer);localtimer=-1;}
        youLose();
        return;
    }
    ui->timedisplay->display(curtime);
}

void MainWindow::on_savebtn_clicked()
{
    QString path=QFileDialog::getSaveFileName(this, tr("Save File"), ".", tr("Text Files(*.txt)"));
    if(!path.isEmpty())
    {
        QFile file(path);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, tr("Write File"), tr("Cannot open file:\n%1").arg(path));
            return;
        }
        QTextStream out(&file);
        out<<game->saveGame(isrunning);
        file.close();
    }
    else
    {
        QMessageBox::warning(this, tr("Path"), tr("You did not select any file."));
    }
}

void MainWindow::on_loadbtn_clicked()
{
    QString path=QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("Text Files(*.txt)"));
    if(!path.isEmpty())
    {
        QFile *file=new QFile(path);
        if(!(file->open(QIODevice::ReadOnly | QIODevice::Text)))
        {
            QMessageBox::warning(this, tr("Read File"), tr("Cannot open file:\n%1").arg(path));
            return;
        }
        QTextStream in(file);
        if(localtimer!=-1){killTimer(localtimer);localtimer=-1;}
        isrunning=false;
        QString str=game->loadGame(in.readAll());
        file->close();
        //-------------------------------------------------------------------------------------------------------------
        ui->textBrowser->append("Game started!");
        if(ishost)ui->textBrowser->append("As a White!");
        else ui->textBrowser->append("As a Black!");
        if((str=="white"&&ishost==true)||(str=="black"&&ishost==false))
        {
            curtime=60;
            isrunning=true;
            localtimer=startTimer(1000);
            ui->timedisplay->display(curtime);
        }
        else
        {
            curtime=0;
            isrunning=false;
            ui->timedisplay->display(curtime);
        }
        setLose=false;
        setWin=false;
        update();
        //-------------------------------------------------------------------------------------------------------------
    }
    else
    {
        QMessageBox::warning(this, tr("Path"), tr("You did not select any file."));
    }
    update();
}

void MainWindow::on_concedebtn_clicked()
{
    game->concedeAction();
}

void MainWindow::on_actionLoad_Game_triggered()
{
    ui->loadbtn->click();
}

void MainWindow::on_actionSave_Game_triggered()
{
    ui->savebtn->click();
}
