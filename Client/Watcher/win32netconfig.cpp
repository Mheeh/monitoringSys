#include "win32netconfig.h"
#include <windows.h>
#include <winreg.h>

#include <QProcess>

Win32NetConfig::Win32NetConfig()
{

}

int Win32NetConfig::saveNetConfig(){
    int t = 0;
    p.start("netsh dump > netshConfig.back");
    p.waitForFinished();
    t = p.exitCode();
    p.start("netsh advfirewall export netshFWConfig.wvw");
    p.waitForFinished();
    t+=p.exitCode();
    return t;
}

int Win32NetConfig::restoreNetConfig(){
    int t = 0;
    p.start("netsh exec netshConfig.back");
    p.waitForFinished();
    t = p.exitCode();
    p.start("netsh advfirewall import netshFWConfig.wvw");
    p.waitForFinished();
    t+= p.exitCode();
    return t;
}


int Win32NetConfig::enableForwarding(){
    HKEY hKey;
    int t = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                  TEXT("p.start\\CurrentControlSet\\Services\\Tcpip\\Parameters"),
                  0, KEY_ALL_ACCESS, &hKey);



    DWORD val = 1;
    t = RegSetValueEx(hKey,
                  TEXT("IPEnableRouter"),0,
                  REG_DWORD,(const BYTE*)&val,sizeof(val));
    t =  RegCloseKey(hKey);
    return t;
}

int Win32NetConfig::disableForwarding(){
    HKEY hKey;
    RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                  TEXT("p.start\\CurrentControlSet\\Services\\Tcpip\\Parameters"),
                  0, KEY_ALL_ACCESS, &hKey);

    DWORD val = 0;
    int t= RegSetValueEx(hKey,
                  TEXT("IPEnableRouter"),0,
                  REG_DWORD,(const BYTE*)&val,sizeof(val));

    t =  RegCloseKey(hKey);
    return t;
}


int Win32NetConfig::setDefaultGW(std::string GW){
    std::string command = "route add 0.0.0.0 MASK 0.0.0.0 "+GW;
    p.start(command.c_str());
    p.waitForFinished();
    return p.exitCode();
}

int Win32NetConfig::setGateway(std::string IP,std::string mask,std::string GW){
    std::string command = "route add " +IP+"mask "+mask+" "+GW;
    p.start(command.c_str());
    p.waitForFinished();
    return p.exitCode();
}

int Win32NetConfig::blockIP(std::string IP){
    std::string command = "netsh advfirewall firewall add rule name=monitor"+IP+" dir=IN remoteip="+IP+" action=block";
    p.start(command.c_str());
    p.waitForFinished();
    command = "netsh advfirewall firewall add rule name=monitor"+IP+" dir=OUT remoteip="+IP+" action=block";
    p.start(command.c_str());
    p.waitForFinished();
    return p.exitCode();
}

int Win32NetConfig::removeRuleIP(std::string IP)
{
    std::string command = "netsh advfirewall firewall delete name=monitor"+IP;
    p.start(command.c_str());
    p.waitForFinished();
    return p.exitCode();
}

int Win32NetConfig::allowIP(std::string IP){
        std::string command = "netsh advfirewall firewall add rule name=monitor"+IP+" dir=IN remoteip="+IP+" action=allow";
        p.start(command.c_str());
        p.waitForFinished();
        command = "netsh advfirewall firewall add rule name=monitor"+IP+" dir=OUT remoteip="+IP+" action=allow";
        p.start(command.c_str());
        p.waitForFinished();
        return p.exitCode();

}

int Win32NetConfig::allowPort(int port)
{

    std::string command = "netsh advfirewall firewall delete rule name=monitorP"+QString::number(port).toStdString();
    p.start(command.c_str());
    p.waitForFinished();
    int test = p.exitCode();
    command = "netsh advfirewall firewall add rule name=monitorP"+QString::number(port).toStdString()+" dir=IN protocol=UDP localport="+QString::number(port).toStdString()+" action=allow";
    p.start(command.c_str());
    p.waitForFinished();
    test +=p.exitCode();
    command = "netsh advfirewall firewall add rule name=monitorP"+QString::number(port).toStdString()+" dir=IN protocol=TCP localport="+QString::number(port).toStdString()+" action=allow";
    p.start(command.c_str());
    p.waitForFinished();
    test +=p.exitCode();
    command = "netsh advfirewall firewall add rule name=monitorP"+QString::number(port).toStdString()+" dir=OUT protocol=UDP remoteport="+QString::number(port).toStdString()+" action=allow";
    p.start(command.c_str());
    p.waitForFinished();
    test +=p.exitCode();
    command = "netsh advfirewall firewall add rule name=monitorP"+QString::number(port).toStdString()+" dir=OUT protocol=TCP remoteport="+QString::number(port).toStdString()+" action=allow";
    p.start(command.c_str());
    p.waitForFinished();
    test +=p.exitCode();
    return test;
}

