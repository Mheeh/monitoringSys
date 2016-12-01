#ifndef ANETCONFIG_H
#define ANETCONFIG_H

#include <string>
#include <QList>
#include <QNetworkInterface>


#define WIN32NETSH 1
#define BLOCK_CONNECTION 0
#define ALLOW_CONNECTION 1

class ANetConfig{
    public:
        ANetConfig();
        static ANetConfig* create(int mode);

        virtual int saveNetConfig() = 0;
        virtual int restoreNetConfig() = 0;

        virtual int enableForwarding() = 0;
        virtual int disableForwarding() = 0;
        virtual int setGateway(std::string IP,std::string mask,std::string GW) = 0;

        virtual QList<QHostAddress> getHostIP();

        virtual int addHostIP(std::string DEV,std::string IP) = 0;
        virtual int removeHostIP(std::string DEV,std::string IP) = 0;

        virtual int blockIP(std::string IP) = 0;
        virtual int removeRuleIP(std::string IP) = 0;
        virtual int allowIP(std::string IP) = 0;

        virtual int allowPort(int port) = 0;
        virtual void removeRulePort(int port) = 0;
        virtual int blockPort(int port) = 0;

        virtual int forwardPort(std::string listenIP,int port_lower,int port_higher,std::string dstIP, char sw = 0) = 0;
        virtual int removeForward(std::string listenIP,int port_lower,int port_higher,std::string dstIP) = 0;


        virtual int addRoute(std::string dstIP,std::string mask,std::string GW) = 0;
        virtual int delRoute(std::string dstIP,std::string mask,std::string GW) = 0;
        virtual int setDefaultGW(std::string GW) = 0;

        virtual int setDefaultPolicy(char policy) = 0;

        virtual bool testConnectivity(std::string IP) = 0;
};

#endif // ANETCONFIG_H
