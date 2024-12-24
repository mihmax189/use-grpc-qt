#ifndef ACTIONDELEGATE_H
#define ACTIONDELEGATE_H

#include <QStyledItemDelegate>
#include <QPushButton>
#include <QAbstractItemView>
#include <QPainter>
#include <QApplication>

#include "tablemodel.h"

class ActionDelegate : public QStyledItemDelegate
{
    Q_OBJECT

    mutable bool isClicked = false;
    bool on = false;

public:
    ActionDelegate(QObject* parnet = nullptr);
    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

protected:
    bool editorEvent(QEvent* event, QAbstractItemModel* model,
                     const QStyleOptionViewItem &option, const QModelIndex& index) override;

signals:
    void buttonClicked(const QModelIndex& index, bool);
};

#endif // ACTIONDELEGATE_H
