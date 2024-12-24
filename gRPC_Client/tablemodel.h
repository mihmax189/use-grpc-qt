#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>

#include "grpcserver.h"
#include "actiondelegate.h"

class TableModel : public QAbstractTableModel
{
    Q_OBJECT

private:
    const QStringList headers = { "IP", "Last ping time", "Status", "Action" };
    const int mColumns = headers.size();
    const QMap<bool, QString> statuses =
        {{false, "Offline"}, {true, "Online"}};
    const QMap<bool, QString> actions =
        {{false, "Connection"}, {true, "Disconnect"}};

    // исопльзуем QHash для хранения ip адреса и порта
    QHash<uint, gRpcServerParams> gRpcServers;

public:
    enum Columns { Ip, LastPingTime, Status, Action };

    TableModel(QObject* parent = nullptr);
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                                    int role = Qt::DisplayRole) const override;

    void insert(uint key, const gRpcServerParams& server);
    void remove(uint key);
    bool find(uint key) const;

    QHash<uint, gRpcServerParams>::iterator getServerParams(uint key);

    uint getKey(const QModelIndex& index)
    {
        return gRpcServers.keys()[index.row()];
    }

    bool getStatus(const QModelIndex& index)
    {
        return gRpcServers.values()[index.row()].getStatus();
    }

public slots:
    void updateStatus(const QModelIndex& index, bool status)
    {
        int current = 0;
        for (auto it = gRpcServers.begin(); it != gRpcServers.end(); ++it, ++current)
        {
            if (current == index.row())
            {
                beginResetModel();
                (*it).setStatus(status);
                endResetModel();
            }
        }
    }
};

#endif // TABLEMODEL_H
