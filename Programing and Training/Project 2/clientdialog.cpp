#include "clientdialog.h"
#include "ui_clientdialog.h"
#include <QMessageBox>
#include "networkmodule.h"

ClientDialog::ClientDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClientDialog)
{
    ui->setupUi(this);
    setFixedSize(this->size());
}

ClientDialog::~ClientDialog()
{
    delete ui;
}

int ClientDialog::getPort()
{
    return ui->port->value();
}

QString ClientDialog::getIP()
{
    return ui->hostip->text();
}

void ClientDialog::reconstruct()
{
    ui->connectbtn->setText("Connect");
    ui->connectbtn->setEnabled(true);
}

void ClientDialog::on_connectbtn_clicked()
{
    NetworkModule *tt=new NetworkModule();
    if(!(tt->isIPv4(ui->hostip->text())))
    {
        QMessageBox::warning(this,"Error","Invalid IP Detected");
        return;
    }
    ui->connectbtn->setText("Connecting...");
    ui->connectbtn->setDisabled(true);
    emit startConnect();
}
