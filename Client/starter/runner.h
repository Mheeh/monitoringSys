#ifndef RUNNER_H
#define RUNNER_H

#include <QObject>
#include <QProcess>

class Runner : public QObject
{
    Q_OBJECT
public:
    explicit Runner(QObject *parent = 0);

    Runner(QString path,QObject *parent = 0);
    QProcess p;
    QString watcherPath;
public slots:
void restartWatcher(int i);
};

#endif // RUNNER_H
