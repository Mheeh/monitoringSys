#include "runner.h"
#include <QDebug>

Runner::Runner(QObject *parent) : QObject(parent)
{

}


Runner::Runner(QString path, QObject *parent):QObject(parent){
    watcherPath = path;
    connect(&p,SIGNAL(finished(int)),this,SLOT(restartWatcher(int)));
}

void Runner::restartWatcher(int i){
    qDebug() << "restarting";
    p.start(watcherPath);
}
