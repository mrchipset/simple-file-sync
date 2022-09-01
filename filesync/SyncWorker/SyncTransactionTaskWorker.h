#ifndef SYNCTRANSACTIONTASKWORKER_H
#define SYNCTRANSACTIONTASKWORKER_H

#include <QQueue>
#include <QThread>
#include <QMutex>

#include "SyncTransactionObject.h"


class SyncTransactionTaskWorker : public QThread
{
    Q_OBJECT
public:
    static SyncTransactionTaskWorker* GetInstance();
    void setRootPath(const QString& path);
    QString rootPath() const;

    bool doTaskWork(const SyncTransactionObject& taskObject);
private:
    static SyncTransactionTaskWorker* Instance;
    QString mRootPath;
    QQueue<SyncTransactionObject> mTaskObjs;
    QMutex mMutex;

private:
    SyncTransactionTaskWorker(QObject* parent=nullptr);

    bool dispatchTask(const SyncTransactionObject& taskObject);
    bool createNewTask(const SyncTransactionObject& taskObject);
    bool modifyTask(const SyncTransactionObject& taskObject);
    bool deleteTask(const SyncTransactionObject& taskObject);
    bool checkExisted(const QString& path);

protected:
    virtual void run() override;
};

#endif // SYNCTRANSACTIONTASKWORKER_H
