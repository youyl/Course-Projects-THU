#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QTimerEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QString>
#include "inidialog.h"
#include "grid.h"
#include "droplet.h"
#include "query.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    iniDialog *iniWindow;

    int row,col,washx,washy,washtimer;
    vector<myPoint>inputPorts;
    myPoint outputPorts,washPorts,wastePorts;
    bool isWashed,boardSet;

    Grid board[20][20];
    vector<Droplet>droplet[120];
    set<int>pollu[120][20][20];
    vector<Query>queryTime[120];
    int timeLimit,curTime,washTime,dmfbTimer;
    vector<pair<int,int> >washSeq;
    bool doError,isplayall;

    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void refreshAll();
    bool queryAnalysis(string str);
    void updateTime();
    void forwardStep(Query qq);
    void collisionCheck();
    bool dropletCheck(Droplet,Droplet);
    void washDisplay();
    bool polluCheck(set<int>x, int y);

private slots:
    void iniBoard();
    void openFile();
    void nextStep();
    void prevStep();
    void restartGame();
    void autoPlay();
    void on_nextbtn_clicked();
    void on_playbtn_clicked();
    void on_previousbtn_clicked();
    void on_restartbtn_clicked();
};

#endif // MAINWINDOW_H
