#ifndef LOANPOSITIONDELEGATE_H
#define LOANPOSITIONDELEGATE_H

#include <QItemDelegate>
#include <QModelIndex>
#include <QSize>
#include <QSpinBox>

/**
 * @brief Klasse zum Manipulieren und Kontrollieren von Mengen in Ausleihpositionen
 */

class LoanPositionDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit LoanPositionDelegate(QObject * parent = 0) : QItemDelegate(parent){}
    QWidget * createEditor (QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};



#endif // LOANPOSITIONDELEGATE_H
