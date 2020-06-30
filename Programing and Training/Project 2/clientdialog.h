#ifndef CLIENTDIALOG_H
#define CLIENTDIALOG_H

#include <QDialog>

namespace Ui {
class ClientDialog;
}

class ClientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientDialog(QWidget *parent = 0);
    ~ClientDialog();
    int getPort();
    void reconstruct();
    QString getIP();

private slots:
    void on_connectbtn_clicked();

private:
    Ui::ClientDialog *ui;

signals:
    void startConnect();
};

#endif // CLIENTDIALOG_H
