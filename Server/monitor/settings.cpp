#include "settings.h"



Settings& Settings::getInstance()
{
    static Settings instance;
    return instance;

}
void Settings::configureMode(char MODE){
    switch(MODE){
        case SERVER_MODE:
            settings = new QSettings("D.E.U.","monitor");
        break;

        case CLIENT_MODE:
            settings = new QSettings("D.E.U.","watcher");
        break;

        default:
                break;

    }
}

void Settings::setConnected()
{
    this->settings->setValue("connected",1);
}

void Settings::setDisconnected()
{
    this->settings->setValue("connected",0);
}

int Settings::getConnected()
{
    return this->settings->value("connected",0).toInt();
}

void Settings::setCommPort(int port)
{
    this->settings->setValue("commport",port);
}

int Settings::getCommPort()
{
    return this->settings->value("commport",27500).toInt();
}

void Settings::setVideoPort(int port)
{
    this->settings->setValue("videoport",port);
}

int Settings::getVideoPort()
{
    return this->settings->value("videoport",8554).toInt();
}

void Settings::setPLCInterfaceAddr(QString plcInterfaceAddr)
{
    this->settings->setValue("PLCInterface",plcInterfaceAddr);
}

QString Settings::getPLCInterfaceAddr()
{
    return this->settings->value("PLCInterface","192.168.3.1").toString();
}

void Settings::setPLCAddr(QString plcAddr)
{
    this->settings->setValue("PLCAddr",plcAddr);
}

QString Settings::getPLCAddr()
{
    return this->settings->value("PLCAddr","192.168.3.250").toString();
}

void Settings::setVideoScale(int scale)
{
    this->settings->setValue("VideoScale",scale);
}

int Settings::getVideoScale(){
    return this->settings->value("VideoScale",100).toInt();
}

void Settings::setVideoBitRate(int kbps)
{
    this->settings->setValue("Videobitrate",kbps);
}

int Settings::getVideoBitRate(){
    return this->settings->value("Videobitrate",264).toInt();
}

QMap<QString, int> &Settings::getClients()
{
    return clients;
}


QString Settings::getHost(){
    return this->settings->value("HostIP","").toString();
}

void Settings::setHost(QString host){
    this->settings->setValue("HostIP",host);
}

void Settings::saveToStorage(){
    this->settings->sync();
}

void Settings::saveClients(){
    this->settings->beginWriteArray("clients",this->clients.size());
    QMap<QString,int>::Iterator it = this->clients.begin();
    for(int i = 0; i < this->clients.size(); i++, it++){
        this->settings->setArrayIndex(i);
        this->settings->setValue("IP",it.key());
        this->settings->setValue("windowNo",it.value());
    }
    this->settings->endArray();
}

void Settings::loadClients(){
    int c_count = this->settings->beginReadArray("clients");
    for (int i = 0; i < c_count; i++){
        this->settings->setArrayIndex(i);
        this->clients.insert(this->settings->value("IP").toString(),this->settings->value("windowNo").toInt());
    }
    this->settings->endArray();
}
