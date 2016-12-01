#include "clientwatcher.h"
#include "ui_clientwatcher.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>

#include <VLCQtCore/Error.h>

#include <QGroupBox>
#include <QHostAddress>

#include "vlcwidgetvideoext.h"
#include "zoomvideodialog.h"
#include "settings.h"
#include "controlpacket.h"
#include "plcdialog.h"

clientWatcher::clientWatcher(int stationNo, QMap<int, QHostAddress> *neighboursMap, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::clientWatcher)
{

    this->setMinimumWidth(350);
    QSizePolicy sizeXPolicy = QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    this->setSizePolicy(sizeXPolicy);
    playDelay = new QTimer();
    playDelay->setSingleShot(true);
    connect(playDelay,SIGNAL(timeout()),this,SLOT(delayedPlay()));
    ui->setupUi(this);



    QList<QString> argsVLC = VlcCommon::args();
    //argsVLC.removeLast();
    //argsVLC.append(QString("--no-drop-late-frames"));
    argsVLC.append(QString("--clock-jitter=0"));
    //argsVLC.append(QString("--netsync-timeout=60000"));
    //argsVLC.append(QString("--rtsp-tcp"));
    //argsVLC.append(QString("--rtp-timeout=60000"));
    argsVLC.append(QString("--ipv4-timeout=60000"));

    qDebug() << "argument VLC: " << argsVLC;
    vlcInst = new VlcInstance(argsVLC,this);


    _neighboursMap = neighboursMap;
    number = stationNo;
    timer = new QTimer;
    timer->setInterval(1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(heartBeat()));
    //timer->start();

    QSizePolicy sizePolicy = QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(mainLayout,Qt::AlignTop);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    workstationNo = new QLabel(this);
    workstationNo->setText(QString::fromStdString("Stanowisko #")+QString::number(stationNo));
    workstationNo->setSizePolicy(sizePolicy);
    mainLayout->addWidget(workstationNo,0,Qt::AlignTop);


    player = new VlcMediaPlayer(vlcInst);
    VlcWidgetVideoExt *canva = new VlcWidgetVideoExt;
    canva->setMinimumSize(400,350);
    canva->setMaximumSize(500,450);
    player->setVideoWidget(canva);


    mainLayout->addWidget(canva,0,Qt::AlignTop);


    _selfStreamResetTimer = new QTimer(this);
    _selfStreamResetTimer->setInterval(80000);
    //Not needed fixed streaming
    //connect(_selfStreamResetTimer,SIGNAL(timeout()),this,SLOT(restartStream()));

    //testy

//    QTimer *test = new QTimer;
//    test->setInterval(60000);
//    test->start();
//     streamLocation = "udp://@:5000";
//     media = new VlcMedia(streamLocation,vlcInst);
//    player->open(media);

//    connect(test,SIGNAL(timeout()),this,SLOT(restartStream()));
    //
    //connect(player,SIGNAL(error()),player,SLOT(togglePause()));
    //QTimer *testing = new QTimer();
    //testing->setInterval(2000);
    //connect(testing,SIGNAL(timeout()),canva,SLOT(timedRequest()));

    //

    //end

    QHBoxLayout *connectionStatusLay = new QHBoxLayout;


    connectionStatus = new QLabel("Rozłaczony");
    connectionStatus->setSizePolicy(sizePolicy);
    if(ipaddress!=0){
        ipaddress->setText("adres: Brak"+ _addressIP.toString());
    }
    else{
        ipaddress = new QLabel("adres: "+_neighboursMap->value(number).toString());
    }
    ipaddress->setSizePolicy(sizePolicy);

    connectionStatusLay->addWidget(connectionStatus);
    connectionStatusLay->addWidget(ipaddress);
    mainLayout->addLayout(connectionStatusLay);


    QGroupBox *control = new QGroupBox("Kontrola");
    QHBoxLayout *layoutControl = new QHBoxLayout;
    broadcastCheck = new QCheckBox("Podgląd");
    broadcastCheck->setSizePolicy(sizePolicy);
    layoutControl->addWidget(broadcastCheck);


    directControlCheck = new QPushButton("PLC");
    directControlCheck->setSizePolicy(sizePolicy);
    layoutControl->addWidget(directControlCheck);
    control->setLayout(layoutControl);
    connect(directControlCheck,SIGNAL(pressed()),this,SLOT(createPLCdialog()));

    mainLayout->addWidget(control);

    QGroupBox *tunnelsBox = new QGroupBox("Proxy");
    tunnelsCheckboxesLay = new QGridLayout;

    //tunnelsCheckboxesLay->addWidget(new QCheckBox("#1"),0,0);
    //tunnelsCheckboxesLay->addWidget(new QCheckBox("#2"),0,1);


    for(int i = 1; i <= _neighboursMap->size();i++){
        if(i==number)continue;
        QSizePolicy sizePolicyFirst = QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        QCheckBox *checkT = new QCheckBox(tr("#")+QString::number(i));
        checkT->setSizePolicy(sizePolicyFirst);
        tunnelsCheckboxesLay->addWidget(checkT,(i-1)/4,(i-1)%4);
        connect(checkT,SIGNAL(stateChanged(int)),this,SLOT(tunnelCheckboxChanged(int)));
    }
    //

    tunnelsBox->setLayout(tunnelsCheckboxesLay);
    mainLayout->addWidget(tunnelsBox);
    this->setLayout(mainLayout);

    //player->open(media);
    //canva->setDisabled(true);
    connect(canva,SIGNAL(doubleclicked()),this,SLOT(zoomVideo()));
}

