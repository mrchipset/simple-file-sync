#include "SyncWorker.h"

SyncWorker::SyncWorker()
    : QRunnable()
{
}

bool SyncWorker::requestInterruption()
{
    return mInterrupt.testAndSetOrdered(0, 1);
}

bool SyncWorker::wait(QDeadlineTimer deadline)
{
    QMutexLocker lk(&mMutex);
    if (!isRunning()) {
        return true;
    } else {
        return mWaitCond.wait(&mMutex, deadline);
    }
}

void SyncWorker::run()
{
    QMutexLocker lk(&mMutex);
    mRunning = true;
    worker();
    mWaitCond.wakeAll();
    mRunning = false;
}