void Win32NetConfig::removeRulePort(int port)
{
    std::string command = "netsh advfirewall firewall delete rule name=monitorP"+QString::number(port).toStdString();
    p.start(command.c_str());
    p.waitForFinished();
}

int Win32NetConfig::blockPort(int port)
{
    std::string command = "netsh advfirewall firewall delete rule name=monitorP"+QString::number(port).toStdString();
    p.start(command.c_str());
    p.waitForFinished();
    int test = p.exitCode();
    command = "netsh advfirewall firewall add rule name=monitorP"+QString::number(port).toStdString()+" dir=IN protocol=UDP localport="+QString::number(port).toStdString()+" action=block";
    p.start(command.c_str());
    p.waitForFinished();
    test +=p.exitCode();
    command = "netsh advfirewall firewall add rule name=monitorP"+QString::number(port).toStdString()+" dir=IN protocol=TCP localport="+QString::number(port).toStdString()+" action=block";
    p.start(command.c_str());
    p.waitForFinished();
    test +=p.exitCode();
    command = "netsh advfirewall firewall add rule name=monitorP"+QString::number(port).toStdString()+" dir=OUT protocol=UDP remoteport="+QString::number(port).toStdString()+" action=block";
    p.start(command.c_str());
    p.waitForFinished();
    test +=p.exitCode();
    command = "netsh advfirewall firewall add rule name=monitorP"+QString::number(port).toStdString()+" dir=OUT protocol=TCP remoteport="+QString::number(port).toStdString()+" action=block";
    p.start(command.c_str());
    p.waitForFinished();
    test +=p.exitCode();
    return test;
}

int Win32NetConfig::forwardPort(std::string listenIP, int port_lower, int port_higher, std::string dstIP,char sw)
{
    int test = 0;
    if(sw ==0){
    if(port_higher==0||port_higher < port_lower){
        port_higher=port_lower;
    }
    for(int i = port_lower; i<=port_higher;i++){
    std::string command = "netsh interface portproxy add v4tov4 listenport="+QString::number(i).toStdString()+" listenaddress="+listenIP+" connectaddress="+dstIP;
    p.start(command.c_str());
    p.waitForFinished();
    test+=p.exitCode();
    }
    }else{
        std::string command = "netsh interface portproxy add v4tov4 listenport="+QString::number(port_lower).toStdString()+" listenaddress="+listenIP+" connectport="+QString::number(port_higher).toStdString()+" connectaddress="+dstIP;
        p.start(command.c_str());
        p.waitForFinished();
        test+=p.exitCode();
    }
    return test;
}

int Win32NetConfig::removeForward(std::string listenIP, int port_lower, int port_higher, std::string dstIP)
{

    int test = 0;
    if(port_higher==0||port_higher < port_lower){
        port_higher=port_lower;
    }
    for(int i = port_lower; i<=port_higher;i++){
    std::string command = "netsh interface portproxy delete v4tov4 listenport="+QString::number(i).toStdString()+" listenaddress="+listenIP;
    p.start(command.c_str());
    p.waitForFinished();
    test+=p.exitCode();
    }
    return test;
}

int Win32NetConfig::addRoute(std::string IP,std::string mask,std::string GW){
    std::string t = "route add "+IP+" mask "+mask+" "+GW;
    p.start(t.c_str());
    p.waitForFinished();
    return p.exitCode();
}

int Win32NetConfig::delRoute(std::string IP,std::string mask,std::string GW){
    std::string command = "route delete "+IP+" mask "+mask+" "+GW;
    p.start(command.c_str());
    p.waitForFinished();
    return p.exitCode();
}

int Win32NetConfig::addHostIP(std::string DEV,std::string IP){
    std::string command = "netsh interface ipv4 add address name="+DEV+" addr="+IP;
    p.start(command.c_str());
    p.waitForFinished();
    return p.exitCode();
}

int Win32NetConfig::removeHostIP(std::string DEV,std::string IP){
    std::string command = "netsh interface ipv4 delete address name="+DEV+" addr="+IP;
    p.start(command.c_str());
    p.waitForFinished();
    return p.exitCode();
}

int Win32NetConfig::setDefaultPolicy(char policy){
    switch(policy){
        case ALLOW_CONNECTION:
            p.start("netsh advfirewall set allprofiles firewallpolicy blockinbound,allowoutbound");
            p.waitForFinished();
            return p.exitCode();
        break;

        case BLOCK_CONNECTION:
            p.start("netsh adfirewall set allprofiles firewallpolicy blockinbound,blockoutbound");
            p.waitForFinished();
            return p.exitCode();
        break;
        default:
        return -50;
    }
}

bool Win32NetConfig::testConnectivity(std::string IP){
    std::string command = "ping "+IP+" -n 1 -w 50";
    p.start(command.c_str());
    p.waitForFinished();
    int test = p.exitCode();
    if(test==0){
        return true;
    }else{
        return false;
    }
}
