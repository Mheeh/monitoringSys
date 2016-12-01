#include "worksetting.h"
#include "ui_worksetting.h"
#include <QMap>
#include "settings.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QSizePolicy>
#include <QLabel>
#include <QDebug>

worksetting::worksetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::worksetting)
{
    ui->setupUi(this);
    Settings &sett = Settings::getInstance();
    sett.loadClients();


    QSizePolicy size = QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    QSizePolicy sizeX = QSizePolicy(QSizePolicy::Minimum,QSizePolicy::MinimumExpanding);

    QMap<QString,int> &clientMap = sett.getClients();
    QValidator *validator = new QIntValidator(this);

    QLabel *column0 = new QLabel("Adres IP",this);
    column0->setSizePolicy(sizeX);
    QLabel *column1 = new QLabel("Numer stanowiska",this);
    column1->setSizePolicy(sizeX);

    QWidget *insideScroll = new QWidget;


/*
    for(int i =0; i<120;i++){
        clientMap[QString::number(i)] = i+2;
    }
*/
    grid = new QGridLayout;
    grid->addWidget(column0,0,0);
    grid->addWidget(column1,0,1);
    QLineEdit *text;
    int row;

    QMap<QString,int>::Iterator it = clientMap.begin();
    for(int i = 0; i < clientMap.size(); i++, it++){
        row = i+1;
        QLabel *label = new QLabel(it.key(),this);
        label->setSizePolicy(sizeX);
        LabelMap[it.key()] = label;
        grid->addWidget(label,row,0);
        text = new QLineEdit(this);
        text->setSizePolicy(size);
        text->setText(QString::number(it.value()));
        text->setValidator(validator);
        grid->addWidget(text,row,1);
        lineEditMap[it.key()] = text;

    }
    addIpButton = new QPushButton("Dodaj IP",this);
    grid->addWidget(addIpButton,0,3);
    connect(addIpButton,SIGNAL(pressed()),this,SLOT(showAddIP()));
    insideScroll->setLayout(grid);
    ui->scrollArea->setWidget(insideScroll);

    this->setAttribute(Qt::WA_DeleteOnClose);

}


worksetting::~worksetting()
{
    delete ui;
}

void worksetting::accept()
{
    Settings &sett = Settings::getInstance();
    QMap<QString,int> &clientMap = sett.getClients();

    clientMap.clear();
    QMap<QString,QLineEdit*>::Iterator it = lineEditMap.begin();
    for(int i =0; i<lineEditMap.size();i++,it++){
     QLineEdit *value = it.value();
     clientMap[it.key()] = value->text().toInt();
    }
    sett.saveClients();


    emit accepted();
    this->deleteLater();
}

void worksetting::showAddIP()
{
    addIPdialog = new addDelIPDialog;
    addIPdialog->setWindowTitle("Dodaj/Usuń ip");
    QGridLayout *lay = new QGridLayout(addIPdialog);


    QLineEdit *ip = new QLineEdit(addIPdialog);
    lay->addWidget(ip,0,0,1,2);

    QPushButton *addIP = new QPushButton("Dodaj",addIPdialog);
    connect(addIP,SIGNAL(pressed()),this,SLOT(addIP()));

    QPushButton *delIP = new QPushButton("Usuń",addIPdialog);
    connect(delIP,SIGNAL(pressed()),this,SLOT(removeIP()));

    lay->addWidget(addIP,1,0);
    lay->addWidget(delIP,1,1);

    addIPdialog->setLayout(lay);

    addIPdialog->ip = ip;

    addIPdialog->setAttribute(Qt::WA_DeleteOnClose);
    addIPdialog->exec();
}

void worksetting::addIP()
{
    if(!lineEditMap.contains(addIPdialog->ip->text())){
    qDebug() << "Kliknieto add";
    int row = lineEditMap.size()+1;
    QSizePolicy sizeX = QSizePolicy(QSizePolicy::Minimum,QSizePolicy::MinimumExpanding);

    QValidator *validator = new QIntValidator(this);
    qDebug() << addIPdialog->ip->text();
    QLabel *label = new QLabel(addIPdialog->ip->text(),this);
    label->setSizePolicy(sizeX);
    LabelMap[addIPdialog->ip->text()] = label;

    QLineEdit *text;
    grid->addWidget(label,row,0);
    text = new QLineEdit(this);
    text->setSizePolicy(sizeX);
    text->setText("0");
    text->setValidator(validator);
    grid->addWidget(text,row,1);
    lineEditMap[addIPdialog->ip->text()] = text;
    }
}
void worksetting::removeIP(){

    lineEditMap[addIPdialog->ip->text()]->deleteLater();
    lineEditMap.remove(addIPdialog->ip->text());
    LabelMap[addIPdialog->ip->text()]->deleteLater();
    LabelMap.remove(addIPdialog->ip->text());
}
