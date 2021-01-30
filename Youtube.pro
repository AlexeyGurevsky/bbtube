APP_NAME = Youtube

CONFIG += qt warn_on cascades10
LIBS += -lbb
LIBS += -lbbdata
LIBS += -lbbmultimedia
LIBS += -lbbsystem
LIBS += -lscreen
LIBS += -laudio_manager
QT += network

include(config.pri)
