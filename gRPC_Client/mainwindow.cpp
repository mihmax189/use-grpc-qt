#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    mModel = new TableModel(this);

    ui->setupUi(this);
    ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setModel(mModel);

    ActionDelegate* delegate = new ActionDelegate(ui->tableView);
    ui->tableView->setItemDelegateForColumn(TableModel::Columns::Action, delegate);


    mSocket = new QUdpSocket(this);
    mSocket->bind(mUdpPort,  QUdpSocket::ShareAddress);

    connect(mSocket, &QUdpSocket::readyRead, this, &MainWindow::readDatagram);
    connect(delegate, SIGNAL(buttonClicked(const QModelIndex&, bool)),
            this, SLOT(buttonClicked(const QModelIndex&, bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readDatagram()
{
    QByteArray datagram;

    do
    {
        datagram.resize(mSocket->pendingDatagramSize());
        mSocket->readDatagram(datagram.data(), datagram.size());
    } while (mSocket->hasPendingDatagrams());

    QDataStream in(&datagram, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_3);

    QString sep;
    in >> mIp >> sep >> mPort >> mIsWork;

    gRpcServerParams server(mIp, mPort);
    uint key = qHash(server);

    if (mIsWork)
    {
        if (!mModel->find(key))
        {
            mModel->insert(key, server);

            Client* tmp = new Client(mIp.toStdString(), mPort);
            clients.insert(std::make_pair(key, tmp));
        }
    }
    else
    {
        clients.erase(key);
        mModel->remove(key);
    }
}
