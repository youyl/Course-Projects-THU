#ifndef PROMOTEDIALOG_H
#define PROMOTEDIALOG_H

#include <QDialog>

namespace Ui {
class PromoteDialog;
}

class PromoteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PromoteDialog(QWidget *parent = 0);
    ~PromoteDialog();
    int promotimer,curtime;
    QString answer;
    void timerEvent(QTimerEvent *evnet);

private slots:
    void on_rookbtn_clicked();

    void on_knightbtn_clicked();

    void on_bishopbtn_clicked();

    void on_queenbtn_clicked();

private:
    Ui::PromoteDialog *ui;
};

#endif // PROMOTEDIALOG_H
