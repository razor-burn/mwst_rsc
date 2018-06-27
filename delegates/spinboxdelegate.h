#ifndef SPINBOXDELEGATE_H
#define SPINBOXDELEGATE_H
#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QSpinBox>

/**
 * @brief Klasse zum Manipulieren und Kontrollieren von Mengen in gewünschten Ausleihen und der Fachbereichsangabe in der Benutzerverwaltung
 */

class SpinBoxDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor
     * @param parent
     */
    explicit SpinBoxDelegate(QObject * parent = 0) : QItemDelegate(parent) {}
    QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // SPINBOXDELEGATE_H
