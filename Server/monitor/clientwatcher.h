#ifndef CLIENTWATCHER_H
#define CLIENTWATCHER_H

#include <QWidget>

#include <QTcpSocket>

#include <VLCQtCore/MediaPlayer.h>
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QHostAddress>
#include <QDialog>
#include <QMap>
#include <QTimer>

#include <QThread>


namespace Ui {
class clientWatcher;
}

class clientWatcher : public QWidget
{
    Q_OBJECT

public:
    explicit clientWatcher(int stationNo,QMap<int,QHostAddress> *neighboursMap,QWidget *parent = 0);
    ~clientWatcher();

    void bindClient(QTcpSocket *qtcp);
    void unbindClient();
private:
    Ui::clientWatcher *ui;
    QHostAddress _addressIP;
    int number;
    QTimer *timer;
    QTimer *playDelay;
    QLabel *workstationNo = 0;
    QTcpSocket *tcpsocket = 0;

    VlcInstance *vlcInst = 0;
    VlcMediaPlayer *player = 0;
    QLabel *connectionStatus = 0;
    QLabel *ipaddress = 0;

    VlcMedia *media = 0;

    QCheckBox *broadcastCheck = 0;
    QPushButton *directControlCheck = 0;

    QGridLayout* tunnelsCheckboxesLay = 0;
    QDialog *fullscreenDialog = 0;

    QMap<int,QHostAddress> *_neighboursMap;
    QString streamLocation;

    QTimer *_selfStreamResetTimer;


public slots:
    void zoomVideo();
    //void proxyClicked();
    void blockInternet();
    void unblockInternet();
    void tunnelTo(int neigbour);
    void removeTunnelTo(int neigbour);
    //void allowTo(quint32 ip);
    //void blockTo(quint32 ip);
    void neigbourhoodChanged();
    void newneighbour();
    void stationAddressChanged(int stationNo);
    void changeConnectionStatus(QAbstractSocket::SocketState state);
    void changeStationNo(int no);
    void tunnelCheckboxChanged(int state);
    void playStream(bool state);
    void startStream();
    void stopStream();


    void heartBeat();
    void connectionError(QAbstractSocket::SocketError err);

    void delayedPlay();
    void restartStream();
    void zoomClosed();
    void createPLCdialog();
signals:
    void directControl(int);
    void tunnelChanged(int from,int to,char ON);

};

#endif // CLIENTWATCHER_H
