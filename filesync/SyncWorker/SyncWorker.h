#ifndef SYNCWORKER_H
#define SYNCWORKER_H

#include "SyncWorker_global.h"
#include <QRunnable>
#include <QAtomicInt>
#include <QMutex>
#include <QWaitCondition>

class SYNCWORKER_EXPORT SyncWorker : public QRunnable
{
public:
    SyncWorker();

    bool requestInterruption();
    inline bool isInterruptionRequested() const { return static_cast<int>(mInterrupt); }
    bool wait(QDeadlineTimer deadline=QDeadlineTimer(QDeadlineTimer::Forever));

    inline bool isRunning() { return mRunning; }
    virtual void run() override final;
private:
    QAtomicInt mInterrupt;
    volatile bool mRunning;
    QMutex mMutex;
    QWaitCondition mWaitCond;
protected:
    virtual void worker() = 0;
};




#endif // SYNCWORKER_H
