#include "MainWindow.h"

#include <QApplication>
#include <QLockFile>
#include <QDir>

#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString path = QDir::temp().absoluteFilePath("SyncerSingleLock.lock");
    QLockFile lockFile(path);
    if (lockFile.tryLock(100)) {
        MainWindow w;
        w.show();
        return a.exec();
    } else {
       QMessageBox::information(nullptr, QObject::tr("Syncer"), QObject::tr("A Syncer instance is already running."));
       QTimer::singleShot(50, &a, &QApplication::quit);
       return a.exec();
    }
}
