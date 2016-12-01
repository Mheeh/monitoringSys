#include "plcdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QIntValidator>
#include <QRegExpValidator>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

BINcheckBox::BINcheckBox(quint8 id, QWidget *parent)
{
    _id = id;
    this->setParent(parent);
}

PLCDialog::PLCDialog(QWidget *parent)
{
    createDialog();
}

PLCDialog::PLCDialog(int stationNo, QHostAddress address, QWidget *parent)
{
    qDebug() << "QHostAddressString" << address.toString();
    createDialog();
    //this->setParent(parent);
    _address->setPlaceholderText(address.toString());
    _address->setText(address.toString());
    qDebug() << "PrzedDisabled";
    _address->setDisabled(true);
    _port->setText(QString::number(27501));
    this->setWindowTitle("PLC #"+QString::number(stationNo));
}

PLCDialog::~PLCDialog()
{

}

PLCDialog::createDialog()
{
    QValidator *numbers = new QIntValidator(0,65535);
    QValidator *hexNumber = new QRegExpValidator(QRegExp("[a-fA-F0-9]{0,4}"),this);
    _tcpsocket = new QTcpSocket;
    //QValidator *ipAddr = new QRegExpValidator("");

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QGridLayout *firstLine = new QGridLayout;
    _address = new QLineEdit("0.0.0.0",this);
    _port = new QLineEdit("0000",this);
    _port->setValidator(numbers);


    QGroupBox *modes = new QGroupBox("Tryb",this);
    QVBoxLayout *modesLay = new QVBoxLayout;

    _binaryTransmissionMode = new QRadioButton("BIN",this);
    _binaryTransmissionMode->setChecked(true);
    _ASCIITransmissionMode = new QRadioButton("ASCII",this);
    connect(_ASCIITransmissionMode,SIGNAL(toggled(bool)),this,SLOT(setASCII(bool)));


    modesLay->addWidget(_binaryTransmissionMode);
    modesLay->addWidget(_ASCIITransmissionMode);
    modes->setLayout(modesLay);


    firstLine->addWidget(new QLabel("Adres:",this),0,0);
    firstLine->addWidget(new QLabel("Port:",this),0,1);
    firstLine->addWidget(_address,1,0);
    firstLine->addWidget(_port,1,1);
    firstLine->addWidget(modes,0,2,2,1);

    mainLayout->addLayout(firstLine);



    QVBoxLayout *secondLine = new QVBoxLayout;
    QLabel *deviceLabel = new QLabel("Urządzenie początkowe:");
    secondLine->addWidget(deviceLabel);
    _headDevice = new QLineEdit();
    secondLine->addWidget(_headDevice);
    connect(_headDevice,SIGNAL(editingFinished()),this,SLOT(checkDeviceCode()));




    _textvalue = new QLineEdit("0",this);
    _textvalue->setValidator(hexNumber);
    _textvalue->setAlignment(Qt::AlignRight);
    secondLine->addWidget(new QLabel("Wartość:",this));
    connect(_textvalue,SIGNAL(editingFinished()),this,SLOT(recalculateValuefromText()));
    secondLine->addWidget(_textvalue);
    mainLayout->addLayout(secondLine);


    QGridLayout *thirdLine = new QGridLayout;
    for(int i = 15,j=0; i>=0; i--,j++){
        //qDebug()<< "i:"<<i;
        BINcheckBox *box = new BINcheckBox(i,this);
        connect(box,SIGNAL(clicked(bool)),this,SLOT(recalculateValuefromCheck(bool)));
        _binaryCheckbox.append(box);

        QLabel *lab = new QLabel(QString::number(i),this);

        thirdLine->addWidget(box,0,j);
        thirdLine->addWidget(lab,1,j);
    }

    mainLayout->addLayout(thirdLine);

    QHBoxLayout *fourthLine = new QHBoxLayout;
    QPushButton *set = new QPushButton("Ustaw",this);
    connect(set,SIGNAL(pressed()),this,SLOT(sendReadRequest()));
    QPushButton *read = new QPushButton("Odczyt",this);
    connect(read,SIGNAL(pressed()),this,SLOT(sendWriteRequest()));

    fourthLine->addWidget(set);
    fourthLine->addWidget(read);
    mainLayout->addLayout(fourthLine);

    this->setLayout(mainLayout);
}

