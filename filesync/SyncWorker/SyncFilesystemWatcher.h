#ifndef SYNCFILESYSTEMWATCHER_H
#define SYNCFILESYSTEMWATCHER_H

#include "SyncWorker_global.h"

#include <QObject>
#include <QThread>
#include "ReadDirectoryChanges/ReadDirectoryChanges.h"
using namespace rdc;

class SYNCWORKER_EXPORT SyncFilesystemWatcher : public QThread
{
    Q_OBJECT
public:
    explicit SyncFilesystemWatcher(QObject *parent = nullptr);
    ~SyncFilesystemWatcher();
    void setWatchPath(const QString& path);
    QString watchPath() const;

private:
    QString mWatchPath;
    CReadDirectoryChanges* mReadDirectoryChanges;
protected:
    virtual void run() override;
};

#endif // SYNCFILESYSTEMWATCHER_H
