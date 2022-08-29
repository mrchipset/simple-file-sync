#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QDebug>
#include "AvailablePortHelper.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QVector<uint16_t> ports;
    AvailablePortHelper::GetAvailableTcpPorts(50000, 60000, ports);
    qDebug() << ports.size();
//    qDebug() << ports;

    mSocket = new QUdpSocket(this);

    mTimer.setInterval(1000);
    mTimer.start();
    connect(&mTimer, &QTimer::timeout, this, &MainWindow::onTimeout);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onTimeout()
{
    QString header = "FIND-HEADE-MAGIC-1234";
    uint16_t port = 50000;
    QNetworkDatagram datagram(header.toLocal8Bit(), QHostAddress::Broadcast, port);
    mSocket->writeDatagram(datagram);
    mSocket->waitForBytesWritten();
}

