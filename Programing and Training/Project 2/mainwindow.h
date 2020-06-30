#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QMouseEvent>
#include "networkmodule.h"
#include "chessgame.h"
#include "hostdialog.h"
#include "clientdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void paintEvent(QPaintEvent *evnet);

    void mousePressEvent(QMouseEvent *event);

    void timerEvent(QTimerEvent *event);

private slots:
    void startChess();

    void youWin();

    void youLose();

    void controlMessageSend(int,int,int,int,QString);

    void controlMessageRecv(QString);

    void on_clientbtn_clicked();

    void on_serverbtn_clicked();

    void on_savebtn_clicked();

    void on_loadbtn_clicked();

    void on_concedebtn_clicked();

    void on_actionLoad_Game_triggered();

    void on_actionSave_Game_triggered();

    void startlisten();

    void stoplisten();

    void startconnect();

private:
    Ui::MainWindow *ui;
    ChessGame *game;
    bool startGame,isrunning,setWin,setLose;
    QLabel *pos[10][10];
    NetworkModule *socket;
    int localtimer,curtime,ishost;
    HostDialog *hostdia;
    ClientDialog *clientdia;
};

#endif // MAINWINDOW_H
