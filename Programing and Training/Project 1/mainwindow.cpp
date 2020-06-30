#include "mainwindow.h"
#include <iostream>
#include <sstream>
#include <QSound>
#include "ui_mainwindow.h"
#include "washsolver.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QSound::play(":/wav/move");
    ui->setupUi(this);
    boardSet=false;doError=false;isplayall=false;
    setFixedSize(this->width(),this->height());
    connect(ui->actionQuit,&QAction::triggered,this,&QWidget::close);
    connect(ui->actionInitialize_Board,&QAction::triggered,this,&MainWindow::iniBoard);
    connect(ui->actionFile_Input,&QAction::triggered,this,&MainWindow::openFile);
    connect(ui->actionNext_Step,&QAction::triggered,this,&MainWindow::nextStep);
    connect(ui->actionPrevious_Step,&QAction::triggered,this,&MainWindow::prevStep);
    connect(ui->actionPlay_All,&QAction::triggered,this,&MainWindow::autoPlay);
    connect(ui->actionRestart,&QAction::triggered,this,&MainWindow::restartGame);
    ui->mainToolBar->addAction(ui->actionInitialize_Board);
    ui->mainToolBar->addAction(ui->actionFile_Input);
    //ui->mainToolBar
    this->setContextMenuPolicy(Qt::NoContextMenu);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::iniBoard()
{
    iniWindow=new iniDialog(this);
    if(iniWindow->exec()==QDialog::Accepted)
    {
        row=iniWindow->rowValue();
        col=iniWindow->colValue();
        inputPorts=iniWindow->inputPoint();
        outputPorts=iniWindow->outputPoint();
        washPorts=iniWindow->washPoint();
        wastePorts=iniWindow->wastePoint();
        isWashed=iniWindow->isWashed();
        boardSet=true;
        refreshAll();
    }
    delete iniWindow;
}

void MainWindow::updateTime()
{
    ui->progress->setMaximum(timeLimit);
    ui->timedis->display(curTime);
    ui->progress->setValue(curTime);
}

void MainWindow::refreshAll()
{
    doError=false;isplayall=false;
    board[0][0].cordleft=40+37;
    board[0][0].cordup=86+37+37;
    for (int i=0;i<=row+1;i++)
    {
        for (int j=0;j<=col+1;j++)
        {
            board[i][j].blocked=false;
            if(j!=0)
            {
                board[i][j].cordleft=board[i][j-1].cordleft+Grid::len;
                board[i][j].cordup=board[i][j-1].cordup;
            }
            else if(i!=0)
            {
                board[i][j].cordleft=board[i-1][j].cordleft;
                board[i][j].cordup=board[i-1][j].cordup+Grid::len;
            }
        }
    }
    for (int i=0;i<120;i++)
    {
        queryTime[i].clear();
        droplet[i].clear();
        for (int j=0;j<20;j++)
        {
            for (int k=0;k<20;k++)
            {
                pollu[i][j][k].clear();
                board[j][k].reset();
            }
        }
    }
    ui->filebro->clear();
    ui->attrbro->clear();
    timeLimit=1;
    curTime=0;
    updateTime();
    update();
}

void MainWindow::openFile()
{
    if(boardSet==false)
    {
        QMessageBox::warning(this,tr("Warning"),tr("Board is not Created yet.\n"));
        return;
    }
    QString path=QFileDialog::getOpenFileName(this,tr("Select File"),".",tr("Text Files(*.txt)"));
    qDebug()<<path<<endl;
    if(!path.isEmpty())
    {
        QFile file(path);
        if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            QMessageBox::warning(this,tr("Warning"),tr("Cannot open file.\n"));
            return;
        }
        refreshAll();
        bool flgfile=true;
        while (!file.atEnd())
        {
            QByteArray line=file.readLine();
            if(line.isEmpty())continue;
            QString tmpstr(line);
            while (tmpstr[tmpstr.length()-1]!=';')
            {
                tmpstr=QString::fromStdString(tmpstr.toStdString().substr(0,tmpstr.length()-1));
            }
            qDebug()<<1<<' '<<tmpstr<<endl;
            ui->filebro->append(tmpstr);
            string str=tmpstr.toStdString();
            flgfile&=queryAnalysis(str);
            if(flgfile==false)break;
        }
        file.close();
        QString tmpstr="----------------------";
        ui->filebro->append(tmpstr);
        for (int i=0;i<120;i++)
        {
            for (int j=0;j<(int)queryTime[i].size();j++)
            {
                timeLimit=i;
                if(j==0)
                {
                    ui->filebro->append("Time = "+QString::number(i));
                }
                ui->filebro->append(queryTime[i][j].translateString());
            }
        }
        timeLimit+=3;curTime=0;
        updateTime();
        if(!flgfile)
        {
            refreshAll();
            return;
        }
    }
    else
    {
        QMessageBox::warning(this,tr("Warning"),tr("You did not select any file."));
    }
}

