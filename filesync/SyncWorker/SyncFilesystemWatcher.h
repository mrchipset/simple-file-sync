#ifndef SYNCFILESYSTEMWATCHER_H
#define SYNCFILESYSTEMWATCHER_H

#include "SyncWorker_global.h"

#include <QObject>
#include <QThread>
#include <QMap>
#include <QQueue>
#include <QMutex>
#include <QTimer>

#include "ReadDirectoryChanges/ReadDirectoryChanges.h"
#include "SyncTransactionObject.h"

using namespace rdc;

struct FileChangeObject {
    SyncTask task;
    QString path;
    QString rootPath;
};

class SyncFilesystemWatcherDispatcher;
class SYNCWORKER_EXPORT SyncFilesystemWatcher : public QThread
{
    Q_OBJECT
public:
    explicit SyncFilesystemWatcher(QObject *parent = nullptr);
    ~SyncFilesystemWatcher();
    void setWatchPath(const QString& path);
    QString watchPath() const;

    SyncFilesystemWatcherDispatcher* dispatcher();
private:
    QString mWatchPath;
    CReadDirectoryChanges* mReadDirectoryChanges;
    SyncFilesystemWatcherDispatcher* mDispatcher;

    void addTask(DWORD dwAction, const QString& path);
protected:
    virtual void run() override;
};

class SYNCWORKER_EXPORT SyncFilesystemWatcherDispatcher : public QObject
{
    Q_OBJECT
public:
    explicit SyncFilesystemWatcherDispatcher(QObject* parent=nullptr);
    ~SyncFilesystemWatcherDispatcher();

    void addChangedFile(SyncTask task, const QString& path);
    bool pop(FileChangeObject& obj);
Q_SIGNALS:
    void fileChanged();
private:
    struct TmpChangeObj {
        QString path;
        SyncTask task;
        QElapsedTimer elpasedTimer;
    };

    QMap<QString, TmpChangeObj*> mTaskList;
    QQueue<FileChangeObject> mFileChangeObjQueue;
    QMutex mQueueMutex;
    QMutex mTaskListMutex;
    QTimer* mQueryTimer;
    void push(FileChangeObject& obj);
private Q_SLOTS:
    void onQueryTimeout();
};

#endif // SYNCFILESYSTEMWATCHER_H
