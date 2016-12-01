#include <controlpacket.h>

#include "settings.h"
#include "anetconfig.h"
#include "win32netconfig.h"
//#include<vlc/vlc.h>
#include <QProcess>

#include <QDataStream>
#include <QDir>


control::ControllerClientSocket::ControllerClientSocket(){

    Settings &sett = Settings::getInstance();

    sett.setCommPort(sett.getCommPort());
    sett.setVideoPort(sett.getVideoPort());
    sett.setVideoScale(sett.getVideoScale());

    portNo = sett.getCommPort();
    timer = new QTimer;
    timer->setInterval(100);
    timer->setParent(this);
    timer->connect(timer,SIGNAL(timeout()),this,SLOT(hearbeat()));

    this->udpSocket = new QUdpSocket(this);
    this->tcpSocket = new QTcpSocket(this);

    //vlcInst = libvlc_new(0,NULL);


    //here should be change for crossplatform
    netCfg = new Win32NetConfig();


    netCfg->removeRulePort(Settings::getInstance().getCommPort());
    netCfg->removeRulePort(Settings::getInstance().getVideoPort());
    netCfg->allowPort(Settings::getInstance().getCommPort());
    netCfg->allowPort(Settings::getInstance().getVideoPort());
    netCfg->removeRuleIP(sett.getPLCAddr().toStdString());
    netCfg->allowIP(sett.getPLCAddr().toStdString());
    //end
    std::string fps = Settings::getInstance().getVideoFPS().toStdString();

    const char *params[]={"--intf=dummy,--screen-fps=5,--mux-caching=5000"};


    std::string scale = Settings::getInstance().getVideoScale().toStdString();
    std::string streamport=QString::number(Settings::getInstance().getVideoPort()).toStdString();
    //"#transcode{vcodec=theo,vb=1984,fps=5,scale=0.5,acodec=none}:rtp{sdp=rtsp://:"+streamport+"/monitor}";
    //:rtp{proto=udp}:rtp{port=8554}
    //std::string transcode = "#transcode{vcodec=theo,vb=4098,fps=3,scale="+sett.getVideoScale().toStdString()+",acodec=none}:rtp{sdp=rtsp://:"+streamport+"/monitor}";

    //last "working" - not working anymore
    //std::string transcode = "#transcode{vcodec=theo,vb=128000,fps="+fps+",scale="+scale+",acodec=none}:rtp{sdp=rtsp://:"+streamport+"/monitor}";

    //TCP
    //std::string transcode = "#transcode{vcodec=theo,vb=4098,fps="+fps+",scale="+scale+",acodec=none}:rtp{sdp=rtsp://:"+streamport+"/monitor,proto=tcp}";

    //testing 2 works somewhat
    //std::string transcode = "#transcode{vcodec=H264,vb=12800,scale=1,fps=6,acodec=none}:rtp{mux=ts,dst=127.0.0.1,port=4444,sdp=rtsp://192.168.1.6:8080/test.sdp}";

    //testing 3 works too
    //std::string transcode = "#transcode{vcodec=theo,vb=12800,scale=1,fps=3,acodec=none}:rtp{caching=100,sdp=rtsp://:8080/test.sdp}";

    //vb ? what value is optimal?!
    //vb 160000 okey?
    //vb 80 less so
    //Working for SURE use it.
    std::string transcode =  "#transcode{vcodec=H264,vb=400,scale="+scale+",fps="+fps+",acodec=none,venc=x264{preset=ultrafast,vbv-maxrate=1200,vbv-bufsize=1200,nf}}:rtp{mux=ts,sdp=rtsp://:"+streamport+"/monitor}";
    //std::string transcode =  "#transcode{vcodec=theo,vb=700,scale="+scale+",fps="+fps+",acodec=none}:rtp{sdp=rtsp://:"+streamport+"/monitor}";

    //std::string transcode =  "#transcode{vcodec=mp2v,vb=80,scale=1,fps=25,acodec=none}:rtp{mux=ts,sdp=rtsp://:"+streamport+"/monitor}";

    //HTTP
    //std::string transcode = "#standard{access=http,mux=ogg,dst=:"+streamport+"/monitor}";
    //std::string transcode = "#transcode{vcodec=theo,vb=2000,fps="+fps+",scale="+scale+",acodec=none}:standard{access=http,mux=ogg,dst=:"+streamport+"/monitor}";
    //std::string transcode = "#transcode{vcodec=theo,vb=5,fps="+fps+",scale="+scale+",acodec=none}:http{mux=ogg,dst=:"+streamport+"/monitor}";
    //std::string transcode = "#transcode{vcodec=theo,vb=40980,fps=5,scale=0.7,acodec=none}:rtp{sdp=rtsp://:"+streamport+"/monitor}";

    //libvlc_f

    /*
    libvlc_vlm_add_broadcast(vlcInst, "monitor",
            "screen://",
            transcode.c_str(),
            3, params, // <= 5 == sizeof(params) == count of parameters
            1, 0);

    */
    //libvlc_vlm_play_media(vlcInst,"monitor");

    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(handleTCPpacket()));
    connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(cleanTCPconnection()));
    connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(cleanTCPconnection()));
    connect(tcpSocket,SIGNAL(connected()),this,SLOT(connectedToHost()));

}
//:sout=#transcode{vcodec=theo,vb=2000,scale=0.5,acodec=none}:rtp{sdp=rtsp://:8554/monitor} :sout-keep
void control::ControllerClientSocket::setPort(quint16 port){
    this->portNo=port;
}

