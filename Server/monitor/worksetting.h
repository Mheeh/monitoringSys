#ifndef WORKSETTING_H
#define WORKSETTING_H

#include <QDialog>
#include "mapclients.h"

#include <QAbstractTableModel>
#include <QMap>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>

namespace Ui {
class worksetting;
}

class addDelIPDialog : public QDialog{
public:
    QLineEdit *ip;
};

class worksetting : public QDialog
{
    Q_OBJECT
public:
    explicit worksetting(QWidget *parent = 0);
    ~worksetting();

private:
    Ui::worksetting *ui;
    int changes;
    QPushButton *addIpButton;
    QMap<QString,QLineEdit*> lineEditMap;
    QMap<QString,QLabel*> LabelMap;
    addDelIPDialog *addIPdialog = 0;
    QGridLayout *grid;

public slots:
    virtual void accept();
    void showAddIP();
    void addIP();
    void removeIP();
};


#endif // WORKSETTING_H
