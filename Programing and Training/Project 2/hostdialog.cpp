#include "hostdialog.h"
#include "ui_hostdialog.h"
#include <QDebug>

HostDialog::HostDialog(QString str,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HostDialog)
{
    ui->setupUi(this);
    setFixedSize(this->size());
    hostip=str;
    ui->hostip->setText(hostip);
    ui->hostip->setReadOnly(true);
    ui->cancelbtn->setDisabled(true);
    ui->startbtn->setEnabled(true);
}

HostDialog::~HostDialog()
{
    delete ui;
}

void HostDialog::on_startbtn_clicked()
{
    emit startListen();
    ui->cancelbtn->setEnabled(true);
    ui->startbtn->setDisabled(true);
}

void HostDialog::on_cancelbtn_clicked()
{
    emit stopListen();
    ui->cancelbtn->setDisabled(true);
    ui->startbtn->setEnabled(true);
}

int HostDialog::getPort()
{
    return ui->port->value();
}
