#include "raventreedelegate.h"
#include "raventreeitem.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

RavenTreeDelegate::RavenTreeDelegate() {}

void RavenTreeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    // Call base class' paint method to render default items
    QStyledItemDelegate::paint(painter, option, index);

    // Apply our changes on top of the default UI
    if (index.isValid())
    {
        auto *treeItem = static_cast<RavenTreeItem*>(index.internalPointer());

        // Applicable to non-heading items only
        if (treeItem->heading)
        {
            painter->restore();
            return;
        }

        auto isItemSelected = (option.state & QStyle::State_Selected);
        auto statusRect = option.rect;

        // Increase yOffset for rect to show custom UI items.
        statusRect.setY(statusRect.y() + 18);

        // Status text (shows D/M/U text at end of row)
        auto statusPoint = statusRect.topRight();
        statusPoint.setX(statusPoint.x() - 20);
        auto font = painter->font();
        font.setWeight(QFont::Weight::Bold);

        if (treeItem->deleted)
        {
            // Show "Deleted" status
            painter->setFont(font);
            painter->setPen(isItemSelected ? option.palette.text().color() : Qt::red);
            painter->drawText(statusPoint, "D");
        }
        else if (treeItem->modified())
        {
            // Show "Uncommitted" status
            painter->setFont(font);
            painter->setPen(isItemSelected ? option.palette.text().color() : Qt::magenta);
            painter->drawText(statusPoint, "M");
        }
        else
        {
            // Show "Uncommitted" status
            painter->setFont(font);
            painter->setPen(isItemSelected ? option.palette.text().color() : Qt::green);
            painter->drawText(statusPoint, "U");
        }

        // Show button to + or - from Changes<->Staging Area

        QRect buttonRect = option.rect;
        buttonRect.setX(option.rect.topRight().x() - 60);
        buttonRect.setWidth(24);
        buttonRect.setHeight(24);

        // Create button widget here
        QStyleOptionButton buttonOption;
        buttonOption.rect = buttonRect;
        buttonOption.state = option.state;
        buttonOption.features = QStyleOptionButton::Flat;
        buttonOption.palette = option.palette;

        QStyle *style = QApplication::style();
        
        // WORKAROUND: Hide the `border-bottom` for `buttonOption` when treeview row is selected
        QBrush buttonOptionBrush;
        buttonOptionBrush.setColor(Qt::GlobalColor::transparent);
        buttonOption.palette.setBrush(QPalette::ColorRole::HighlightedText, buttonOptionBrush);

        if (treeItem->initiator == RavenTreeItem::STAGING)
        {
            // Show - icon
            buttonOption.text="-";
            style->drawControl(QStyle::CE_PushButton, &buttonOption, painter);
        }
        else
        {
            // Show + icon
            buttonOption.text="+";
            style->drawControl(QStyle::CE_PushButton, &buttonOption, painter);
        }
    }
    painter->restore();
}

QSize RavenTreeDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}
