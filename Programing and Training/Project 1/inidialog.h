#ifndef INIDIALOG_H
#define INIDIALOG_H

#include <QDialog>
#include <QDebug>
#include <vector>
#include <QString>
#include <QtGui>
#include <QtWidgets>

using namespace std;

class myPoint
{
public:
    myPoint();
    myPoint(int xx,int yy);
    int x,y;
};


namespace Ui {
class iniDialog;
}

class iniDialog : public QDialog
{
    Q_OBJECT

public:
    explicit iniDialog(QWidget *parent = 0);
    ~iniDialog();
    int rowValue();
    int colValue();
    vector<myPoint> inputPoint();
    myPoint outputPoint();
    myPoint washPoint();
    myPoint wastePoint();
    bool isWashed();
    void updateDisplay();
    void updateWashPorts();

private:
    vector<myPoint> inputPt;
    myPoint outputPt,washPt,wastePt;
    Ui::iniDialog *ui;

private slots:
    void validCheck();
    void on_inputbtn_clicked();
    void on_outputbtn_clicked();
    void on_washbtn_clicked();
    void on_wastebtn_clicked();
    void on_clearbtn_clicked();
    void on_row_valueChanged(int arg1);
    void on_column_valueChanged(int arg1);
};

#endif // INIDIALOG_H
