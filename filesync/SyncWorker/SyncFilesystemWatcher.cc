#include "SyncFilesystemWatcher.h"
#include <QFileInfo>
#include <QMutexLocker>
#include <QDir>

#include <QDebug>

static constexpr int EMIT_CHANGE_EXPIRETIME = 5000;

QString ExplainAction(DWORD dwAction)
{
    switch (dwAction)
    {
    case FILE_ACTION_ADDED:
        return "[Add]";
    case FILE_ACTION_REMOVED:
        return "[Delete]";
    case FILE_ACTION_MODIFIED:
        return "[Modified]";
    case FILE_ACTION_RENAMED_OLD_NAME:
        return "[Renamed From]";
    case FILE_ACTION_RENAMED_NEW_NAME:
        return "[Renamed To]";
    default:
        return "[BAD DATA]";
    }
}

SyncFilesystemWatcher::SyncFilesystemWatcher(QObject *parent)
    : QThread{parent},
      mReadDirectoryChanges(nullptr),
      mDispatcher(new SyncFilesystemWatcherDispatcher(this))
{

}

SyncFilesystemWatcher::~SyncFilesystemWatcher()
{
    requestInterruption();
    wait();
    if (mReadDirectoryChanges) {
        delete mReadDirectoryChanges;
    }
}

void SyncFilesystemWatcher::setWatchPath(const QString &path)
{
    mWatchPath = path;
    requestInterruption();
    wait();
    if (mReadDirectoryChanges) {
        delete mReadDirectoryChanges;
    }
    EIoModelType eIoModel = eIoModel_CompletionRoutine;
    DWORD dwMaxChanges = 4096;
    DWORD dwBufSize = 16 * 1024;
    const DWORD dwNotificationFlags = FILE_NOTIFY_CHANGE_LAST_WRITE
        | FILE_NOTIFY_CHANGE_CREATION
        | FILE_NOTIFY_CHANGE_FILE_NAME
        | FILE_NOTIFY_CHANGE_DIR_NAME;
    mReadDirectoryChanges = new CReadDirectoryChanges(eIoModel, dwMaxChanges);
    QFileInfo fileInfo(path);
    QString absPath = fileInfo.absoluteFilePath();
    CString tmpStr(absPath.toStdString().c_str());
    mReadDirectoryChanges->AddDirectory(tmpStr, true, dwNotificationFlags, dwBufSize);
    if (!isRunning()) {
        start();
    }
}

QString SyncFilesystemWatcher::watchPath() const
{
    return mWatchPath;
}

SyncFilesystemWatcherDispatcher *SyncFilesystemWatcher::dispatcher()
{
    return mDispatcher;
}

void SyncFilesystemWatcher::addTask(DWORD dwAction, const QString &path)
{
    SyncTask task;
    switch (dwAction)
    {
    case FILE_ACTION_ADDED:
        task = CreateNew;
        break;
    case FILE_ACTION_REMOVED:
        task = Delete;
        break;
    case FILE_ACTION_MODIFIED:
        task = Modify;
        break;
    default:
        return;
    }
    QFileInfo fileInfo(path);
    QDir d(mWatchPath);
    QString _dirPath = d.absolutePath();
    QString _filePath = fileInfo.absoluteFilePath();
    _filePath.remove(_dirPath);
    if (_filePath.startsWith('/')) {
        _filePath.remove(0, 1);
    }
    mDispatcher->addChangedFile(task, _filePath);
    qDebug() << _dirPath << _filePath;
}

void SyncFilesystemWatcher::run()
{
    DWORD WAIT_TIME_OUT = 100;
    const HANDLE handles[] = { mReadDirectoryChanges->GetWaitHandle() };
    while(!isInterruptionRequested()) {
        DWORD rc = ::WaitForMultipleObjectsEx(_countof(handles), handles, false, WAIT_TIME_OUT, true);
        switch (rc)
        {
//        case WAIT_OBJECT_0 + 0:
//            // hStdIn was signaled. This can happen due to mouse input, focus change,
//            // Shift keys, and more.  Delegate to TryGetKeyboardInput().
//            // TryGetKeyboardInput sets bTerminate to true if the user hits Esc.
//            if (TryGetKeyboardInput(hStdIn, bTerminate, buf, MAX_PATH))
//            {
//                changes.AddDirectory(CStringW(buf), false, dwNotificationFlags, dwBufSize);
//            }
//            break;
        case WAIT_OBJECT_0 + 0:
            // We've received a notification in the queue.
            {
                DWORD dwAction;
                CString strFilename;
                if (mReadDirectoryChanges->CheckOverflow())
                {
                    wprintf(L"Queue overflowed.\n");
                }
                else
                {
                    if (mReadDirectoryChanges->Pop(dwAction, strFilename)) {
                        addTask(dwAction, QString::fromWCharArray(strFilename.GetBuffer(), strFilename.GetLength()));
                        qDebug() << ExplainAction(dwAction) << QString::fromWCharArray(strFilename.GetBuffer(), strFilename.GetLength());
                    }
                }
            }
            break;
        case WAIT_IO_COMPLETION:
            // Nothing to do.
            break;
        }
    }
}

SyncFilesystemWatcherDispatcher::SyncFilesystemWatcherDispatcher(QObject *parent) : QObject(parent)
{
    mQueryTimer = new QTimer(this);
    mQueryTimer->setInterval(500);
    mQueryTimer->start();
    connect(mQueryTimer, &QTimer::timeout, this, &SyncFilesystemWatcherDispatcher::onQueryTimeout);
}

SyncFilesystemWatcherDispatcher::~SyncFilesystemWatcherDispatcher()
{

}

void SyncFilesystemWatcherDispatcher::addChangedFile(SyncTask task, const QString& path)
{
    QMutexLocker lk(&mTaskListMutex);
    if (!mTaskList.contains(path)) {
        TmpChangeObj* pObj = new TmpChangeObj;
        pObj->task = task;
        pObj->elpasedTimer.restart();
        pObj->path = path;
        mTaskList.insert(path, pObj);
    } else {
        if(mTaskList[path]->task < task) {
            mTaskList[path]->task = task;
            mTaskList[path]->elpasedTimer.restart();
        }
    }
}

bool SyncFilesystemWatcherDispatcher::pop(FileChangeObject &obj)
{
    QMutexLocker lk(&mQueueMutex);
    if (mFileChangeObjQueue.isEmpty()) {
        return false;
    } else {
        obj = mFileChangeObjQueue.dequeue();
        return true;
    }
}

void SyncFilesystemWatcherDispatcher::push(FileChangeObject &obj)
{
    QMutexLocker lk(&mQueueMutex);
    if (!mFileChangeObjQueue.isEmpty()) {
        if(mFileChangeObjQueue.last().path == obj.path) {
            mFileChangeObjQueue.last() = obj;
        } else {
            mFileChangeObjQueue.enqueue(obj);
        }
    } else {
        mFileChangeObjQueue.enqueue(obj);
    }
}

void SyncFilesystemWatcherDispatcher::onQueryTimeout()
{
    QMutexLocker lk(&mTaskListMutex);
    for (auto kv = mTaskList.keyValueBegin(); kv != mTaskList.keyValueEnd(); ++kv) {
        const QString key = kv->first;
        const auto& task = kv->second;
        if (task->elpasedTimer.hasExpired(EMIT_CHANGE_EXPIRETIME)) {
            FileChangeObject obj({task->task, task->path});
            TmpChangeObj* tmpObj = mTaskList.take(key);
            delete tmpObj;
            push(obj);
            Q_EMIT fileChanged();
            return;
        }
    }
}
