#include "SyncFilesystemWatcher.h"
#include <QFileInfo>
#include <QDebug>

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
      mReadDirectoryChanges(nullptr)
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

void SyncFilesystemWatcher::run()
{
    char buf[MAX_PATH] = { 0 };

    const HANDLE handles[] = { mReadDirectoryChanges->GetWaitHandle() };
    while(!isInterruptionRequested()) {
        DWORD rc = ::WaitForMultipleObjectsEx(_countof(handles), handles, false, INFINITE, true);
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
                    mReadDirectoryChanges->Pop(dwAction, strFilename);
//                    wprintf(L"%s %s\n", ExplainAction(dwAction), strFilename.GetBuffer());
                    qDebug() << ExplainAction(dwAction) << QString::fromWCharArray(strFilename.GetBuffer(), strFilename.GetLength());
                }
            }
            break;
        case WAIT_IO_COMPLETION:
            // Nothing to do.
            break;
        }
    }
}