quint16 control::ControllerClientSocket::getPort(){
    return this->portNo;
}

void  control::ControllerClientSocket::handleUDPpacket(){
    qDebug() <<"Przyszedl pakiet udp";
    QByteArray data;
    data.resize(udpSocket->pendingDatagramSize());
    QHostAddress serverAddress;
    udpSocket->readDatagram(data.data(),data.size(),&serverAddress);
    //do things only if not already connected
    if(tcpSocket->state()!= QTcpSocket::ConnectedState){

        QDataStream stream(data);
        control::controlpacket packet; // = (control::controlpacket*)data.data();

        stream >> packet;
        //check packet funcion
        if(!_blockedServers.contains(serverAddress.toIPv4Address())&&(packet.func == control::BROADCAST)){
            qDebug() << "Rozpoznano broadcast";
            disconnect(udpSocket,SIGNAL(readyRead()),this,SLOT(handleUDPpacket()));
            hostAddr = serverAddress;
            initilazeSettings();
            qDebug() << "Łączenie do " << hostAddr;
            tcpSocket->connectToHost(serverAddress,Settings::getInstance().getCommPort());

            timer->start();
        }
    }
}

void control::ControllerClientSocket::handleTCPpacket()
{

    //QDataStream stream(&readbuffer,QIODevice::ReadOnly);
    QDataStream stream(tcpSocket);
    stream.setVersion(QDataStream::Qt_5_6);
    qDebug() << "otrzymano tcp";
    controlpacket packet;
    proxyArgs proxy;
    routeArgs route;
    blockArgs block;
    videoArgs video;

    //readbuffer.append(tcpSocket->readAll());


    //qDebug() << "Wielkośc przed peek: " << readbuffer.size();
    char blocksize = stream.device()->peek(1).data()[0];

    while(tcpSocket->bytesAvailable() > 0 && blocksize <= tcpSocket->bytesAvailable()){


        //packet = (control::controlpacket *)readbuffer.data();
        qDebug() << "funkcja po tcp SIZE:" << (quint8)blocksize;
        //qDebug() << "buffersize:" << readbuffer.size();
        //qDebug() << packet->func;
        if(blocksize>0){

            stream >> packet;
            qDebug() << "Funkcja:" << (quint8)packet.func << "Wielkość pakietu:" << (quint8)packet.totalSize;
            switch(packet.func){
            case control::BLOCK_GIVEN:
            case control::UNBLOCK_GIVEN:
                stream >> block;
                control::ControllerClientSocket::executeCommands(&packet,&block);
                break;
            case control::ROUTE_ADD:
            case control::ROUTE_DEL:
                stream >> route;
                control::ControllerClientSocket::executeCommands(&packet,&route);
                break;
            case control::PROXY_ADD:
            case control::PROXY_DEL:
                stream >> proxy;
                control::ControllerClientSocket::executeCommands(&packet,&proxy);
                break;
            case control::VIDEO_ON:
                stream >> video;
                control::ControllerClientSocket::executeCommands(&packet,&video);
                break;
            default:
                control::ControllerClientSocket::executeCommands(&packet);
                break;
            }

            blocksize = stream.device()->peek(1).data()[0];
            //control::ControllerClientSocket::executeCommands(packet);
            //readbuffer.remove(0,blocksize);
        }else{
            break;
        }
    }

}

void control::ControllerClientSocket::cleanTCPconnection()
{
    qDebug() <<"Sprzatanie po tcp";
    stopBroadcast();
    timer->stop();
    timeRemaining=3000;
    netCfg->restoreNetConfig();
    Settings::getInstance().setDisconnected();
    tcpSocket->disconnectFromHost();
    udpSocket->bind(QHostAddress::AnyIPv4,Settings::getInstance().getCommPort());
    //udpSocket->readAll();
    udpSocket->connect(udpSocket,SIGNAL(readyRead()),this,SLOT(handleUDPpacket()));
    disconnect(tcpSocket,SIGNAL(readyRead()),this,SLOT(handleTCPpacket()));

}

