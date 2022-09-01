QT -= gui
QT += core network

TEMPLATE = lib
DEFINES += SYNCWORKER_LIBRARY

CONFIG += c++17
DESTDIR     = $$OUT_PWD/../bin

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AvailablePortHelper.cc \
    ReadDirectoryChanges/ReadDirectoryChanges.cpp \
    ReadDirectoryChanges/ReadDirectoryRequest/CompletionRoutineRequest.cpp \
    ReadDirectoryChanges/ReadDirectoryRequest/IoCompletionPortReqeust.cpp \
    ReadDirectoryChanges/ReadDirectoryRequest/ReadDirectoryRequest.cpp \
    ReadDirectoryChanges/ReadDirectoryServer/CompletionRoutineServer.cpp \
    ReadDirectoryChanges/ReadDirectoryServer/IoCompletionPortServer.cpp \
    ReadDirectoryChanges/ReadDirectoryServer/ReadDirectoryServer.cpp \
    SyncFilesystemWatcher.cc \
    SyncManager.cc \
    SyncServerWorker.cc \
    SyncService.cc \
    SyncTcpServer.cc \
    SyncTransactionObject.cc \
    SyncTransactionTaskCreator.cc \
    SyncTransactionTaskWorker.cc \
    SyncWorker.cc

HEADERS += \
    AvailablePortHelper.h \
    ReadDirectoryChanges/ReadDirectoryChanges.h \
    ReadDirectoryChanges/ReadDirectoryRequest/CompletionRoutineRequest.h \
    ReadDirectoryChanges/ReadDirectoryRequest/IoCompletionPortReqeust.h \
    ReadDirectoryChanges/ReadDirectoryRequest/ReadDirectoryRequest.h \
    ReadDirectoryChanges/ReadDirectoryServer/CompletionRoutineServer.h \
    ReadDirectoryChanges/ReadDirectoryServer/IoCompletionPortServer.h \
    ReadDirectoryChanges/ReadDirectoryServer/ReadDirectoryServer.h \
    ReadDirectoryChanges/Util/IoCompletionPort.h \
    ReadDirectoryChanges/Util/ThreadSafeQueue.h \
    ReadDirectoryChanges/Util/auto_release.h \
    ReadDirectoryChanges/stdafx.h \
    SyncFilesystemWatcher.h \
    SyncManager.h \
    SyncServerWorker.h \
    SyncService.h \
    SyncTcpServer.h \
    SyncTransactionObject.h \
    SyncTransactionTaskCreator.h \
    SyncTransactionTaskWorker.h \
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
