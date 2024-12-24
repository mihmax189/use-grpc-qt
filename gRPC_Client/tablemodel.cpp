#include "tablemodel.h"

TableModel::TableModel(QObject* parent) : QAbstractTableModel(parent) {}

QVariant TableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    if (Qt::TextAlignmentRole == role)
    {
        return Qt::AlignCenter;
    }

    if (Qt::DisplayRole == role)
    {
        QList<gRpcServerParams> values = gRpcServers.values();
        int current = 0;
        for (auto it = values.begin(); it != values.end(); ++it, ++current)
        {
            if (current == index.row())
            {
                QVariant data;
                switch (index.column())
                {
                case Columns::Ip:
                    data = (*it).getIp() +
                            ":" +
                            QString::number((*it).getPort());
                    break;
                case Columns::LastPingTime:
                {
                    QTime t = (*it).getLastPingTime();
                    if (t == QTime())
                        data = " - ";
                    else
                        data = t;
                    break;
                }
                case Columns::Status:
                    data = statuses[(*it).getStatus()];
                    break;
                case Columns::Action:
                    data = actions[(*it).getStatus()];
                    break;
                }

                return data;
            }
        }
    }

    return QVariant();
}

int TableModel::rowCount(const QModelIndex& parent) const
{
    return gRpcServers.size();
}

int TableModel::columnCount(const QModelIndex& parent) const
{
    return mColumns;
}

Qt::ItemFlags TableModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    if (index.isValid() && index.column() == Columns::Action)
        return flags | Qt::ItemIsEditable;
    else
        return flags;
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
    if (role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if(orientation == Qt::Vertical)
    {
        return QVariant();
    }

    return (orientation == Qt::Horizontal) ? headers[section] : QVariant();
}

void TableModel::insert(uint key, const gRpcServerParams& server)
{
    beginResetModel();
    gRpcServers.insert(key, server);
    endResetModel();
}

void TableModel::remove(uint key)
{
    beginResetModel();
    gRpcServers.remove(key);
    endResetModel();
}

bool TableModel::find(uint key) const
{
    auto it = gRpcServers.find(key);
    return (it == gRpcServers.end() ? false : true);
}

QHash<uint, gRpcServerParams>::iterator TableModel::getServerParams(uint key)
{
    return gRpcServers.find(key);
}