clientWatcher::~clientWatcher()
{
    delete ui;
    delete player;
    delete vlcInst;
    if(tcpsocket!=0 && tcpsocket->state()==QTcpSocket::ConnectedState){
        tcpsocket->close();
        delete tcpsocket;
        tcpsocket = 0;
    }

}

void clientWatcher::bindClient(QTcpSocket *qtcp)
{
    if(tcpsocket!=0){
        tcpsocket->close();
        tcpsocket->deleteLater();
        tcpsocket = qtcp;
    }
    else{
        tcpsocket = qtcp;
    }

    connect(tcpsocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(changeConnectionStatus(QAbstractSocket::SocketState)));
    _addressIP = tcpsocket->peerAddress();
    ipaddress->setText("adres: "+_addressIP.toString());
    connectionStatus->setText(tr("Połączono"));
    //streamLocation = "rtsp://"+addressIP.toString()+":"+QString::number(Settings::getInstance().getVideoPort())+"/monitor";
    streamLocation ="udp://@:"+QString::number(27500+number);
    broadcastCheck->setChecked(false);
    directControlCheck->setChecked(false);
    connect(broadcastCheck,SIGNAL(clicked(bool)),this,SLOT(playStream(bool)));
    timer->start();

}

void clientWatcher::unbindClient()
{
    player->stop();
    media->deleteLater();
    media = 0;
    if(tcpsocket!=0){
        tcpsocket->abort();
        tcpsocket->deleteLater();
        tcpsocket = 0;
    }


    broadcastCheck->setCheckState(Qt::Unchecked);
    //ipaddress->setText(_neighboursMap->value());
    connectionStatus->setText(tr("Rozłączony"));

    for(QObject *t : tunnelsCheckboxesLay->children()){
        QCheckBox *box = qobject_cast<QCheckBox *>(t);
        box->setCheckState(Qt::Checked);

    }
    timer->stop();

}

void clientWatcher::zoomVideo()
{
    player->stop();
    zoomVideoDialog *zoom = new zoomVideoDialog(streamLocation,0,this);
    connect(zoom,SIGNAL(rejected()),this,SLOT(zoomClosed()));
    zoom->setWindowTitle("Stanowisko #"+QString::number(number));
    zoom->show();
    if(tcpsocket!=0 && tcpsocket->state()==QTcpSocket::ConnectedState){
        stopStream();
        QTimer::singleShot(200,this,SLOT(startStream()));
    }
    //connect(,,this,SLOT())
}