bool MainWindow::queryAnalysis(string str)
{
    stringstream ins(str);
    string opt;
    int x1,x2,t,x3,x4,x5,x6;
    char trash;
    ins>>opt;
    qDebug()<<QString::fromStdString(opt)<<endl;
    if(opt=="Input")
    {
        ins>>t;
        trash=ins.get();
        ins>>x1;
        trash=ins.get();
        ins>>x2;
        Query newQ;
        newQ.type="Input";
        newQ.x1=x1;
        newQ.y1=x2;
        bool flg=false;
        for (int i=0;i<(int)inputPorts.size();i++)
        {
            if(distPnts(inputPorts[i].x,inputPorts[i].y,x1,x2)<=1)flg=true;
        }
        if(flg==false)
        {
            QMessageBox::warning(this,tr("Warning"),tr("Invalid Input Query Detected."));
            return false;
        }
        queryTime[t].push_back(newQ);
    }
    else if(opt=="Output")
    {
        ins>>t;
        trash=ins.get();
        ins>>x1;
        trash=ins.get();
        ins>>x2;
        Query newQ;
        newQ.type="Output";
        newQ.x1=x1;
        newQ.y1=x2;
        if(distPnts(outputPorts.x,outputPorts.y,x1,x2)>1)
        {
            QMessageBox::warning(this,tr("Warning"),tr("Invalid Output Query Detected."));
            return false;
        }
        queryTime[t].push_back(newQ);
    }
    else if(opt=="Mix")
    {
        ins>>t;
        trash=ins.get();
        ins>>x1;
        trash=ins.get();
        ins>>x2;
        trash=ins.get();
        while (!ins.eof())
        {
            ins>>x3;
            trash=ins.get();
            ins>>x4;
            trash=ins.get();
            Query newQ;
            newQ.type="Mix";
            newQ.x1=x1;
            newQ.y1=x2;
            newQ.x2=x3;
            newQ.y2=x4;
            if(x1!=x3||x2!=x4)queryTime[t].push_back(newQ);
            t++;
            x1=x3;
            x2=x4;
        }
    }
    else if(opt=="Merge")
    {
        ins>>t;
        trash=ins.get();
        ins>>x1;
        trash=ins.get();
        ins>>x2;
        trash=ins.get();
        ins>>x3;
        trash=ins.get();
        ins>>x4;
        Query newQ;
        x5=(x1+x3)/2;
        x6=(x2+x4)/2;
        newQ.x1=x1;
        newQ.y1=x2;
        newQ.x2=x3;
        newQ.y2=x4;
        newQ.x3=x5;
        newQ.y3=x6;
        newQ.type="Merge";
        queryTime[t].push_back(newQ);
        newQ.type="Shrink";
        queryTime[t+1].push_back(newQ);
    }
    else if(opt=="Split")
    {
        ins>>t;
        trash=ins.get();
        ins>>x1;
        trash=ins.get();
        ins>>x2;
        trash=ins.get();
        ins>>x3;
        trash=ins.get();
        ins>>x4;
        trash=ins.get();
        ins>>x5;
        trash=ins.get();
        ins>>x6;
        Query newQ;
        newQ.x1=x1;
        newQ.y1=x2;
        newQ.x2=x3;
        newQ.y2=x4;
        newQ.x3=x5;
        newQ.y3=x6;
        newQ.type="Split";
        queryTime[t+1].push_back(newQ);
        newQ.type="Expand";
        queryTime[t].push_back(newQ);
    }
    else if(opt=="Move")
    {
        ins>>t;
        trash=ins.get();
        ins>>x1;
        trash=ins.get();
        ins>>x2;
        trash=ins.get();
        ins>>x3;
        trash=ins.get();
        ins>>x4;
        Query newQ;
        newQ.type="Move";
        newQ.x1=x1;
        newQ.y1=x2;
        newQ.x2=x3;
        newQ.y2=x4;
        queryTime[t].push_back(newQ);
    }
    else
    {
        QMessageBox::warning(this,tr("Warning"),tr("Invalid Input Detected."));
        return false;
    }
    qDebug()<<trash<<endl;
    return true;
}

