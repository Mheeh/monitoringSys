#include <controlpacket.h>

#include "settings.h"
#include "anetconfig.h"
#include "win32netconfig.h"




QDataStream &operator<<(QDataStream &out, const control::controlpacket &packet)
{
    out << (quint8)packet.totalSize << packet.srcIP << (quint8)packet.func;
    return out;
}

QDataStream &operator<<(QDataStream &out, const control::blockArgs &args)
{
    out << (quint8)args.par << args.low_port << args.high_port << args.ip;
    return out;
}

QDataStream &operator<<(QDataStream &out, const control::proxyArgs &args)
{
    out << args.low_port << args.high_port << args.listAddr << args.connAddr << (quint8)args.sw;
    return out;
}

QDataStream &operator<<(QDataStream &out, const control::routeArgs &args)
{
    out << args.dstAddr << args.mask << args.gateway;
    return out;
}

QDataStream &operator>>(QDataStream &in,  control::controlpacket &packet)
{
    quint8 total;
    quint8 func;
    in >> total >> packet.srcIP >> func;
    packet.srcIP = total;
    packet.func = func;
    //in >> packet.totalSize;
    return in;
}

QDataStream &operator>>(QDataStream &in,  control::blockArgs &args)
{
    quint8 par;
    in >> par >> args.low_port >> args.high_port >> args.ip;
    args.par = par;
    return in;
}

QDataStream &operator>>(QDataStream &in,  control::proxyArgs &args)
{
    quint8 sw;
    in >> args.low_port >> args.high_port >> args.listAddr >> args.connAddr >> sw;
    args.sw = sw;
    return in;
}

QDataStream &operator>>(QDataStream &in,  control::routeArgs &args)
{
    in >> args.dstAddr >> args.mask >> args.gateway;
    return in;
}


QDataStream &operator<<(QDataStream &out, const control::videoArgs &args)
{
    out << args.no << args.kbitrate << args.scalePerc;
    return out;
}

QDataStream &operator>>(QDataStream &in, control::videoArgs &args)
{
    in >> args.no >> args.kbitrate >> args.scalePerc;
    return in;
}
