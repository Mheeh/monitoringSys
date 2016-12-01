#include "zoomvideodialog.h"
#include <VLCQtCore/Common.h>
#include <QHBoxLayout>
#include <QThread>
#include <QList>


zoomVideoDialog::zoomVideoDialog(const QString &location, VlcInstance* vlcInst, QWidget *parent):QDialog(parent)
{
    this->setAttribute(Qt::WA_DeleteOnClose);


    _selfResetTimer = new QTimer(this);
    _selfResetTimer->setInterval(120000);
    QList<QString> argsVLC = VlcCommon::args();
    //argsVLC.removeLast();
    //argsVLC.append(QString("--no-drop-late-frames"));
    argsVLC.append(QString("--clock-jitter=0"));

    if(vlcInst==0)_vlcInst = new VlcInstance(argsVLC,this);
    else _vlcInst = vlcInst;

    _media = new VlcMedia(location,_vlcInst);
    _player = new VlcMediaPlayer(_vlcInst);
    _canva = new VlcWidgetVideo(_player);

    //_canva->setSizePolicy(QSizePolicy::Ignored);
    QHBoxLayout *lay = new QHBoxLayout(this);
    _canva->setMinimumSize(300,300);
    _player->setVideoWidget(_canva);
    lay->addWidget(_canva);

    this->setWindowState(Qt::WindowMaximized);
    this->setLayout(lay);

    //not used anymore
    //connect(_selfResetTimer,SIGNAL(timeout()),this,SLOT(restartStream()));

    //_selfResetTimer->start();
}

void zoomVideoDialog::show()
{
    QDialog::show();
    _player->open(_media);
}

zoomVideoDialog::~zoomVideoDialog(){

}

zoomVideoDialog::restartStream()
{
    _player->stop();
    _player->open(_media);
}
