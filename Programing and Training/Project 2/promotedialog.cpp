#include "promotedialog.h"
#include "ui_promotedialog.h"

PromoteDialog::PromoteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PromoteDialog)
{
    ui->setupUi(this);
    setFixedSize(this->size());
    curtime=10;
    ui->timedisplay->display(curtime);
    promotimer=startTimer(1000);
}

PromoteDialog::~PromoteDialog()
{
    delete ui;
}

void PromoteDialog::timerEvent(QTimerEvent *event)
{
    if(event->timerId()!=promotimer)return;
    curtime--;
    if(curtime==-1)
    {
        answer="concede";
        this->close();
    }
    ui->timedisplay->display(curtime);
}

void PromoteDialog::on_rookbtn_clicked()
{
    answer="rook";
    this->close();
}

void PromoteDialog::on_knightbtn_clicked()
{
    answer="knight";
    this->close();
}

void PromoteDialog::on_bishopbtn_clicked()
{
    answer="bishop";
    this->close();
}

void PromoteDialog::on_queenbtn_clicked()
{
    answer="queen";
    this->close();
}
