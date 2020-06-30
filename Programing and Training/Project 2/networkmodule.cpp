#include "networkmodule.h"
#include <QMessageBox>
#include <QHostInfo>
#include <QDataStream>
#include <string>
#include <QDebug>
#include <QtEndian>

using namespace std;

NetworkModule::NetworkModule(QObject *parent) : QObject(parent)
{
    this->server=new QTcpServer;
    this->socket=new QTcpSocket;
    next_block_size=0;
    connect(this->server,SIGNAL(newConnection()),this,SLOT(nativeConnectionSuccess()));
    connect(this->socket,SIGNAL(connected()),this,SLOT(nativeConnectionSuccess()));
    connect(this->socket,SIGNAL(readyRead()),this,SLOT(nativeReceive()));
    QList<QHostAddress>ipAddressesList=(QHostInfo::fromName(QHostInfo::localHostName())).addresses();
    for (int i = 0;i<ipAddressesList.size();i++)
    {
        if(ipAddressesList.at(i)!=QHostAddress::LocalHost&&ipAddressesList.at(i).toIPv4Address())
        {
            hostIP=ipAddressesList.at(i).toString();
            break;
        }
    }
    if(hostIP.isEmpty())hostIP=QHostAddress(QHostAddress::LocalHost).toString();
}

void NetworkModule::startHost(int _port)
{
    isHost=true;
    this->server->listen(QHostAddress::Any,_port);
}

void NetworkModule::stopHost()
{
    if(this->server->isListening())this->server->close();
}

QString NetworkModule::getHostIP()
{
    return hostIP;
}

void NetworkModule::connectToHost(QString _str,int _port)
{
    isHost=false;
    this->socket->connectToHost(QHostAddress(_str),_port);
}

void NetworkModule::sendMessage(QString _str)
{
    QByteArray block;
    QDataStream sendStream(&block,QIODevice::ReadWrite);
    sendStream<<quint16(0)<<_str;
    sendStream.device()->seek(0);
    sendStream<<(quint16)(block.size()-sizeof(quint16));
    this->socket->write(block);
//    this->socket->write(_str.toUtf8());
}

void NetworkModule::nativeReceive()
{
    QDataStream clientReadStream(this->socket);
    while(true)
    {
        if(!next_block_size)
        {
            if(this->socket->bytesAvailable()<sizeof(quint16))
            {
                break;
            }
            clientReadStream>>next_block_size;
        }
        if (this->socket->bytesAvailable()<next_block_size)
        {
            break;
        }
        QString str;
        clientReadStream >> str;
        emit getMessage(str);
        next_block_size = 0;
    }
//    currentContent=this->socket->readAll();
//    emit getMessage(currentContent);
}

bool NetworkModule::checkNum(QString _str)
{
    string str=_str.toStdString();
    int res=0;
    for (int i=0;i<(int)(str.length());i++)
    {
        if(str[i]>'9'||str[i]<'0')return true;
        res=res*10+str[i]-'0';
    }
    if(res>=0&&res<=255)return false;
    return true;
}

bool NetworkModule::isIPv4(QString str)
{
    QStringList comp=str.split(".");
    if(comp.size()!=4)return false;
    for (int i=0;i<comp.size();i++)
    {
        QString tmp=comp.at(i);
        if(checkNum(tmp))return false;
    }
    return true;
}

void NetworkModule::nativeConnectionSuccess()
{
    if(isHost)
    {
        if(this->socket!=nullptr)
        {
            this->socket->disconnect(this);
            delete this->socket;
        }
        this->socket=this->server->nextPendingConnection();
        connect(this->socket,SIGNAL(readyRead()),this,SLOT(nativeReceive()));
    }
    emit connectionSuccess();
}
