#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkInterface>
#include <QList>
#include <QProcess>
#include <QDir>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->comboBox->addItem("BRAK");
    QList<QNetworkInterface> t = QNetworkInterface::allInterfaces();
    for(int i = 0; i< t.size() ; i++){
        if(t[i].name().contains("ethernet")){
        ui->comboBox->addItem(t[i].humanReadableName());
        }
    qDebug() << "Indeks: " << t[i].index() << "Name:" << t[i].humanReadableName() ;
    }
    QDir dir;
    qDebug() << dir.absolutePath();
}

MainWindow::~MainWindow()
{
    delete ui;
}


//usun
void MainWindow::on_pushButton_2_clicked()
{
    QProcess p;
    //p.execute("sc delete Watcher");
    p.execute("schtasks /DELETE /TN Watcher /F");
    p.waitForFinished();


    p.execute("netsh advfirewall firewall delete rule name=watcher");
    p.waitForFinished();

    qDebug() << ui->comboBox->currentIndex();
    /*
   QDialog *qdialog;
        QVBoxLayout *lay;
        lay->addWidget(new QLabel(tr("Napotkano błąd!")));
        qdialog->setLayout(lay);
        qdialog->show();

    */
    this->close();
}

void MainWindow::on_pushButton_clicked()
{
    QProcess p;
    QDir dir;

    QString starter = dir.absolutePath()+"/"+"starter.exe";
    starter = dir.toNativeSeparators(starter);
    //starter.replace(tr("/"),tr("\\"));

    QString watcher = dir.absolutePath()+"/"+"Watcher.exe";
    watcher = dir.toNativeSeparators(watcher);
    qDebug() << "Scieza:" << starter;

    QString ffmpeg = dir.absolutePath()+"/ffmpeg/bin/"+"ffmpeg.exe";
    ffmpeg = dir.toNativeSeparators(ffmpeg);
    //qDebug() << starter;
    //p.execute("sc create Watcher start=auto binPath="+watcher+" DisplayName=Watcher");
    p.execute("schtasks /Create /SC ONLOGON /TN Watcher /TR \""+starter+"\" /F /DELAY 0002:00");
    p.waitForFinished();


    p.execute("netsh advfirewall firewall add rule name=FFMPEG dir=out program=\""+ffmpeg+"\"  action=allow");
    p.waitForFinished();

    p.execute("netsh advfirewall firewall add rule name=FFMPEG dir=in edge=yes program=\""+ffmpeg+"\"  action=allow");
    p.waitForFinished();

    p.execute("netsh advfirewall firewall add rule name=watcher dir=in edge=yes program=\""+watcher+"\"  action=allow");
    p.waitForFinished();


    //p.execute("netsh advfirewall firewall add rule name=watcher dir=in edge=yes program=\""+dir.absolutePath()+"\\WatcherInstaller.exe\" action=allow");
    //p.waitForFinished();

    p.execute("netsh advfirewall firewall add rule name=watcher dir=out program=\""+watcher+"\"  action=allow");
    p.waitForFinished();
    if(ui->comboBox->currentIndex()>0){
    qDebug() << p.errorString();
    p.execute("netsh interface ipv4 add address \""+ui->comboBox->currentText()+"\" 192.168.3.1 255.255.255.0");

    p.waitForFinished();

    qDebug() << p.errorString();
    }

    p.execute("schtasks /RUN /I /TN Watcher");
    p.waitForFinished();


    this->close();
}