void clientWatcher::zoomClosed(){
    if(media!=0)player->open(media);
    if(tcpsocket!=0 && tcpsocket->state()==QTcpSocket::ConnectedState){
        //qDebug() <<"Wyłączono Zoom";
        stopStream();
        QTimer::singleShot(200,this,SLOT(startStream()));
    }
}

void clientWatcher::createPLCdialog()
{
    qDebug() << "Kliknieto PLC";
    qDebug() << _neighboursMap->value(number);
    PLCDialog *t = new PLCDialog(this->number,_neighboursMap->value(number),this);
    t->show();

}

void clientWatcher::blockInternet()
{
    if(tcpsocket!=0 && tcpsocket->state()==QTcpSocket::ConnectedState){
        control::controlpacket packet;

        QByteArray buffer;
        QDataStream stream(&buffer,QIODevice::ReadWrite);
        stream.setVersion(QDataStream::Qt_5_6);


        packet.func = control::BLOCK_ALL;
        packet.srcIP = tcpsocket->localAddress().toIPv4Address();

        stream << packet;
        buffer.data()[0] = (quint8) buffer.size();
        tcpsocket->write(buffer);

    }

}

void clientWatcher::unblockInternet()
{
    if(tcpsocket!=0 && tcpsocket->state()==QTcpSocket::ConnectedState){
        QByteArray buffer;
        QDataStream stream(&buffer,QIODevice::ReadWrite);
        stream.setVersion(QDataStream::Qt_5_6);
        control::controlpacket packet;
        packet.func = control::BLOCK_DEFAULT;
        packet.srcIP = tcpsocket->localAddress().toIPv4Address();

        stream << packet;
        buffer.data()[0] = (quint8) buffer.size();
        tcpsocket->write(buffer);

    }
}

void clientWatcher::tunnelTo(int neigbour)
{
    if(tcpsocket!=0 && tcpsocket->state()==QTcpSocket::ConnectedState){
        control::controlpacket packet;
        control::proxyArgs args;
        QByteArray buffer;
        QDataStream stream(&buffer,QIODevice::ReadWrite);
        stream.setVersion(QDataStream::Qt_5_6);
        //z wewnątrz out
        packet.func = control::PROXY_ADD;
        packet.srcIP = tcpsocket->localAddress().toIPv4Address();

        args.connAddr = _neighboursMap->value(neigbour).toIPv4Address();
        args.low_port = 27500+neigbour;
        args.high_port= 27500+number;
        QString ip = Settings::getInstance().getPLCInterfaceAddr();
        args.listAddr = QHostAddress(ip).toIPv4Address();
        args.sw =1;

        //packet.totalSize = sizeof(packet)+sizeof(args);
        qDebug() << "Pierwsza:" << buffer.size() <<"b";

        stream << packet;
        stream << args;

        //stream.device()->seek(0);
        //stream << quint8(buffer.size());
        buffer.data()[0] = (quint8)buffer.size();

        /*
        qDebug() << "Zawartość bufora:";
        for(int i =0; i< buffer.size();i++){
            qDebug() << (quint8)buffer.data()[i] ;
        }

        qDebug() << "Rozmiar wyslanego: " << buffer.size();
        qDebug() << (quint8)packet.func;
        //data.append((char *)&packet,sizeof(packet));
        //data.append((char *)&args,sizeof(args));
        */
        tcpsocket->write(buffer);


        QByteArray buffer2;
        QDataStream stream2(&buffer2,QIODevice::ReadWrite);
        stream2.setVersion(QDataStream::Qt_5_6);

        control::blockArgs args2;
        packet.func = control::UNBLOCK_GIVEN;
        args2.ip = args.connAddr;

        stream2 << packet;
        stream2 << args2;

        stream2.device()->seek(0);
        stream2 << quint8(buffer2.size());

        //qDebug() << "Druga wiad:" << buffer2.size() <<"b";
        //data.append((char *)&packet,sizeof(packet));
        //data.append((char *)&args2,sizeof(args2));
        //packet.totalSize = data.size();
        tcpsocket->write(buffer2);
    }
}