void MainWindow::forwardStep(Query qq)
{
    if(qq.type=="Move"||qq.type=="Mix")
    {
        QSound::play(":/wav/move");
        for (int i=0;i<(int)droplet[curTime+1].size();i++)
        {
            Droplet tmp=droplet[curTime+1][i];
            if(tmp.isdisabled==true)continue;
            if(qq.x1==tmp.x&&qq.y1==tmp.y)
            {
                droplet[curTime+1][i].x=qq.x2;
                droplet[curTime+1][i].y=qq.y2;
                pollu[curTime+1][tmp.x][tmp.y].insert(tmp.idx);
            }
        }
    }
    if(qq.type=="Expand")
    {
        QSound::play(":/wav/expand");
        for (int i=0;i<(int)droplet[curTime+1].size();i++)
        {
            Droplet tmp=droplet[curTime+1][i];
            if(tmp.isdisabled==true)continue;
            if(qq.x1==tmp.x&&qq.y1==tmp.y)
            {
                Droplet tmp1(i,qq.x2,qq.y2);
                Droplet tmp2(i,qq.x3,qq.y3);
                droplet[curTime+1].push_back(tmp1);
                droplet[curTime+1].push_back(tmp2);
                droplet[curTime+1][i].isExpand=true;
                if(qq.x1==qq.x2)droplet[curTime+1][i].alongRow=true;
            }
        }
    }
    if(qq.type=="Shrink")
    {
        QSound::play(":/wav/merge");
        for (int i=0;i<(int)droplet[curTime+1].size();i++)
        {
            Droplet tmp=droplet[curTime+1][i];
            if(tmp.isdisabled==true)continue;
            if(qq.x1==tmp.x&&qq.y1==tmp.y)
            {
                droplet[curTime+1][i].isdisabled=true;
            }
            if(qq.x2==tmp.x&&qq.y2==tmp.y)
            {
                droplet[curTime+1][i].isdisabled=true;
            }
            if(qq.x3==tmp.x&&qq.y3==tmp.y)
            {
                droplet[curTime+1][i].isExpand=false;
                droplet[curTime+1][i].alongRow=false;
            }
        }
    }
    if(qq.type=="Merge")
    {
        Droplet tmp1(droplet[curTime+1].size(),qq.x3,qq.y3);
        droplet[curTime+1].push_back(tmp1);
        int tpp=tmp1.idx;
        for (int i=0;i<(int)droplet[curTime+1].size();i++)
        {
            Droplet tmp=droplet[curTime+1][i];
            if(tmp.isdisabled==true)continue;
            if(qq.x1==tmp.x&&qq.y1==tmp.y)
            {
                pollu[curTime+1][tmp.x][tmp.y].insert(tmp.idx);
                droplet[curTime+1][i].idx=tpp;
            }
            if(qq.x2==tmp.x&&qq.y2==tmp.y)
            {
                pollu[curTime+1][tmp.x][tmp.y].insert(tmp.idx);
                droplet[curTime+1][i].idx=tpp;
            }
            if(qq.x3==tmp.x&&qq.y3==tmp.y)
            {
                droplet[curTime+1][i].isExpand=true;
                if(qq.x1==qq.x2)droplet[curTime+1][i].alongRow=true;
            }
        }
    }
    if(qq.type=="Split")
    {
        QSound::play(":/wav/split");
        for (int i=0;i<(int)droplet[curTime+1].size();i++)
        {
            Droplet tmp=droplet[curTime+1][i];
            if(tmp.isdisabled==true)continue;
            if(qq.x1==tmp.x&&qq.y1==tmp.y)
            {
                droplet[curTime+1][i].isdisabled=true;
                pollu[curTime+1][tmp.x][tmp.y].insert(tmp.idx);
            }
            if(qq.x2==tmp.x&&qq.y2==tmp.y)
            {
                droplet[curTime+1][i].idx=i;
            }
            if(qq.x3==tmp.x&&qq.y3==tmp.y)
            {
                droplet[curTime+1][i].idx=i;
            }
        }
    }
    if(qq.type=="Input")
    {
        Droplet tmp(droplet[curTime+1].size(),qq.x1,qq.y1);
        droplet[curTime+1].push_back(tmp);
    }
    if(qq.type=="Output")
    {
        for (int i=0;i<(int)droplet[curTime+1].size();i++)
        {
            Droplet tmp=droplet[curTime+1][i];
            if(tmp.isdisabled==true)continue;
            if(qq.x1==tmp.x&&qq.y1==tmp.y)
            {
                droplet[curTime+1][i].isdisabled=true;
                pollu[curTime+1][tmp.x][tmp.y].insert(tmp.idx);
            }
        }
    }
}

