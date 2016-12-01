#ifndef WIN32NETCONFIG_H
#define WIN32NETCONFIG_H

#include "anetconfig.h"
#include <QProcess>

class Win32NetConfig : public ANetConfig
{
public:
    Win32NetConfig();

    virtual int saveNetConfig();
    virtual int restoreNetConfig();

    virtual int blockIP(std::string IP);
    virtual int removeRuleIP(std::string IP);
    virtual int allowIP(std::string IP);

    virtual int allowPort(int port);
    virtual void removeRulePort(int port);
    virtual int blockPort(int port);

    virtual int forwardPort(std::string listenIP,int port_lower,int port_higher,std::string dstIP,char sw = 0);
    virtual int removeForward(std::string listenIP, int port_lower, int port_higher, std::string dstIP);

    virtual int addRoute(std::string dstIP, std::string mask, std::string GW);
    virtual int delRoute(std::string dstIP, std::string mask, std::string GW);

    virtual int setDefaultGW(std::string GW);
    virtual int setDefaultPolicy(char policy);


    virtual int enableForwarding();
    virtual int disableForwarding();
    virtual int setGateway(std::string IP,std::string mask,std::string GW);
    virtual int addHostIP(std::string DEV, std::string IP);
    virtual int removeHostIP(std::string DEV,std::string IP);

    virtual bool testConnectivity(std::string IP);
private:
    QProcess p;
};

#endif // WIN32NETCONFIG_H
