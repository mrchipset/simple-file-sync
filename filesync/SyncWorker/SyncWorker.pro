QT -= gui
QT += network

TEMPLATE = lib
DEFINES += SYNCWORKER_LIBRARY

CONFIG += c++17
DESTDIR     = $$OUT_PWD/../bin

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AvailablePortHelper.cc \
    SyncWorker.cc

HEADERS += \
    AvailablePortHelper.h \
    SyncWorker_global.h \
    SyncWorker.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

win32 {
    LIBS += -lWS2_32 -lIPHlpApi
}
