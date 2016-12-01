#ifndef VLCWIDGETVIDEOEXT_H
#define VLCWIDGETVIDEOEXT_H

#include <QWidget>
#include <QEvent>
#include <VLCQtWidgets/WidgetVideo.h>

class VlcWidgetVideoExt : public VlcWidgetVideo
{
    Q_OBJECT
public:
    explicit VlcWidgetVideoExt(QWidget *parent = 0);

protected:
    bool event(QEvent *e);
signals:
    void doubleclicked();
public slots:
    void timedRequest();
};

#endif // VLCWIDGETVIDEOEXT_H
