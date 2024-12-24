#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    isWork = true;
    udpSocket = new QUdpSocket(this);
    timer = new QTimer(this);

    //  посылаем в broadcast
    timer->setInterval(1000);

    gRpcServer = new ServerBuilder();
    getLocalIp();

    connect(ui->startButton, &QPushButton::clicked,
            this, &MainWindow::clickedStartButton);

    connect(timer, &QTimer::timeout, this,
            &MainWindow::startUdpBroadcast);

    connect(gRpcServer, &ServerBuilder::stopUdpBroadcast,
            this, &MainWindow::stopTimer);

    connect(gRpcServer, &ServerBuilder::startUdpBroadcast,
            this, &MainWindow::startTimer);

    connect(gRpcServer, &ServerBuilder::logMessage,
            this, &MainWindow::outLog);
}

MainWindow::~MainWindow()
{
    isWork = false;
    timer->stop();

    startUdpBroadcast();

    delete ui;
}

bool MainWindow::getPort()
{
    QString portStr = ui->portLineEdit->text();
    if (portStr.isEmpty())
    {
        return false;
    }

    QRegularExpression rxp = QRegularExpression("[0-9]");
    if (portStr.contains(rxp))
    {
        port = portStr.toUInt();
        return true;
    }

    return false;
}

void MainWindow::getLocalIp()
{
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    QList<QString> addresses;

    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
             addresses.append(address.toString());
    }

    ip = (!addresses.size() ? "0.0.0.0" : addresses[0]);
}

void MainWindow::outLog(const QString& message)
{
    ui->logsTextEdit->append(message + "\t" + QTime::currentTime().toString());
}

void MainWindow::clickedStartButton(bool)
{
    if (!getPort())
    {
        QMessageBox::information(0, tr("Предупреждение"), tr("Неверное значение порта"));
        return;
    }

    ui->startButton->setDisabled(true);
    timer->start();
    startService();
}

void MainWindow::startUdpBroadcast()
{
    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);

    out << ip << ":" << port << isWork;
    udpSocket->writeDatagram(datagram, QHostAddress::Broadcast, updPort);

    QString logMessage = "udp broadcast: " +  ip + ":" + QString::number(port);

    outLog(logMessage);
}

void MainWindow::startService()
{
    gRpcServer->set(ip.toStdString(), port);
    ScopedThread t(std::thread(&ServerBuilder::startLoop, gRpcServer));
}
