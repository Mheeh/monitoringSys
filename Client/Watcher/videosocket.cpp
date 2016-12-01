#include "videosocket.h"
#include <vlc/vlc.h>
#include <QThread>

VideoSocket::VideoSocket(QObject *parent) : QObject(parent)
{
    libvlc_instance_t * inst;
     libvlc_media_player_t *mp;
     libvlc_media_t *m;

     /* Load the VLC engine */
     inst = libvlc_new (0, NULL);

     /* Create a new item */
     m = libvlc_media_new_location (inst, "http://www.steppublishers.com/sites/default/files/stepteen1.mov");
     //m = libvlc_media_new_path (inst, "/path/to/test.mov");

     /* Create a media player playing environement */
     mp = libvlc_media_player_new_from_media (m);

     /* No need to keep the media now */
     libvlc_media_release (m);

 #if 0
     /* This is a non working code that show how to hooks into a window,
      * if we have a window around */
      libvlc_media_player_set_xwindow (mp, xid);
     /* or on windows */
      libvlc_media_player_set_hwnd (mp, hwnd);
     /* or on mac os */
      libvlc_media_player_set_nsobject (mp, view);
  #endif

     /* play the media_player */
     libvlc_media_player_play (mp);

     QThread::sleep(10);

     /* Stop playing */
     libvlc_media_player_stop (mp);

     /* Free the media_player */
     libvlc_media_player_release (mp);

     libvlc_release (inst);

    //connect(socket,SIGNAL())
}

void VideoSocket::captureScreen(){


}
