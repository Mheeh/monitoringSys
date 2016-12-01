#include "vlcwidgetvideoext.h"


VlcWidgetVideoExt::VlcWidgetVideoExt(QWidget *parent) : VlcWidgetVideo(parent)
{

}

bool VlcWidgetVideoExt::event(QEvent *e)
{
    if(e->type()==QEvent::MouseButtonDblClick){
        emit doubleclicked();
        return true;
    }else{

    return VlcWidgetVideo::event(e);
    }
}

void VlcWidgetVideoExt::timedRequest()
{
    request();
}
