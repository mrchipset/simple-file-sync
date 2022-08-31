#include "SyncerRunnable.h"
#include <QMutexLocker>

SyncerRunnable::SyncerRunnable() : QRunnable()
{
}

bool SyncerRunnable::requestInterruption()
{
    return mInterrupt.testAndSetOrdered(0, 1);
}

bool SyncerRunnable::wait(QDeadlineTimer deadline)
{
    QMutexLocker lk(&mMutex);
    if (!isRunning()) {
        return true;
    } else {
        return mWaitCond.wait(&mMutex, deadline);
    }
}

void SyncerRunnable::run()
{
    QMutexLocker lk(&mMutex);
    mRunning = true;
    worker();
    mWaitCond.wakeAll();
    mRunning = false;
}
