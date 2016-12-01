#include "maindialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include "clientwatcher.h"
#include "settings.h"
#include "controlpacket.h"


mainDialog::mainDialog(QWidget *parent) :
    QWidget(parent)
{

    mainLayout = new QGridLayout;
    _clientsCount = 0;
    Settings *settings;
    settings = &Settings::getInstance();
    settings->loadClients();
    //clientWatcher *test = new clientWatcher(1,&_neigbourhood);
    //clientWatcher *test2 = new clientWatcher(2,&_neigbourhood);
    //clientWatcher *test3 = new clientWatcher(3,&_neigbourhood);
    //QLabel *label = new QLabel("test");

    settingChanged();
    //mainLayout->addWidget(label,2,0);

    tcpserver = new QTcpServer(this);

    connect(tcpserver,SIGNAL(newConnection()),this,SLOT(newConnection()));
    tcpserver->listen(QHostAddress::AnyIPv4,settings->getCommPort());

    udpsocket = new QUdpSocket(this);
    udpsocket->bind(settings->getCommPort());

    discoveryTimer = new QTimer;
    discoveryTimer->setInterval(5000);
    connect(discoveryTimer,SIGNAL(timeout()),this,SLOT(sendBroadcast()));


    if(_clientsCount == 0){
        clientWatcher *watcher = new clientWatcher(0,&_neigbourhood);
        _watcherList[0] = watcher;
        mainLayout->addWidget(watcher);
    }
    this->setLayout(mainLayout);


}

mainDialog::~mainDialog()
{

}

void mainDialog::newConnection()
{
    //qDebug() << "Nowe połaczenie";
    Settings *sett = &Settings::getInstance();
    QMap<QString,int> *clients = &sett->getClients();
    if(tcpserver->hasPendingConnections()){
        QTcpSocket * socket = tcpserver->nextPendingConnection();
        QHostAddress addr = socket->peerAddress();
        qDebug() << "Nowe połaczenie";
        qDebug() << addr;
        int id = clients->value(addr.toString(),-1);
        if(id==0){

            QByteArray packetData;
            QDataStream stream(&packetData,QIODevice::ReadWrite);
            stream.setVersion(QDataStream::Qt_5_6);
            control::controlpacket packet;
            packet.srcIP = socket->localAddress().toIPv4Address();
            packet.func = control::BANNED;

            stream << packet;
            packetData.data()[0]=(quint8) packetData.size();
            qDebug() << "Zbanowany";
            //socket->close();
            socket->write(packetData);
            socket->deleteLater();
        }
        else if(id == -1){

            qDebug() << "Nowy";
            clients->insert(addr.toString(),++_clientsCount);
            _neigbourhood.insert(_clientsCount,addr);
            clientWatcher *watcher = new clientWatcher(_clientsCount,&_neigbourhood,this);
            _watcherList[_clientsCount] = watcher;
            mainLayout->addWidget(watcher,(_clientsCount-1)/4,(_clientsCount-1)%4);
            connect(this,SIGNAL(blockInternetReq()),watcher,SLOT(blockInternet()));
            connect(this,SIGNAL(unblockInternetReq()),watcher,SLOT(unblockInternet()));

            watcher->bindClient(socket);

            qDebug()<< "Emited new client";
            emit newClient();
            connect(this,SIGNAL(newClient()),watcher,SLOT(newneighbour()));

            sett->saveClients();

            if(_clientsCount == 1){
                _watcherList[0]->setVisible(false);
                _watcherList[0]->setEnabled(false);
            }

            mainLayout->update();
            this->parentWidget()->parentWidget()->update();
            this->parentWidget()->parentWidget()->repaint();
        }
        else{
            clientWatcher *watcher = _watcherList.value(id);
            watcher->bindClient(socket);

        }

    }
}

void mainDialog::internetConnection(bool arg)
{
    if(arg){
        emit unblockInternetReq();
    }else{
        emit blockInternetReq();
    }
}

void mainDialog::discovery(bool arg)
{
    qDebug() << "zmiana timer";
    if(arg){
        discoveryTimer->start();
    }else{
        discoveryTimer->stop();
    }
}

void mainDialog::settingChanged()
{
    qDebug() << "Settings changed";
    _clientsCount = 0;
    Settings *settings;
    settings = &Settings::getInstance();
    settings->saveClients();

    _neigbourhood.clear();

    for(int i = 1; i<=_watcherList.size();i++){
        _watcherList[i]->hide();
        _watcherList[i]->deleteLater();
        _watcherList.remove(i);
    }

    QMap<QString,int> clients = settings->getClients();
    QMap<QString,int>::iterator it = clients.begin();
    _clientsCount = 0;
    for(int i =0; i< clients.size();i++,it++){
        if(it.key()==0)continue;
        _neigbourhood[it.value()] = QHostAddress(it.key());
        if(_clientsCount<it.value())_clientsCount = it.value();
    }


    for(int i = 1;i<=_clientsCount;i++){
        clientWatcher *t = _watcherList.value(i,0);
        if(t == 0){
            //qDebug() << "Pusty watcher";
            clientWatcher *watcher = new clientWatcher(i,&_neigbourhood,this);
            _watcherList[i] = watcher;
            mainLayout->addWidget(watcher,(i-1)/4,(i-1)%4);
            connect(this,SIGNAL(blockInternetReq()),watcher,SLOT(blockInternet()));
            connect(this,SIGNAL(unblockInternetReq()),watcher,SLOT(unblockInternet()));
            connect(this,SIGNAL(newClient()),watcher,SLOT(newneighbour()));
        }else{
            t->unbindClient();
        }
    }
    this->repaint();
    emit restartNeeded();
}

void mainDialog::sendBroadcast()
{
    qDebug() << "Broadcast";
    Settings *sett = &Settings::getInstance();
    //for(QHostAddress addr : QNetworkInterface::allAddresses()){

        QByteArray datagram;
        QDataStream stream(&datagram,QIODevice::ReadWrite);

        control::controlpacket t;
        t.func = control::BROADCAST;
        //t.srcIP = addr.toIPv4Address();

        stream << t;
        datagram.data()[0] = (quint8)datagram.size();
        //t.totalSize = sizeof(t);
        udpsocket->writeDatagram(datagram,QHostAddress::Broadcast,sett->getCommPort());
        //udpsocket->writeDatagram((char*)&t,t.totalSize,QHostAddress::Broadcast,sett->getCommPort());

        if(_neigbourhood.size()>0){
            QMap<int,QHostAddress>::iterator it = _neigbourhood.begin();
            for(int i = 0; i < _neigbourhood.size(); i++, it++){
                udpsocket->writeDatagram(datagram,it.value(),sett->getCommPort());
            }
        }


    //}

}