void control::ControllerClientSocket::connectedToHost()
{
    qDebug() << "Połaczono z hostem";

    if(Settings::getInstance().getConnected() == 0){
        netCfg->saveNetConfig();
        QProcess p;
        p.execute("netsh advfirewall set allprofiles state on");
    }
    Settings::getInstance().setConnected();
    Settings::getInstance().setHost(hostAddr.toString());
    qDebug() << "Rozłączanie udp";
    QObject::disconnect(udpSocket,SIGNAL(readyRead()),this,SLOT(handleUDPpacket()));
    udpSocket->close();
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(handleTCPpacket()));
}

void control::ControllerClientSocket::hearbeat()
{
    timeRemaining-=100;
    if(timeRemaining<0){
        if(tcpSocket->state()==QTcpSocket::ConnectedState) tcpSocket->close();
        else{
            timer->stop();
            timeRemaining = 5000;
        }
    }
}

void control::ControllerClientSocket::restartBroadcast(int t)
{
    startBroadcast();
}

int control::ControllerClientSocket::startListening(){

    udpSocket->bind(QHostAddress::AnyIPv4,portNo,QUdpSocket::ShareAddress);
    udpSocket->connect(udpSocket,SIGNAL(readyRead()),this,SLOT(handleUDPpacket()));
    qDebug() <<"powiązano udp:" << portNo << " " << udpSocket->state();
    qDebug() << "Pending :" << udpSocket->pendingDatagramSize();
    return 0;
}

void control::ControllerClientSocket::startBroadcast()
{
    //QDir dir;
    //QString ffmpeg = dir.absolutePath()+"/ffmpeg/bin/ffmpeg.exe";
    QString ffmpeg = Settings::getInstance().getFFMPEGpath();
    qDebug() << "ffmpegBIN: " << ffmpeg;
    QString scale = QString::number(scalePerc/100);
    if(scalePerc%100>0){
        scale.append(".");
        scale.append(scalePerc%100);
    }
    QString kbit = QString::number(kbitrate);

    ffmpeg = QDir::toNativeSeparators(ffmpeg);
    QString fullCommand = ffmpeg+" -f gdigrab -i desktop -r 5 -vf scale=iw*"+scale+":ih*"+scale+" -b:v "+kbit+"k -an -vcodec libx264 -tune zerolatency -preset ultrafast -f mpegts udp://"+hostAddr.toString()+":"+QString::number(27500+stationNo);

    qDebug() << fullCommand;
    //broadcastProc.move
    //broadcastProc.execute(fullCommand);
    broadcastProc.start(fullCommand);
    qDebug() << "Po execute";
    //libvlc_vlm_play_media(vlcInst,"monitor");
}

void control::ControllerClientSocket::stopBroadcast()
{
    broadcastProc.kill();
    //libvlc_vlm_stop_media(vlcInst,"monitor");
}

quint16 control::ControllerServerSocket::getPortNo() const
{
    return portNo;
}

void control::ControllerServerSocket::setPortNo(const quint16 &value)
{
    portNo = value;
}

void control::ControllerClientSocket::executeCommands(control::controlpacket *packet, void *args)
{
    blockArgs *block;
    routeArgs *route;
    proxyArgs *proxy;
    videoArgs *video;

    block = (blockArgs*)args;
    route = (routeArgs*)args;
    proxy = (proxyArgs*)args;
    video = (videoArgs*)args;
    switch(packet->func){
    case control::BROADCAST:
        qDebug() << "otrzymano Broadcast";
        timeRemaining = 3000;

        break;
    case control::BLOCK_ALL:
        qDebug() << "otrzymano Block";
        //netCfg->removeRuleIP(QHostAddress(block->ip).toString().toStdString());
        //qDebug() << "Po remove rule";
        netCfg->allowIP(hostAddr.toString().toStdString());
        qDebug() << "Po allow IP";
        netCfg->setDefaultPolicy(BLOCK_CONNECTION);
        qDebug() << "Po policy";
        netCfg->blockPort(80);
        qDebug() << "Po 80";
        netCfg->blockPort(443);
        qDebug() << "Po 443";
        break;
    case control::BLOCK_DEFAULT:
        qDebug() << "otrzymano Default";
        netCfg->setDefaultPolicy(ALLOW_CONNECTION);
        netCfg->removeRuleIP(hostAddr.toString().toStdString());
        netCfg->removeRulePort(80);
        netCfg->removeRulePort(443);
        break;
    case control::BLOCK_GIVEN:
        netCfg->removeRuleIP(QHostAddress(block->ip).toString().toStdString());
        netCfg->blockIP(QHostAddress(block->ip).toString().toStdString());
        //if(block->par)
        break;
    case control::UNBLOCK_GIVEN:
        netCfg->removeRuleIP(QHostAddress(block->ip).toString().toStdString());
        netCfg->allowIP(QHostAddress(block->ip).toString().toStdString());
        break;
    case control::VIDEO_ON:
        qDebug() << "otrzymano VideoOn";
        this->stationNo = video->no;
        this->kbitrate = video->kbitrate;
        this->scalePerc = video->scalePerc;
        this->startBroadcast();
        connect(&broadcastProc,SIGNAL(finished(int)),this,SLOT(restartBroadcast(int)));
        break;
    case control::VIDEO_OFF:
        disconnect(&broadcastProc,SIGNAL(finished(int)),this,SLOT(restartBroadcast(int)));
        this->stopBroadcast();
        qDebug() << "otrzymano VideoOff";
    break;
    case control::PROXY_ADD:
        qDebug() << "otrzymano ProxyAdd";
        netCfg->forwardPort(QHostAddress(proxy->listAddr).toString().toStdString(),proxy->low_port,proxy->high_port,QHostAddress(proxy->connAddr).toString().toStdString(),proxy->sw);
        break;
    case control::PROXY_DEL:
        qDebug() << "otrzymano ProxyDel";
        netCfg->removeForward(QHostAddress(proxy->listAddr).toString().toStdString(),proxy->low_port,proxy->high_port,QHostAddress(proxy->connAddr).toString().toStdString());
        break;

    case control::BANNED:
        _blockedServers.append(hostAddr.toIPv4Address());
        tcpSocket->disconnectFromHost();
        break;
    }
}

