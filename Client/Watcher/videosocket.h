#ifndef VIDEOSOCKET_H
#define VIDEOSOCKET_H

#include <QObject>
#include <QUdpSocket>
#include <QApplication>
#include <QDesktopWidget>
#include <QThread>


class VideoSocket : public QObject
{
    Q_OBJECT
public:
    explicit VideoSocket(QObject *parent = 0);

private:
    QUdpSocket* socket;

    void captureScreen();
signals:

public slots:
};

#endif // VIDEOSOCKET_H
