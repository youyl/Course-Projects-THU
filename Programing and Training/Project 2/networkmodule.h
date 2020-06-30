#ifndef NETWORKMODULE_H
#define NETWORKMODULE_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QWidget>
#include <QNetworkInterface>
#include <QString>

class NetworkModule : public QObject
{
    Q_OBJECT
public:
    explicit NetworkModule(QObject *parent = 0);
    void startHost(int);
    void stopHost();
    QString getHostIP();
    void connectToHost(QString,int);
    bool isIPv4(QString);
    bool checkNum(QString);

private:
    bool isHost;
    QTcpSocket *socket;
    QTcpServer *server;
    quint16 next_block_size;
    QString hostIP;

signals:
    void getMessage(QString);
    void connectionSuccess();

public slots:
    void sendMessage(QString);

private slots:
    void nativeReceive();
    void nativeConnectionSuccess();
};

#endif // NETWORKMODULE_H
