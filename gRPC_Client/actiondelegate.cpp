#include "actiondelegate.h"

ActionDelegate::ActionDelegate(QObject* parnet) : QStyledItemDelegate(parnet) {}

void ActionDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const
{
    QStyleOptionButton button;
    button.rect = option.rect;
    button.text = index.model()->data(index, Qt::DisplayRole).toString();

    QStyle::State state = QStyle::State_Selected | QStyle::State_Enabled;

    if (isClicked) {
        button.state = state | QStyle::State_Sunken;
        isClicked = false;
    }
    else
        button.state = state;

    QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
}

bool ActionDelegate::editorEvent(QEvent* event, QAbstractItemModel* model,
                                 const QStyleOptionViewItem& option, const QModelIndex& index)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        auto m = qobject_cast<TableModel*>(model);
        bool on = m->getStatus(index) ? false : true;

        m->updateStatus(index, on);

        emit buttonClicked(index, on);
        isClicked = true;
    }

    return true;
}
