#ifndef RAVENTREEDELEGATE_H
#define RAVENTREEDELEGATE_H

#include <QStyledItemDelegate>


class RavenTreeDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    RavenTreeDelegate();

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
};

#endif // RAVENTREEDELEGATE_H
