#include "inidialog.h"
#include "ui_inidialog.h"

iniDialog::iniDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::iniDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox,&QDialogButtonBox::accepted,this,&iniDialog::validCheck);
    outputPt=myPoint(1,1);
    washPt=myPoint(1,1);
    wastePt=myPoint(1,1);
    setFixedSize(this->width(),this->height());
    updateDisplay();
}

iniDialog::~iniDialog()
{
    delete ui;
}

myPoint::myPoint(){}

myPoint::myPoint(int xx,int yy)
{
    x=xx;
    y=yy;
}

int iniDialog::rowValue()
{
    return ui->row->value();
}

int iniDialog::colValue()
{
    return ui->column->value();
}

vector<myPoint> iniDialog::inputPoint()
{
    return inputPt;
}

myPoint iniDialog::outputPoint()
{
    return outputPt;
}

myPoint iniDialog::washPoint()
{
    return washPt;
}

myPoint iniDialog::wastePoint()
{
    return wastePt;
}

bool iniDialog::isWashed()
{
    if(ui->iswashbox->isChecked())return true;
    return false;
}

void iniDialog::validCheck()
{
    if(rowValue()<=3&&colValue()<=3)
    {
        QMessageBox::warning(this,tr("Warning"),tr("The Board is Too Small.\n"));
        return;
    }
    vector<myPoint>chk;
    chk=inputPt;
    if(inputPt.size()==0)
    {
        QMessageBox::warning(this,tr("Warning"),tr("No Inputs Detected.\n"));
        return;
    }
    chk.push_back(outputPt);
    if(isWashed())chk.push_back(washPt);
    if(isWashed())chk.push_back(wastePt);
    for (int i=0;i<chk.size();i++)
    {
        for (int j=i+1;j<chk.size();j++)
        {
            if(chk[i].x==chk[j].x&&chk[i].y==chk[j].y)
            {
                QMessageBox::warning(this,tr("Warning"),tr("Repeated Ports Detected.\n"));
                return;
            }
        }
        if(!(chk[i].x==0||chk[i].y==0||chk[i].x==rowValue()+1||chk[i].y==colValue()+1))
        {
            QMessageBox::warning(this,tr("Warning"),tr("Ports are not on the Borders.\n"));
            return;
        }
        if((chk[i].x==0||chk[i].x==rowValue()+1)&&(chk[i].y==0||chk[i].y==colValue()+1))
        {
            QMessageBox::warning(this,tr("Warning"),tr("Ports are not on the Borders.\n"));
            return;
        }
        if(chk[i].x>rowValue()+1||chk[i].y>colValue()+1)
        {
            QMessageBox::warning(this,tr("Warning"),tr("Ports are not on the Borders.\n"));
            return;
        }
    }
    this->accept();
}

void iniDialog::on_inputbtn_clicked()
{
    inputPt.push_back(myPoint(ui->rowbox->value(),ui->colbox->value()));
    updateDisplay();
}

void iniDialog::on_outputbtn_clicked()
{
    outputPt=myPoint(ui->rowbox->value(),ui->colbox->value());
    updateDisplay();
}

void iniDialog::on_washbtn_clicked()
{
    washPt=myPoint(ui->rowbox->value(),ui->colbox->value());
    updateDisplay();
}

void iniDialog::on_wastebtn_clicked()
{
    wastePt=myPoint(ui->rowbox->value(),ui->colbox->value());
    updateDisplay();
}

void iniDialog::on_clearbtn_clicked()
{
    inputPt.clear();
    updateDisplay();
}

void iniDialog::updateDisplay()
{
    ui->inputbro->clear();
    ui->outputbro->clear();
    ui->washbro->clear();
    ui->wastebro->clear();
    QString str;
    for (int i=0;i<inputPt.size();i++)
    {
        str.clear();
        str.sprintf("(%d,%d)",inputPt[i].x,inputPt[i].y);
        ui->inputbro->append(str);
    }
    str.clear();
    str.sprintf("(%d,%d)",outputPt.x,outputPt.y);
    ui->outputbro->append(str);
    str.clear();
    str.sprintf("(%d,%d)",wastePt.x,wastePt.y);
    ui->wastebro->append(str);
    str.clear();
    str.sprintf("(%d,%d)",washPt.x,washPt.y);
    ui->washbro->append(str);
}


void iniDialog::updateWashPorts()
{
    washPt=myPoint(0,1);
    wastePt=myPoint(ui->row->value()+1,ui->column->value());
    updateDisplay();
}


void iniDialog::on_row_valueChanged(int arg1)
{
    updateWashPorts();
}

void iniDialog::on_column_valueChanged(int arg1)
{
    updateWashPorts();
}
