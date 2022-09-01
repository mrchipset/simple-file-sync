#ifndef SYNCSERVICE_H
#define SYNCSERVICE_H

#include <QThread>
#include "SyncFilesystemWatcher.h"

class SyncService : public QThread
{
    Q_OBJECT
public:
    explicit SyncService(QObject *parent = nullptr);

    void setRootPath(const QString& path);
    QString rootPath() const;


private:
    SyncFilesystemWatcher* mFilesystemWatcher;
    QString mRootPath;
};

#endif // SYNCSERVICE_H
