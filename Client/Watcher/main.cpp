

#include <QCoreApplication>
#include <QThread>

#include <QUdpSocket>
#include <QTcpSocket>

#include <QNetworkInterface>
#include <QHostAddress>
#include <QList>

#include <controlpacket.h>
#include <win32netconfig.h>


#include <stdio.h>
#include <iostream>

#include <QDir>

#include "controlpacket.h"
#include "settings.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc,argv);


    Settings::getInstance().configureMode(CLIENT_MODE);
    control::ControllerClientSocket socket;


    Settings *sett = &Settings::getInstance();

    sett->setVideoScale(sett->getVideoScale());
    sett->setVideoFPS(sett->getVideoFPS());

    QString ffmpegPath = sett->getFFMPEGpath();

    if(ffmpegPath.isEmpty()){
        QDir dir;

        ffmpegPath = app.applicationDirPath()+"/ffmpeg/bin/ffmpeg.exe";
        sett->setFFMPEGpath(ffmpegPath);
    }
    Win32NetConfig cfg;

    if(sett->getConnected()){
        cfg.restoreNetConfig();
        sett->setDisconnected();
    }

    cfg.removeRulePort(sett->getCommPort());
    cfg.allowPort(sett->getCommPort());
    socket.startListening();


    app.exec();




        return 0;
}


