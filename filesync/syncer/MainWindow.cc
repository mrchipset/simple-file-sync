#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QDebug>
#include <QThreadPool>
#include <QStringList>

#include <QMessageBox>

#include "SyncFilesystemWatcher.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mProvider = new SyncServiceProvider(this);
    mProvider->createService();
    mFinder = new SyncServiceFinder(this);
    QThreadPool::globalInstance()->start(mFinder);
    connect(mFinder, &SyncServiceFinder::searchFinished, this, &MainWindow::onSearchFinished);

    mFindingDlg = new QProgressDialog(this);
    mFindingDlg->setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    mFindingDlg->setCancelButton(nullptr);
    mFindingDlg->setLabelText(tr("Finding syncing service..."));
    mFindingDlg->setRange(0, 0);
    mFindingDlg->show();

    mSyncService = new SyncService(this);
    mSyncService->setRootPath("D:/tmp");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onSearchFinished()
{
    ui->CB_DEST_SERVER->clear();
    mFindingDlg->close();
    if (!mFinder->isFind()) {
        QMessageBox::information(this, tr("Error"), tr("Cound not found syncing service in local network."));
    } else {
        mEndPoints = mFinder->getFoundEndPoints();
        QStringList strHosts;
        for(auto& endpoint : mEndPoints) {
            qDebug() << endpoint.Host.protocol() << (endpoint.Host != QHostAddress::LocalHost);
            strHosts << QString("%1:%2").arg(endpoint.Host.toString()).arg(endpoint.Port);
        }
        ui->CB_DEST_SERVER->addItems(strHosts);
    }
}

void MainWindow::on_PB_CONNECT_clicked()
{
    // TODO create a TCP socket;
}


void MainWindow::on_PB_REFRESH_clicked()
{
    ui->PB_REFRESH->setEnabled(false);
    mFindingDlg->show();
    if (!mFinder->isRunning()) {
        QThreadPool::globalInstance()->start(mFinder);
    }

    QTimer::singleShot(3000, [&](){
        ui->PB_REFRESH->setEnabled(true);
    });
}

