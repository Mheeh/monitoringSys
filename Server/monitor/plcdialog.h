#ifndef PLCDIALOG_H
#define PLCDIALOG_H

#include <QObject>
#include <QDialog>
#include <QTcpSocket>
#include <QPushButton>
#include <QByteArray>
#include <QList>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QHostAddress>
#include <QMap>
#include <QDataStream>

static QMap<QString,quint16> DEVICE_CODE{{"x",0x9c00},{"y",0x9D00},{"m",0x9000},{"d",0xA00},{"sm",0x9100},{"sd",0xA900}};
//Header is in big-endiannes
#define SLMP_REQ 0x00D0 //should be 0xD000 but thanks to inconsistent endianess of Subheader and rest of message for easier parsing
#define SLMP_RESP 0x0050 //should be 0x5000 but same as above

const QString SLM_REQ_ASCII = "D000";
const QString SLM_RESP_ASCII ="5000";

//rest of packet is in Little-Endianness with commands
const quint16 COMM_BATWrite = 0x1401;

//writing in units of 16bytes to 1bit devices.
const quint16 SUBCOMM_BATCHREAD = 0x0000;

/*[ Structure of SLMP Info  ]*/
struct SLMP_REQ_Write
{
    quint16	ulFrameType = SLMP_REQ;			/* Frame Type - subheader*/
    // unsigned short	usSerialNumber;			/* Serial Number - not needed according to iQ-F FX5 smlp manual*/
    quint8	usNetNumber = 0;			/* Network Number  - x00 for FX5*/
    quint8	usNodeNumber = 0xFF;			/* Node Number - 0xFF for FX5 other values not supported*/
    quint8	usProcNumber = 0x03FF;			/* Processor Number - 0x03FF Default Proc*/
    quint8 multidrop = 0x0000;              //multidrop connection not used for FX5u
    quint16	usDataLength = 16;			/* Data Length  = Command-2,SubCom-2,HeadDev-4,DevCode-2,NoDev-2,Data-2,Reserve-2*/
    quint16	usReserveCode = 0;				/* Reserved Code */

    quint16	usCommand = 0x1401;				/* BatchWrtie Command */
    quint16	usSubCommand = 0x0002;			/* Sub Command  - according to 36 page SMLP manual 4digitCode/8digit number spec Data size word*/

    quint32 headDevNo;
    quint16 headdevCode;
    quint16  noDev = 0x0001;
    quint16	Data;				/* Data */
};

struct SLMP_REQ_READ
{
    quint16	ulFrameType = SLMP_REQ;			/* Frame Type - subheader*/
    // unsigned short	usSerialNumber;			/* Serial Number - not needed according to iQ-F FX5 smlp manual*/
    quint8	usNetNumber = 0;			/* Network Number  - x00 for FX5*/
    quint8	usNodeNumber = 0xFF;			/* Node Number - 0xFF for FX5 other values not supported*/
    quint8	usProcNumber = 0x03FF;			/* Processor Number - 0x03FF Default Proc*/
    quint8 multidrop = 0x0000;              //multidrop connection not used for FX5u
    quint16	usDataLength = 14;			/* Data Length  = Command-2,SubCom-2,HeadDev-4,DevCode-2,NoDev-2,Data-0,Reserve-2*/
    quint16	usReserveCode = 0;				/* Reserved Code */

    quint16	usCommand = 0x0401;				/* BatchWrtie Command */
    quint16	usSubCommand = 0x0002;			/* Sub Command  - according to 36 page SMLP manual 4digitCode/8digit number spec Data size word*/

    quint32 headDevNo;
    quint16 headdevCode;
    quint16  noDev = 0x0001;
};

struct SLMP_RESP_MSG
{
    quint16	ulFrameType = SLMP_RESP;			/* Frame Type - subheader*/
    // unsigned short	usSerialNumber;			/* Serial Number - not needed according to iQ-F FX5 smlp manual*/
    quint8	usNetNumber = 0;			/* Network Number  - x00 for FX5*/
    quint8	usNodeNumber = 0xFF;			/* Node Number - 0xFF for FX5 other values not supported*/
    quint8	usProcNumber = 0x03FF;			/* Processor Number - 0x03FF Default Proc*/
    quint8 multidrop = 0x0000;              //multidrop connection not used for FX5u
    quint16	usDataLength = 16;			/* Data Length  = Command-2,SubCom-2,HeadDev-4,DevCode-2,NoDev-2,Data-2,Reserve-2*/
    quint16	usERRORCODE = 0;				/* Reserved Code */
    quint16 data = 0;

};




class BINcheckBox : public QCheckBox
{
  Q_OBJECT
public:
    explicit BINcheckBox(quint8 id,QWidget *parent = 0);
    quint8 _id;

};

class PLCDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PLCDialog(QWidget *parent = 0);
    explicit PLCDialog(int stationNo,QHostAddress address,QWidget *parent = 0);
    ~PLCDialog();

private:
    bool _read;
    bool _ascii = false;
    quint16 _binvalue = 0;
    quint16 _deviceCode = 0;
    quint32 _deviceNumber = 0;

    QTcpSocket *_tcpsocket;
    QByteArray _buffer;

    QLineEdit *_headDevice;

    QLineEdit *_address;
    QLineEdit *_port;
    QLineEdit *_textvalue;
    QList<BINcheckBox* > _binaryCheckbox;

    QRadioButton *_binaryTransmissionMode;
    QRadioButton *_ASCIITransmissionMode;


    createDialog();
    wrongDeviceError();

private slots:
     bool checkDeviceCode();
    void incomingResponse();
    void sendReadRequest();
    void sendWriteRequest();
    void recalculateValuefromText();
    void recalculateValuefromCheck(bool checked);
    void setASCII(bool is);
};


QDataStream & operator<<(QDataStream &out,const SLMP_REQ_Write &packet);
QDataStream & operator<<(QDataStream &out,const SLMP_REQ_READ &packet);

QDataStream & operator>>(QDataStream &in, SLMP_RESP_MSG &packet);
QString SLMP_REQ_Write_toASCII(SLMP_REQ_Write packet);
QString SLMP_REQ_Read_toASCII(SLMP_REQ_READ packet);

SLMP_REQ_READ ASCII_to_SLMP_RESP(QByteArray _buffer);
#endif // PLCDIALOG_H