PLCDialog::wrongDeviceError()
{
    QString message = "Podano nieprawidłowe urządzenie. \Obsługiwane urządzenia to:\n";

    QMap<QString,quint16>::Iterator it = DEVICE_CODE.begin();
    for(int i = 0; i < DEVICE_CODE.size(); i++, it++){
        message.append(it.key());
        if(i+1 < DEVICE_CODE.size()){
            message.append(", ");
        }
    }
    message.append(".");
    QMessageBox *msg = new QMessageBox(QMessageBox::Critical,tr("Złe parametry"),message);
    msg->setAttribute(Qt::WA_DeleteOnClose);
    msg->exec();
}

bool PLCDialog::checkDeviceCode()
{

    QString headDevice = _headDevice->text();
    qDebug() << "headDeviceText:" << headDevice;
    if(!headDevice.isEmpty()){
        int numberStart = headDevice.indexOf(QRegularExpression("[0-9]"));
        if(numberStart == -1 || numberStart == 0){
            return false;
        }

        QString deviceCode = headDevice.mid(0,numberStart);
        headDevice = headDevice.mid(numberStart);
        qDebug() << "After cut: " << headDevice;

        _deviceCode = DEVICE_CODE.value(deviceCode.toLower(),0);
        _deviceNumber = headDevice.toInt();
        if(_deviceCode == 0)return false;


        qDebug() << "Device code:" << QString::number(_deviceCode,16) << ", number: " << _deviceNumber;
        return true;

    }
    return false;
}

void PLCDialog::incomingResponse()
{
    SLMP_RESP_MSG packet;
    if(!_ascii){
        QDataStream stream(_tcpsocket);
        stream.setVersion(QDataStream::Qt_5_6);
        qDebug() << "otrzymano tcp";
        stream.setByteOrder(QDataStream::LittleEndian);

        while(_tcpsocket->bytesAvailable() < 9 &&  _tcpsocket->state() == QTcpSocket::ConnectedState);
        stream >> packet;
    }else{
    //ASCII code

    }
    _tcpsocket->disconnectFromHost();

    if(packet.usERRORCODE == 0 && packet.usDataLength >= 4){
        _textvalue->setText(QString::number(packet.data,16).toUpper());
        recalculateValuefromText();

    }

}

void PLCDialog::sendReadRequest()
{
    if(!checkDeviceCode()){
        wrongDeviceError();
    }
    else{
        if(!_ascii){
            QByteArray buffer;
            QDataStream stream(&buffer,QIODevice::ReadWrite);
            stream.setVersion(QDataStream::Qt_5_6);
            stream.setByteOrder(QDataStream::ByteOrder::LittleEndian);

            SLMP_REQ_READ packet;
            packet.headdevCode = _deviceCode;
            packet.headDevNo = _deviceNumber;

            stream << packet;

            _tcpsocket->connectToHost(QHostAddress(_address->text()),_port->text().toInt());
           _tcpsocket->waitForConnected(10000);

            if(_tcpsocket->state()!= QTcpSocket::ConnectedState){
                QMessageBox *t;
                t =new QMessageBox(QMessageBox::Critical,tr("Błąd połączenia"),"Nie udało się nawiązać połączenia");
                t->setAttribute(Qt::WA_DeleteOnClose);
                t->exec();
            }else{
                _tcpsocket->write(buffer);
            }
        }else{
            QMessageBox *t;
            t =new QMessageBox(QMessageBox::Information,tr("Brak wsparcia"),"Komunikacja w trybie ASCII nie jest wspierana!");
            t->setAttribute(Qt::WA_DeleteOnClose);
            t->exec();
        }

    }
}

