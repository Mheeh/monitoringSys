#include "mainwindow.h"
#include <QApplication>


#include <QGridLayout>
#include <QMediaPlayer>
#include <QVideoWidget>

#include <QPushButton>
#include <QDialog>


#include <VLCQtCore/Common.h>


#include "settings.h"
#include "maindialog.h"
#include "win32netconfig.h"

#include <QDir>
#include <QProcess>

#include "plcdialog.h"

MainWindow *w;

void restartWindow(){
    delete w;
    w = new MainWindow;
    w->connect(w,&MainWindow::restartNeeded,restartWindow);
    w->show();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationVersion(APP_VERSION);
    //PLCDialog *t = new PLCDialog;
    //t->show();
    //QDir dir;


    Settings::getInstance().configureMode(SERVER_MODE);
    Win32NetConfig netCfg;
    Settings *sett = &Settings::getInstance();

    if(!sett->getConnected()){
        QProcess p;
        QString monitorPath = a.applicationFilePath();
        monitorPath.replace("/","\\");
        qDebug() << monitorPath;
        p.execute("netsh advfirewall firewall add rule name=watcher dir=in edge=yes program=\""+monitorPath+"\"  action=allow");
        p.waitForFinished();

        p.execute("netsh advfirewall firewall add rule name=watcher dir=out program=\""+monitorPath+"\"  action=allow");
        p.waitForFinished();

        for(int i = 1; i < 20;i++){
            netCfg.allowPort(27500+i);
        }
        sett->setConnected();
    }

    netCfg.allowPort(Settings::getInstance().getCommPort());

    //VlcCommon::setPluginPath("C:/SDKs/VLC-Qt_1.0.1_win32_mingw/");
    qDebug() << a.applicationDirPath();
    VlcCommon::setPluginPath(a.applicationDirPath()+"/VLC-Qt_1.1.0_win32_mingw/");
    w = new MainWindow;
    w->connect(w,&MainWindow::restartNeeded,restartWindow);
    w->show();

    return a.exec();
}


