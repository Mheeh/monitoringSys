#ifndef CONTROLPACKET_H
#define CONTROLPACKET_H

#include <string>
#include <QObject>
#include <QUdpSocket>
#include <QTcpSocket>
#include "anetconfig.h"
//#include <vlc/vlc.h>
#include <QTimer>

#include <QDataStream>

#include <QList>
#include <QProcess>




namespace control{

    const char BROADCAST = 00;
    const char WELCOME = 01;
    const char BLOCK_ALL = 10;
    const char BLOCK_DEFAULT = 11;
    const char BLOCK_GIVEN = 20;
    const char UNBLOCK_GIVEN = 21;
    const char PAR_DIRECTION_OUT = 0x8;
    const char PAR_DIRECTION_IN = 0x4;
    const char PAR_PROCOL_UDP = 0x2;
    const char PAR_PROTOCOL_TCP = 0x1;
    const char ROUTE_ADD = 30;
    const char ROUTE_DEL = 31;
    const char VIDEO_ON = 40;
    const char VIDEO_OFF = 41;
    const char PROXY_ADD = 50;
    const char PROXY_DEL = 51;

    const char BANNED = 99;

    struct controlpacket{
        char totalSize;
        quint32 srcIP;
        char func;
    };

    struct blockArgs{
        char par;
        quint16 low_port;
        quint16 high_port;
        quint32 ip;
    };

    struct routeArgs{
        quint32 dstAddr;
        quint32 mask;
        quint32 gateway;
    };

    struct proxyArgs{
      quint16 low_port;
      quint16 high_port;
      quint32 listAddr;
      quint32 connAddr;
      char sw;
    };

    struct videoArgs{
      quint8 no;
      quint8 scalePerc;
      quint16 kbitrate;
    };


    class ControllerClientSocket : public QObject{
        Q_OBJECT
    public:

        ControllerClientSocket();

        quint16 getPort();
        void setPort(quint16 port);

        int startListening();
        int sendUDPpacket();

        void startBroadcast();
        void stopBroadcast();
        void initilazeSettings();
    private:
        quint16 portNo;
        quint8 stationNo;

        quint8 scalePerc;
        quint16 kbitrate;

        QByteArray readbuffer;
        QUdpSocket *udpSocket;
        QTcpSocket *tcpSocket;
        ANetConfig *netCfg;
        //libvlc_instance_t *vlcInst;

        QHostAddress hostAddr;
        int timeRemaining;
        QTimer *timer;
        void executeCommands(controlpacket *packet, void *args = 0);

        QList<quint32> _blockedServers;
        QProcess broadcastProc;

    private slots:
        void handleUDPpacket();
        void handleTCPpacket();
        void cleanTCPconnection();
        void connectedToHost();
        void hearbeat();
        void restartBroadcast(int t);

    };
    //payloadOdVideo

    class ControllerServerSocket : public QObject{
      public:
        quint16 portNo;
        CotrollerClientSocket();

        quint16 getPortNo() const;
        void setPortNo(const quint16 &value);
    };
}

QDataStream & operator<<(QDataStream &out,const control::controlpacket &packet);
QDataStream & operator<<(QDataStream &out,const control::blockArgs &args);
QDataStream & operator<<(QDataStream &out,const control::proxyArgs &args);
QDataStream & operator<<(QDataStream &out,const control::routeArgs &args);
QDataStream & operator<<(QDataStream &out,const control::videoArgs &args);

QDataStream & operator>>(QDataStream &in, control::controlpacket &packet);
QDataStream & operator>>(QDataStream &in, control::blockArgs &args);
QDataStream & operator>>(QDataStream &in, control::proxyArgs &args);
QDataStream & operator>>(QDataStream &in, control::routeArgs &args);
QDataStream & operator>>(QDataStream &in, control::videoArgs &args);



#endif // CONTROLPACKET_H
