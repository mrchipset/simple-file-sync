#ifndef SYNCERRUNNABLE_H
#define SYNCERRUNNABLE_H

#include <QRunnable>
#include <QAtomicInt>
#include <QMutex>
#include <QWaitCondition>

class SyncerRunnable : public QRunnable
{
public:
    SyncerRunnable();

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

#endif // SYNCERRUNNABLE_H
