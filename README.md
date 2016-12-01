# monitoringSys
System for monitoring students' workstations.

Server
------------

Role of server is to control network settings and broadcast of worksations.


- Compiled using https://vlc-qt.tano.si/ qt-vlc library in version 1.1.0


Client
----------

Program for students workstations. 
It was designed with 2 Network adapters in mind.
One connected to normal network, and other connected directly to Mitsubishi PLC.
That's why program do create portproxy on specific address.


Program heavly use netsh command line tool for windows and ffmpeg.
ffmpeg should be unpacked to application path, if not it can be configured in registry.


Projects in this directory has 3 different roles but binary files should all be copied to 1 directory after compiling.
- WatcherInstaller : program for adding starter file to windows autostart.
- starter : very simple program keeping Watcher alive in case of unexpected error.
- Watcher : client program.