void PLCDialog::sendWriteRequest()
{
    if(!checkDeviceCode()){
        wrongDeviceError();
    }
    else{
        if(!_ascii){
            QByteArray buffer;
            QDataStream stream(&buffer,QIODevice::ReadWrite);
            stream.setVersion(QDataStream::Qt_5_6);
            stream.setByteOrder(QDataStream::ByteOrder::LittleEndian);

            SLMP_REQ_Write packet;
            packet.Data = _binvalue;
            packet.headdevCode = _deviceCode;
            packet.headDevNo = _deviceNumber;

            stream << packet;

            _tcpsocket->connectToHost(QHostAddress(_address->text()),_port->text().toInt());
            _tcpsocket->waitForConnected(10000);

            if(_tcpsocket->state()!= QTcpSocket::ConnectedState){
                QMessageBox *t;
                t =new QMessageBox(QMessageBox::Critical,tr("Błąd połączenia"),"Nie udało się nawiązać połączenia");
                t->setAttribute(Qt::WA_DeleteOnClose);
                t->exec();
            }else{
                _tcpsocket->write(buffer);
            }
        }
        else{
            QMessageBox *t;
            t =new QMessageBox(QMessageBox::Information,tr("Brak wsparcia"),"Komunikacja w trybie ASCII nie jest wspierana!");
            t->setAttribute(Qt::WA_DeleteOnClose);
            t->exec();
        }

    }


}

void PLCDialog::recalculateValuefromText()
{
    if(_textvalue->text().isEmpty()){
        _binvalue = 0;
        _textvalue->setText("0");
    }else
    {
        _binvalue = _textvalue->text().toInt(nullptr,16);
    }
    for(int i = 15,j = 0; i>=0;i--,j++){
        quint16 mask = 1 << i;
        _binaryCheckbox[j]->setChecked(_binvalue & mask);
    }
    _textvalue->setText(_textvalue->text().toUpper());
}

void PLCDialog::recalculateValuefromCheck(bool checked)
{
    BINcheckBox *box = qobject_cast<BINcheckBox*>(sender());
    qDebug() << "id:" << box->_id;
    quint16 t_value = 0x01;
    t_value = t_value << box->_id;
    qDebug() << "mask:" << t_value;
    if(checked){
        _binvalue = _binvalue | t_value;
    }
    else{
        t_value = ~t_value;
        _binvalue = _binvalue & t_value;
    }
    qDebug() << "bin:" << _binvalue;
    _textvalue->setText(QString::number(_binvalue,16).toUpper());
}

void PLCDialog::setASCII(bool is)
{
 _ascii = is;
}

//TODO refactoring - lazy programing
QDataStream &operator<<(QDataStream &out, const SLMP_REQ_Write &packet)
{
    out << packet.ulFrameType << packet.usNetNumber << packet.usNodeNumber << packet.usProcNumber << packet.multidrop << packet.usDataLength
        << packet.usReserveCode << packet.usCommand << packet.usSubCommand << packet.headdevCode << packet.headDevNo << packet.noDev << packet.Data;
    return out;
}

QDataStream &operator<<(QDataStream &out, const SLMP_REQ_READ &packet)
{
    out << packet.ulFrameType << packet.usNetNumber << packet.usNodeNumber << packet.usProcNumber << packet.multidrop << packet.usDataLength
        << packet.usReserveCode << packet.usCommand << packet.usSubCommand << packet.headdevCode << packet.headDevNo << packet.noDev;
    return out;
}

QDataStream &operator>>(QDataStream &in, SLMP_RESP_MSG &packet)
{
     in >> packet.ulFrameType >> packet.usNetNumber >> packet.usNodeNumber >> packet.usProcNumber >> packet.multidrop >> packet.usDataLength >> packet.usERRORCODE;
     if(packet.usERRORCODE == 0 && packet.usDataLength >= 4){
         in >> packet.data;
     }
     return in;
}
