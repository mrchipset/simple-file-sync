#ifndef SYNCMANAGER_H
#define SYNCMANAGER_H

#include "SyncWorker_global.h"

#include <QObject>
#include <QFileSystemWatcher>

class SYNCWORKER_EXPORT SyncManager : public QObject
{
    Q_OBJECT
public:
    SyncManager(QObject* parent=nullptr);

    void setWatchDirPath(const QString& path);
    QString waitchDirPath() const;

private:
    QString mWatchDirPath;

    QFileSystemWatcher* mWatcher;

    void addFileToWatcher(const QString& filePath, QFileSystemWatcher* watcher);
    void addDirToWatcher(const QString& dirPath, QFileSystemWatcher* watcher);
private Q_SLOTS:
    void onDirectoryChanged(const QString& path);
    void onFileChanged(const QString& path);
};

#endif // SYNCMANAGER_H
