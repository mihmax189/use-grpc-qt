#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>
#include <QRegularExpression>
#include <QMessageBox>
#include <QHostInfo>
#include <QTime>
#include <QNetworkInterface>

#include <thread>

#include "maintainingapiservice.h"

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

    ServerBuilder *gRpcServer;
    // udp socket - send broadcast port 10001
    QUdpSocket* udpSocket;
    // таймер, запускается, когда нажата кнопка Start
    QTimer* timer;

    const quint16 updPort = 10001;
    QString ip;
    uint port;
    bool isWork;

    bool getPort();
    void getLocalIp();
    void startService();

private slots:
    void clickedStartButton(bool);
    void startUdpBroadcast();
    void outLog(const QString&);

    void stopTimer()
    {
        timer->stop();
    }

    void startTimer()
    {
        timer->start();
    }
};

#endif // MAINWINDOW_H
