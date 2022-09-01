#ifndef SYNCTRANSACTIONTASKCREATOR_H
#define SYNCTRANSACTIONTASKCREATOR_H

#include <QObject>
#include "SyncTransactionObject.h"

class SyncTransactionTaskCreator : public QObject
{
    Q_OBJECT
public:
    explicit SyncTransactionTaskCreator(QObject *parent = nullptr);

    void setRootPath(const QString& path);
    QString rootPath() const;

    bool createNewTask(const QString& path, SyncTransactionObject& taskObj);
    bool createModifyTask(const QString& path, SyncTransactionObject& taskObj);
    bool createDeleteTask(const QString& path, SyncTransactionObject& taskObj);

private:
    QString mRootPath;

private:
    bool readFile(const QString& path, SyncTransactionObject& taskObj);
};

#endif // SYNCTRANSACTIONTASKCREATOR_H