void clientWatcher::removeTunnelTo(int neigbour)
{
    if(tcpsocket!=0 && tcpsocket->state()==QTcpSocket::ConnectedState){
        control::controlpacket packet;
        control::proxyArgs args;
        QByteArray buffer;
        QDataStream stream(&buffer,QIODevice::ReadWrite);
        stream.setVersion(QDataStream::Qt_5_6);

        //z wewnątrz out
        packet.func = control::PROXY_DEL;
        packet.srcIP = QHostAddress("192.168.3.1").toIPv4Address();


        args.connAddr = _neighboursMap->value(neigbour).toIPv4Address();
        //tcpsocket->peerAddress().toIPv4Address();
        args.low_port = 27500+neigbour;
        args.high_port= 27500+number;
        args.listAddr = QHostAddress(Settings::getInstance().getPLCInterfaceAddr()).toIPv4Address();
        args.sw =1;

        stream << packet;
        stream << args;

        buffer.data()[0] = (quint8)buffer.size();


        tcpsocket->write(buffer);




        QByteArray buffer2;
        QDataStream stream2(&buffer2,QIODevice::ReadWrite);
        stream2.setVersion(QDataStream::Qt_5_6);


        control::blockArgs args2;
        packet.func = control::BLOCK_GIVEN;
        args2.ip = args.connAddr;

        stream2 << packet;
        stream2 << args2;

        stream2.device()->seek(0);
        stream2 << quint8(buffer2.size());

        tcpsocket->write(buffer2);
    }
}

void clientWatcher::neigbourhoodChanged()
{
    changeStationNo(number);
}

void clientWatcher::newneighbour()
{
    //qDebug() << "New neigbour in " << number;
    int i = _neighboursMap->size();
    qDebug() << "New neigbour in " << number << " new: " << i;
    int checkCount = tunnelsCheckboxesLay->count();

    qDebug() << "In " << number << " checkBoxCount" << checkCount;
    //i++;
    QSizePolicy sizePolicyFirst = QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    QCheckBox *checkbox = new QCheckBox(tr("#")+QString::number(i));
    checkbox->setSizePolicy(sizePolicyFirst);
    tunnelsCheckboxesLay->addWidget(checkbox,(checkCount)%4,(checkCount)/4);

    connect(checkbox,SIGNAL(stateChanged(int)),this,SLOT(tunnelCheckboxChanged(int)));
}

void clientWatcher::stationAddressChanged(int stationNo)
{
    changeStationNo(stationNo);
}

void clientWatcher::changeConnectionStatus(QAbstractSocket::SocketState state)
{
    qDebug() << "zmiana stanu połączenia";
    qDebug() << state;
    if(tcpsocket!=0 && tcpsocket->state()== QTcpSocket::ConnectedState){
    connectionStatus->setText("Połączono");
    }else{
        broadcastCheck->setChecked(false);
        directControlCheck->setChecked(false);
        disconnect(broadcastCheck,SIGNAL(clicked(bool)),this,SLOT(playStream(bool)));
        connectionStatus->setText("Rozłączono");
        this->unbindClient();
    }
}

void clientWatcher::changeStationNo(int no)
{
    number = no;
    workstationNo->setText("Stanowisko #"+QString::number(no));
    foreach (QWidget * w, tunnelsCheckboxesLay->findChildren<QWidget*>()) delete w;
    QSizePolicy sizePolicyFirst = QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    for(int i =0; i < _neighboursMap->size() ;i++){
        if(i==no) continue;
        removeTunnelTo(i);
        QCheckBox *checkbox = new QCheckBox(QString::number(i));
        checkbox->setSizePolicy(sizePolicyFirst);
        tunnelsCheckboxesLay->addWidget(checkbox,i%4,i/4);
        connect(checkbox,SIGNAL(stateChanged(int)),this,SLOT(tunnelCheckboxChanged(int)));
    }
}

