#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "maindialog.h"
#include "author.h"
#include "streamsettingsdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_actionO_Autorze_triggered();

    void on_actionKolejno_triggered();

    void on_actionPo_cz_triggered(bool checked);

    void on_actionInternet_triggered(bool checked);

    void restartWindow();

    void on_actionWideo_Stream_triggered();

private:
    Ui::MainWindow *ui;

    mainDialog *dialog;

    Author* authorDialog;
    StreamSettingsDialog *streamSettings = 0;
    void loadConfig();

signals:

    void discoveryChange(bool);
    void internetChange(bool);
    void restartNeeded();
    //void changedClientList();
public slots:
    void videoSettingsChanged();

};

#endif // MAINWINDOW_H
