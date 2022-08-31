#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QProgressDialog>

#include "SyncServiceFinder.h"
#include "SyncServiceProvider.h"
#include "SyncManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    SyncServiceProvider* mProvider;
    SyncServiceFinder* mFinder;
    SyncManager* mSyncManager;

    QProgressDialog* mFindingDlg;
    QVector<SyncServiceFinder::EndPoint> mEndPoints;
private Q_SLOTS:
    void onSearchFinished();

    void on_PB_CONNECT_clicked();
    void on_PB_REFRESH_clicked();

};
#endif // MAINWINDOW_H
