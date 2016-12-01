#include <QCoreApplication>
#include <QDebug>
#include <QProcess>
#include <QDir>
#include "runner.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString watcherPath = a.applicationDirPath()+"/Watcher.exe";
    watcherPath = QDir::toNativeSeparators(watcherPath);
    qDebug() << watcherPath;
    //watcherPath = "C:\\Users\\Mechastrike\\Documents\\build-Watcher-Desktop_Qt_5_6_0_MinGW_32bit-Release\\release\\Watcher.exe";
    //watcherPath.replace("/","\\");
    //QString watcherPath = "C:/Users/Mechastrike/Documents/build-Watcher-Desktop_Qt_5_6_0_MinGW_32bit-Release/release/Watcher.exe";
    //qDebug() << "startowanie procesu";

    //p = new QProcess;
    //p->start(watcherPath);

    Runner r(watcherPath);
    r.restartWatcher(0);
    qDebug() << "wystartowano";

    //qDebug()<< a.applicationFilePath();
    return a.exec();
}


