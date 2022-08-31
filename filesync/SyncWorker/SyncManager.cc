#include "SyncManager.h"
#include <QDebug>
#include <QFileInfo>
#include <QDir>

SyncManager::SyncManager(QObject *parent)
    : QObject{parent}
{
    mWatcher = new QFileSystemWatcher(this);
    connect(mWatcher, &QFileSystemWatcher::directoryChanged, this, &SyncManager::onDirectoryChanged);
    connect(mWatcher, &QFileSystemWatcher::fileChanged, this, &SyncManager::onFileChanged);
}

void SyncManager::setWatchDirPath(const QString &path)
{
    mWatchDirPath = path;
    // TODO add paths recursively
    QFileInfo info(mWatchDirPath);
    addDirToWatcher(info.absoluteFilePath(), mWatcher);
    qDebug() << mWatcher->files() << mWatcher->directories();
}

QString SyncManager::waitchDirPath() const
{
    return mWatchDirPath;
}

void SyncManager::addFileToWatcher(const QString &filePath, QFileSystemWatcher *watcher)
{
    if (!watcher->files().contains(filePath)) {
        watcher->addPath(filePath);
    }
}

void SyncManager::addDirToWatcher(const QString &dirPath, QFileSystemWatcher *watcher)
{
    if (!watcher->directories().contains(dirPath)) {
        watcher->addPath(dirPath);
    }

    QDir d(dirPath);
    for (auto& info : d.entryInfoList()) {
        if (info.isDir()) {
            addDirToWatcher(info.absoluteFilePath(), watcher);
        } else {
            if (info.isFile()) {
                addFileToWatcher(info.absoluteFilePath(), watcher);
            }
        }
    }
}

void SyncManager::onDirectoryChanged(const QString &path)
{
    qDebug() << "Dir Changed:" << path;
}

void SyncManager::onFileChanged(const QString &path)
{
    qDebug() << "File Changed:" << path;
}

