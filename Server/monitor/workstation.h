#ifndef WORKSTATION_H
#define WORKSTATION_H

#include <QObject>
#include <string>

class Workstation : public QObject
{
    Q_OBJECT
public:
    explicit Workstation(QObject *parent = 0);
    Workstation(std::string ip);
    std::string getIp();
    void setIp();

private:
    std::string ipAddress;
    char stationId;
    bool selected;
signals:

public slots:
};

#endif // WORKSTATION_H