void control::ControllerClientSocket::initilazeSettings()
{
    Settings &sett = Settings::getInstance();
    sett.setPLCAddr(sett.getPLCAddr());
    sett.setPLCInterfaceAddr(sett.getPLCInterfaceAddr());


    qDebug() << "allow plc IP";
    netCfg->removeRuleIP(sett.getPLCAddr().toStdString());
    netCfg->allowIP(sett.getPLCAddr().toStdString());
    qDebug() << "port proxy to PLC from another stations";
    netCfg->forwardPort(tcpSocket->localAddress().toString().toStdString(),27501,27520,sett.getPLCAddr().toStdString());
    qDebug() << "port proxy to control PLC";

    netCfg->forwardPort(tcpSocket->localAddress().toString().toStdString(),5562,5562,sett.getPLCAddr().toStdString());
    QProcess p;
    //QString command = "netsh interface portproxy add v4tov4 listenport=5000-6000";
    netCfg->allowIP(tcpSocket->peerAddress().toString().toStdString());
    //command +=
    qDebug() << sett.getPLCAddr();
    p.execute("netsh interface portproxy add v4tov4 listenport=5000-6000 connectaddress="+sett.getPLCAddr()+" listenaddress="+tcpSocket->localAddress().toString());
    p.waitForFinished();
    //netCfg->forwardPort(tcpSocket->peerAddress().toString().toStdString(),5000,6000,sett.getPLCAddr().toStdString());
}


QDataStream &operator<<(QDataStream &out, const control::controlpacket &packet)
{
    out << (quint8)packet.totalSize << packet.srcIP << (quint8)packet.func;
    return out;
}

QDataStream &operator<<(QDataStream &out, const control::blockArgs &args)
{
    out << (quint8)args.par << args.low_port << args.high_port << args.ip;
    return out;
}

QDataStream &operator<<(QDataStream &out, const control::proxyArgs &args)
{
    out << args.low_port << args.high_port << args.listAddr << args.connAddr << (quint8)args.sw;
    return out;
}

QDataStream &operator<<(QDataStream &out, const control::routeArgs &args)
{
    out << args.dstAddr << args.mask << args.gateway;
    return out;
}

QDataStream &operator>>(QDataStream &in,  control::controlpacket &packet)
{
    quint8 total;
    quint8 func;
    in >> total >> packet.srcIP >> func;
    packet.totalSize = total;
    packet.func = func;

    qDebug()<<"total:" << total <<" func: " << func;
    //in >> packet.totalSize;
    return in;
}

QDataStream &operator>>(QDataStream &in,  control::blockArgs &args)
{
    quint8 par;
    in >> par >> args.low_port >> args.high_port >> args.ip;
    args.par = par;
    return in;
}

QDataStream &operator>>(QDataStream &in,  control::proxyArgs &args)
{
    quint8 sw;
    in >> args.low_port >> args.high_port >> args.listAddr >> args.connAddr >> sw;
    args.sw = sw;
    return in;
}

QDataStream &operator>>(QDataStream &in,  control::routeArgs &args)
{
    in >> args.dstAddr >> args.mask >> args.gateway;
    return in;
}

QDataStream &operator<<(QDataStream &out, const control::videoArgs &args)
{
    out << args.no << args.kbitrate << args.scalePerc;
    return out;
}

QDataStream &operator>>(QDataStream &in, control::videoArgs &args)
{
    in >> args.no >> args.kbitrate >> args.scalePerc;
    return in;
}