bool MainWindow::dropletCheck(Droplet d1,Droplet d2)
{
    if(d1.isdisabled)return true;
    if(d2.isdisabled)return true;
    if(d1.isExpand)return true;
    if(d2.isExpand)return true;
    if(d1.idx==d2.idx)return true;
    if(abs_dmfb(d1.x-d2.x)<=1&&abs_dmfb(d1.y-d2.y)<=1)return false;
    return true;
}

void MainWindow::collisionCheck()
{
    bool flgg=true;
    for (int i=0;i<(int)droplet[curTime].size();i++)
    {
        for (int j=i+1;j<(int)droplet[curTime].size();j++)
        {
            if(!dropletCheck(droplet[curTime][i],droplet[curTime][j]))flgg=false;
        }
    }
    if(!flgg)
    {
        doError=true;
        QMessageBox::warning(this,tr("Error"),tr("Static collision rules are broken.\n"));
        return;
    }
    for (int i=0;i<(int)droplet[curTime].size();i++)
    {
        for (int j=0;j<(int)droplet[curTime-1].size();j++)
        {
            if(i==j)continue;
            if(!dropletCheck(droplet[curTime][i],droplet[curTime-1][j]))flgg=false;
        }
    }
    if(!flgg)
    {
        doError=true;
        QMessageBox::warning(this,tr("Error"),tr("Dynamic collision rules are broken.\n"));
        return;
    }
    for (int i=0;i<(int)droplet[curTime].size();i++)
    {
        if(polluCheck(pollu[curTime][droplet[curTime][i].x][droplet[curTime][i].y],i))
        {
            flgg=false;break;
        }
    }
    if(!flgg&&isWashed)
    {
        doError=true;
        QMessageBox::warning(this,tr("Error"),tr("Pollution rules are broken.\n"));
        return;
    }
}

bool MainWindow::polluCheck(set<int>x, int y)
{
    if(x.size()>=2)return true;
    if(x.size()==0)return false;
    if(x.find(droplet[curTime][y].idx)!=x.end())return false;
    if(x.find(y)!=x.end())return false;
    return true;
}

void MainWindow::nextStep()
{
    if(curTime==timeLimit||doError)return;
    //next step
    for (int i=0;i<20;i++)
    {
        for (int j=0;j<20;j++)
        {
            pollu[curTime+1][i][j]=pollu[curTime][i][j];
        }
    }
    droplet[curTime+1]=droplet[curTime];
    for (int i=0;i<(int)queryTime[curTime].size();i++)
    {
        forwardStep(queryTime[curTime][i]);
    }
    curTime++;
    updateTime();
    update();
    if(isWashed&&curTime!=timeLimit)washDisplay();
    collisionCheck();
}

void MainWindow::prevStep()
{
    if(curTime==0)return;
    doError=false;
    curTime--;
    updateTime();
    update();
}

void MainWindow::restartGame()
{
    curTime=0;
    doError=false;
    for (int i=0;i<120;i++)
    {
        droplet[i].clear();
        for (int j=0;j<20;j++)
        {
            for (int k=0;k<20;k++)
            {
                pollu[i][j][k].clear();
                board[j][k].reset();
            }
        }
    }
    updateTime();
    update();
}

void MainWindow::autoPlay()
{
    ui->playbtn->setDisabled(true);
    ui->previousbtn->setDisabled(true);
    ui->nextbtn->setDisabled(true);
    ui->restartbtn->setDisabled(true);
    dmfbTimer=startTimer(1000);
    isplayall=true;
}

