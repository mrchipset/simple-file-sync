#include "SyncService.h"

SyncService::SyncService(QObject *parent)
    : QThread{parent}
{
    mFilesystemWatcher = new SyncFilesystemWatcher(this);
}

void SyncService::setRootPath(const QString &path)
{
    mRootPath = path;
    mFilesystemWatcher->setWatchPath(path);
}

QString SyncService::rootPath() const
{
    return mRootPath;
}