void clientWatcher::tunnelCheckboxChanged(int state)
{

    //tunnelTo(1);
    qDebug() << "Changed state of vlan";
    int neigbourId;
    //if(tunnelsCheckboxesLay->children().contains(obj)){
        QCheckBox* checkbox = qobject_cast<QCheckBox*>(sender());
        neigbourId = checkbox->text().remove("#").toInt();
        qDebug() << neigbourId;
        if(!_neighboursMap->value(neigbourId).isNull()){
            if(checkbox->isChecked()){
                qDebug() << "tunnelTo" << neigbourId;
                tunnelTo(neigbourId);
            }else{
                removeTunnelTo(neigbourId);
                qDebug() << "Remove tunnelTo" << neigbourId;
            }
        }
    //}
}

void clientWatcher::playStream(bool state)
{
    if(state){
        startStream();
        playDelay->start(100);

    }else{
        stopStream();
        player->stop();
        _selfStreamResetTimer->stop();

    }
}

void clientWatcher::startStream()
{
    control::controlpacket packet;
    QByteArray buffer;
    QDataStream stream(&buffer,QIODevice::ReadWrite);
    stream.setVersion(QDataStream::Qt_5_6);
    //z wewnątrz out
    packet.func = control::VIDEO_ON;
    packet.srcIP = tcpsocket->localAddress().toIPv4Address();


    stream << packet;

    control::videoArgs video;
    video.no = number;
    video.scalePerc = Settings::getInstance().getVideoScale();
    video.kbitrate = Settings::getInstance().getVideoBitRate();

    stream << video;

    buffer.data()[0] = (quint8)buffer.size();

    tcpsocket->write(buffer);
}

void clientWatcher::stopStream()
{
    control::controlpacket packet;
    QByteArray buffer;
    QDataStream stream(&buffer,QIODevice::ReadWrite);
    stream.setVersion(QDataStream::Qt_5_6);
    //z wewnątrz out
    qDebug() <<"Wyslanie stop stream";
    packet.func = control::VIDEO_OFF;
    packet.srcIP = tcpsocket->localAddress().toIPv4Address();


    stream << packet;

    buffer.data()[0] = (quint8)buffer.size();

    tcpsocket->write(buffer);

}

void clientWatcher::heartBeat()
{
    if(tcpsocket!=0 && tcpsocket->state()==QTcpSocket::ConnectedState){
        qDebug() << "Wysyłanie heartbeat do ";
        qDebug() << tcpsocket->peerAddress();
        control::controlpacket packet;

        QByteArray buffer;
        QDataStream stream(&buffer,QIODevice::ReadWrite);
        stream.setVersion(QDataStream::Qt_5_6);

        packet.func = control::BROADCAST;
        packet.srcIP = tcpsocket->localAddress().toIPv4Address();

        stream << packet;

        buffer.data()[0] = (quint8)buffer.size();

        tcpsocket->write(buffer);

    }
}

void clientWatcher::connectionError(QAbstractSocket::SocketError err)
{
    qDebug() <<"Blad polaczenia";
    qDebug() << err;
    unbindClient();
}

void clientWatcher::delayedPlay()
{
    qDebug() << "StreamLOC" << streamLocation;
    if(media != 0 ){
        if(player->state()==Vlc::State::Playing)player->stop();
        delete media;
    }
    media = new VlcMedia(streamLocation,vlcInst);
    player->open(media);
    _selfStreamResetTimer->start();
}

void clientWatcher::restartStream()
{
    //qDebug() << tr("Nastąpił error VLC");
    //qDebug() << VlcError::errmsg();
    player->stop();
    player->open(media);
}
