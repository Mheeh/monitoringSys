#include "streamsettingsdialog.h"
#include "ui_streamsettingsdialog.h"
#include <QIntValidator>
#include "settings.h"

StreamSettingsDialog::StreamSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StreamSettingsDialog)
{
    ui->setupUi(this);
    QValidator *bitrateValidator = new QIntValidator(10,3000);
    QValidator *scaleValidator = new QIntValidator(20,150);
    ui->bitrateEdit->setValidator(bitrateValidator);
    ui->scaleEdit->setValidator(scaleValidator);

    ui->bitrateEdit->setText(QString::number(Settings::getInstance().getVideoBitRate()));
    ui->scaleEdit->setText(QString::number(Settings::getInstance().getVideoScale()));

    connect(ui->bitrateEdit,SIGNAL(editingFinished()),this,SLOT(checkbitratevalue()));
    connect(ui->scaleEdit,SIGNAL(editingFinished()),this,SLOT(checkScalevalue()));
    connect(ui->pushButton,SIGNAL(pressed()),this,SLOT(inputDefault()));
}

StreamSettingsDialog::~StreamSettingsDialog()
{
    delete ui;
}

int StreamSettingsDialog::getFPSSett()
{
    return _bitrateSett;
}

int StreamSettingsDialog::getscaleSett()
{
    return _scaleSett;
}

void StreamSettingsDialog::checkbitratevalue()
{
    _bitrateSett = ui->bitrateEdit->text().toInt();
    if(_bitrateSett <10 ){
        _bitrateSett = 10;
        ui->bitrateEdit->setText(QString::number(_bitrateSett));
    }
}

void StreamSettingsDialog::checkScalevalue()
{
    _scaleSett = ui->scaleEdit->text().toInt();
    if(_scaleSett <20 ){
        _bitrateSett = 20;
        ui->scaleEdit->setText(QString::number(_scaleSett));
    }
}

void StreamSettingsDialog::inputDefault()
{
    _scaleSett = 100;
    ui->scaleEdit->setText(QString::number(_scaleSett));
    _bitrateSett = 264;
    ui->bitrateEdit->setText(QString::number(_bitrateSett));
}
