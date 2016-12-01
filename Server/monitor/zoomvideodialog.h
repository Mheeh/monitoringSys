#ifndef ZOOMVIDEODIALOG_H
#define ZOOMVIDEODIALOG_H

#include <QDialog>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtWidgets/WidgetVideo.h>
#include <VLCQtCore/MediaPlayer.h>
#include <QTimer>

class zoomVideoDialog : public QDialog
{
    Q_OBJECT
public:
    explicit zoomVideoDialog(const QString &location,VlcInstance *vlcInst = 0, QWidget *parent = 0);
    void show();
    ~zoomVideoDialog();
private:
    VlcInstance * _vlcInst;
    VlcWidgetVideo *_canva;
    VlcMediaPlayer *_player;
    VlcMedia *_media;

    QTimer *_selfResetTimer;
public slots:
    restartStream();

};

#endif // ZOOMVIDEODIALOG_H
