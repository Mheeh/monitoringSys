#include "anetconfig.h"

#include "win32netconfig.h"

ANetConfig::ANetConfig(){}

ANetConfig* ANetConfig::create(int mode){
    switch(mode){
        case WIN32NETSH:
            return new Win32NetConfig();
            break;
        default:
            return new Win32NetConfig();
            break;
    }
}

QList<QHostAddress> ANetConfig::getHostIP(){
    QNetworkInterface t;
    return t.allAddresses();
}
