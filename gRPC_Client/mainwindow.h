#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QHash>

#include "tablemodel.h"
#include "maintainingapiservice.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:

    Ui::MainWindow *ui;

    TableModel* mModel;
    QUdpSocket* mSocket;
    const quint16 mUdpPort = 10001;

    QString mIp;
    uint mPort;
    bool mIsWork;

    std::unordered_map<uint, std::unique_ptr<Client>> clients;

private slots:
    void readDatagram();
    void buttonClicked(const QModelIndex&index, bool on)
    {
        clients.at(mModel->getKey(index))->Ping(on);
    }
};

#endif // MAINWINDOW_H
