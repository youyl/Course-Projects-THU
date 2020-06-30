#ifndef HOSTDIALOG_H
#define HOSTDIALOG_H

#include <QDialog>

namespace Ui {
class HostDialog;
}

class HostDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HostDialog(QString str,QWidget *parent = 0);
    ~HostDialog();
    int getPort();

private:
    Ui::HostDialog *ui;
    QString hostip;

signals:
    void startListen();

    void stopListen();
private slots:
    void on_startbtn_clicked();
    void on_cancelbtn_clicked();
};

#endif // HOSTDIALOG_H
