#ifndef STREAMSETTINGSDIALOG_H
#define STREAMSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class StreamSettingsDialog;
}

class StreamSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StreamSettingsDialog(QWidget *parent = 0);
    ~StreamSettingsDialog();


    int getFPSSett();
    int getscaleSett();
private:
    Ui::StreamSettingsDialog *ui;
    int _bitrateSett = 0;
    int _scaleSett = 0;


public slots:
    void checkbitratevalue();
    void checkScalevalue();
    void inputDefault();
};

#endif // STREAMSETTINGSDIALOG_H
