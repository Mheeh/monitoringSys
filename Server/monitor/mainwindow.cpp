#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "worksetting.h"

#include <QWidget>
#include <QHBoxLayout>
#include "clientwatcher.h"
#include <QScrollArea>
#include "settings.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    authorDialog = 0;
    ui->setupUi(this);
    QScrollArea *scrollArea;
    QHBoxLayout *hboxLay = new QHBoxLayout;

    QWidget *central = new QWidget(this);

    scrollArea = new QScrollArea;

    dialog = new mainDialog(central);

    QSizePolicy sizePolicy = QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    dialog->setSizePolicy(sizePolicy);
    scrollArea->setWidget(dialog);
    scrollArea->setMinimumSize(350,400);

    hboxLay->addWidget(scrollArea);
    central->setLayout(hboxLay);

    this->setCentralWidget(central);


    connect(dialog,SIGNAL(restartNeeded()),this,SLOT(restartWindow()));

    connect(this,SIGNAL(internetChange(bool )),dialog,SLOT(internetConnection(bool)));

    connect(this,SIGNAL(discoveryChange(bool)),dialog,SLOT(discovery(bool)));

    //connect(dialog,SIGNAL(newClient()),this,SLOT(repaint()));
    //QHBoxLayout *lay = new QHBoxLayout;
    /*
    lay->addWidget(new clientWatcher());
    lay->addWidget(new clientWatcher());
    lay->addWidget(new clientWatcher());
    */
    //widget->setLayout(lay);

    //widget->show();




    //this->setCentralWidget(dialog);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionO_Autorze_triggered()
{
    if(authorDialog == 0){
        authorDialog = new Author();
        authorDialog->show();
    }
    else{
        delete authorDialog;
        authorDialog = new Author();
        authorDialog->show();
    }
}

void MainWindow::on_actionKolejno_triggered()
{
    worksetting *t;
    t = new worksetting();
    connect(t,SIGNAL(accepted()),this->dialog,SLOT(settingChanged()));
    t->show();
}



void MainWindow::on_actionPo_cz_triggered(bool checked)
{
    emit discoveryChange(checked);

}

void MainWindow::on_actionInternet_triggered(bool checked)
{
    emit internetChange(checked);
}

void MainWindow::restartWindow()
{
    emit restartNeeded();
}

void MainWindow::on_actionWideo_Stream_triggered()
{
    streamSettings = new StreamSettingsDialog;
    connect(streamSettings,SIGNAL(accepted()),this,SLOT(videoSettingsChanged()));
    streamSettings->exec();
}

void MainWindow::videoSettingsChanged()
{
    Settings &sett = Settings::getInstance();
    sett.setVideoBitRate(streamSettings->getFPSSett());
    sett.setVideoScale(streamSettings->getscaleSett());
    streamSettings->deleteLater();
}