void MainWindow::washDisplay()
{
    if(isplayall)
    {
        if(dmfbTimer!=-1)killTimer(dmfbTimer);
        dmfbTimer=-1;
    }

    ui->playbtn->setDisabled(true);
    ui->previousbtn->setDisabled(true);
    ui->nextbtn->setDisabled(true);
    ui->restartbtn->setDisabled(true);

    WashSolver *solv=new WashSolver(row,col,washPorts.x,washPorts.y,wastePorts.x,wastePorts.y);
    washTime=1;
    washSeq.clear();
    for (int i=0;i<(int)droplet[curTime].size();i++)
    {
        if(droplet[curTime][i].isdisabled==false)solv->setBlockCenter(droplet[curTime][i].x,droplet[curTime][i].y);
    }
    for (int i=1;i<=row;i++)
    {
        for (int j=1;j<=col;j++)
        {
            if(board[i][j].blocked)solv->setBlock(i,j);
            if(pollu[curTime][i][j].size()!=0)solv->setPollu(i,j);
        }
    }
    washSeq=solv->solve();
    delete solv;
    qDebug()<<washSeq.size();
    for (int i=0;i<washSeq.size();i++)
    {
        qDebug()<<washSeq[i].first<<' '<<washSeq[i].second;
    }
    if(washSeq[washSeq.size()-1]!=make_pair(wastePorts.x,wastePorts.y))
    {
        if(isplayall)dmfbTimer=startTimer(1000);
        else
        {
            ui->playbtn->setEnabled(true);
            ui->previousbtn->setEnabled(true);
            ui->nextbtn->setEnabled(true);
            ui->restartbtn->setEnabled(true);
        }
        return;
    }
    washtimer=startTimer(200);
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if(event->timerId()==dmfbTimer)
    {
        nextStep();
        if(curTime==timeLimit||doError==true)
        {
            if(dmfbTimer!=-1)killTimer(dmfbTimer);dmfbTimer=-1;
            ui->playbtn->setEnabled(true);
            ui->previousbtn->setEnabled(true);
            ui->nextbtn->setEnabled(true);
            ui->restartbtn->setEnabled(true);
            isplayall=false;
        }
    }
    else if(event->timerId()==washtimer)
    {
        if(washTime==washSeq.size()-1)
        {
            washx=0;washy=0;update();washSeq.clear();
            if(washtimer!=-1)killTimer(washtimer);washtimer=-1;
            if(isplayall)dmfbTimer=startTimer(1000);
            else
            {
                ui->playbtn->setEnabled(true);
                ui->previousbtn->setEnabled(true);
                ui->nextbtn->setEnabled(true);
                ui->restartbtn->setEnabled(true);
            }
            return;
        }
        washx=washSeq[washTime].first;
        washy=washSeq[washTime].second;
        pollu[curTime][washx][washy].clear();
        washTime++;
        QSound::play(":/wav/move");
        update();
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    if(boardSet==false)return;
    QPainter pter(this);
    int x,y;
    pter.setPen(QPen(Qt::black,2));
    QFont ftt=pter.font();
    ftt.setBold(true);
    ftt.setPointSize(12);
    pter.setFont(ftt);
    for (int i=1;i<=row;i++)
    {
        for (int j=1;j<=col;j++)
        {
            pter.setBrush(Qt::NoBrush);
            if(board[i][j].blocked==true)
            {
                pter.setBrush(QBrush(QColor(255,0,0,40)));
            }
            pter.drawRect(board[i][j].cordleft,board[i][j].cordup,Grid::len,Grid::len);
        }
    }
    pter.setBrush(QBrush(QColor(238,121,66,100)));
    for (int i=0;i<(int)inputPorts.size();i++)
    {
        x=inputPorts[i].x;y=inputPorts[i].y;
        pter.drawRect(board[x][y].cordleft,board[x][y].cordup,Grid::len,Grid::len);
        pter.drawText(QRect(board[x][y].cordleft,board[x][y].cordup,Grid::len,Grid::len),Qt::AlignCenter,"I");
    }
    pter.setBrush(QBrush(QColor(0,0,255,100)));
    x=outputPorts.x;y=outputPorts.y;
    pter.drawRect(board[x][y].cordleft,board[x][y].cordup,Grid::len,Grid::len);
    pter.drawText(QRect(board[x][y].cordleft,board[x][y].cordup,Grid::len,Grid::len),Qt::AlignCenter,"O");
    if(isWashed)
    {
        pter.setBrush(QBrush(QColor(135,206,255,100)));
        x=washPorts.x;y=washPorts.y;
        pter.drawRect(board[x][y].cordleft,board[x][y].cordup,Grid::len,Grid::len);
        pter.drawText(QRect(board[x][y].cordleft,board[x][y].cordup,Grid::len,Grid::len),Qt::AlignCenter,"W");
        pter.setBrush(QBrush(QColor(139,129,76,100)));
        x=wastePorts.x;y=wastePorts.y;
        pter.drawRect(board[x][y].cordleft,board[x][y].cordup,Grid::len,Grid::len);
        pter.drawText(QRect(board[x][y].cordleft,board[x][y].cordup,Grid::len,Grid::len),Qt::AlignCenter,"T");
    }
    //count pollution
    if(curTime==timeLimit)
    {
        for (int i=1;i<=row;i++)
        {
            for (int j=1;j<=col;j++)
            {
                pter.drawText(QRect(board[i][j].cordleft,board[i][j].cordup,Grid::len,Grid::len),Qt::AlignCenter,QString::number((int)pollu[curTime][i][j].size()));
            }
        }
        return;
    }
    pter.setPen(Qt::NoPen);
    //draw pollution
    for (int i=1;i<=row;i++)
    {
        for (int j=1;j<=col;j++)
        {
            if(pollu[curTime][i][j].size()!=0)
            {
                pter.setBrush(QBrush(QColor(0,0,0,40)));
                pter.drawEllipse(board[i][j].cordleft+12,board[i][j].cordup+12,12,12);
            }
        }
    }
    //draw expand
    for (int i=0;i<(int)droplet[curTime].size();i++)
    {
        Droplet tmp=droplet[curTime][i];
        if(tmp.isdisabled==true)continue;
        if(tmp.isExpand==true)
        {
            x=tmp.x;
            y=tmp.y;
            pter.setBrush(QBrush(presetColor(i)));
            if(tmp.alongRow==true)pter.drawRect(board[x][y].cordleft-17,board[x][y].cordup+6,72,26);
            else pter.drawRect(board[x][y].cordleft+6,board[x][y].cordup-17,26,72);
        }
    }
    //draw droplet
    for (int i=0;i<(int)droplet[curTime].size();i++)
    {
        Droplet tmp=droplet[curTime][i];
        if(tmp.isdisabled==true)continue;
        if(tmp.isExpand==false)
        {
            x=tmp.x;
            y=tmp.y;
            pter.setBrush(QBrush(presetColor(i)));
            pter.drawEllipse(board[x][y].cordleft+6,board[x][y].cordup+6,26,26);
        }
    }
    if(washx>0&&washy>0)
    {
        pter.setPen(QPen(Qt::black,2));
        pter.setBrush(QBrush(QColor(0,0,255,40)));
        pter.setFont(ftt);
        pter.drawEllipse(board[washx][washy].cordleft+1,board[washx][washy].cordup+1,35,35);
        pter.drawText(QRect(board[washx][washy].cordleft,board[washx][washy].cordup,Grid::len,Grid::len),Qt::AlignCenter,"W");
    }
/*
    pter.setPen(QPen(Qt::black,2));
    pter.setFont(ftt);
    for (int i=1;i<=row;i++)
    {
        for (int j=1;j<=col;j++)
        {
            pter.drawText(QRect(board[i][j].cordleft,board[i][j].cordup,Grid::len,Grid::len),Qt::AlignCenter,QString::number((int)pollu[curTime][i][j].size()));
        }
    }*/
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(!isWashed)return;
    if(event->button()!=Qt::RightButton)return;
    int x=event->x(),y=event->y();
    for (int i=1;i<=row;i++)
    {
        for (int j=1;j<=col;j++)
        {
            if(QRect(board[i][j].cordleft,board[i][j].cordup,Grid::len,Grid::len).contains(x,y))
            {
                board[i][j].blocked=!board[i][j].blocked;
            }
        }
    }update();
}

void MainWindow::on_nextbtn_clicked()
{
    nextStep();
}

void MainWindow::on_playbtn_clicked()
{
    autoPlay();
}

void MainWindow::on_previousbtn_clicked()
{
    prevStep();
}

void MainWindow::on_restartbtn_clicked()
{
    restartGame();
}
