#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QWidget>
#include <QGridLayout>

#include <QMap>
#include <QThread>
#include <QTcpServer>
#include <QGridLayout>
#include "clientwatcher.h"
#include <QUdpSocket>
#include <QTimer>
#include "clientwatcher.h"

class mainDialog : public QWidget
{
    Q_OBJECT

public:
    explicit mainDialog(QWidget *parent = 0);
    ~mainDialog();



private:
    QMap<int,QHostAddress> _neigbourhood;
    QMap<int,clientWatcher*> _watcherList;
    int _clientsCount = 0;
    QTimer* discoveryTimer;
    QTcpServer* tcpserver;
    QUdpSocket* udpsocket;
    QGridLayout* mainLayout = 0;


public slots:

    void newConnection();
    void internetConnection(bool arg);
    void discovery(bool arg);
    void settingChanged();
    void sendBroadcast();
    //void maketunnel(int from, int to, char ON);
signals:
    void blockInternetReq();
    void unblockInternetReq();
    void newClient();
    void restartNeeded();


};

#endif // MAINDIALOG_H
