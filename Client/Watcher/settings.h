#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QMap>

#define CLIENT_MODE 0
#define SERVER_MODE 1

class Settings
{
public:
    static Settings& getInstance();

    QString getHost();
    void setHost(QString host);

    void loadClients();
    void saveClients();

    void saveWindow();
    void loadWindows();

    void saveToStorage();

    void configureMode(char MODE);

    void setConnected();
    void setDisconnected();
    int getConnected();

    void setCommPort(int port);
    int getCommPort();

    void setVideoPort(int port);
    int getVideoPort();

    //interface directly connected to PLC
    void setPLCInterfaceAddr(QString plcInterfaceAddr);
    QString getPLCInterfaceAddr();

    //PLC address
    void setPLCAddr(QString plcAddr);
    QString getPLCAddr();

    //scale of screen
    void setVideoScale(QString scale);
    QString getVideoScale();

    void setVideoFPS(QString fps);
    QString getVideoFPS();

    //path absolute path to ffmpeg.exe
    void setFFMPEGpath(QString path);
    QString getFFMPEGpath();

private:
    Settings(){}
    Settings(Settings const&) = delete;
    void operator=(Settings const&) = delete;
    QSettings *settings = 0;


    QMap<QString,int> clients;
    char connected;
};


#endif // SETTINGS_H
