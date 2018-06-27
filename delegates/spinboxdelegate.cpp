#include "delegates\spinboxdelegate.h"
#include "dbinterface\dbcon.h"
#include "dbinterface\resource.h"

#include <QAbstractItemModel>

#define USERMGMNT_MASK 4
#define DESIREDITEMS_MASK 10
#define FACULTY_MAX 4
#define FACULTY_MIN 1
#define RESOURCE_ID 7

/**
 * @brief Erstellt Editor in Tabellenzelle (in Form einer Spinbox) und setzt die Minimalen und Maximalen Werte abhängig davon in welcher Spalte wir uns befinden.
 * @param parent
 * @param option Wird nicht benötigt.
 * @param index QModelIndex der ausgewählten Tabellenzelle. Wird benötigt um die Spalte zu identifizieren und somit die Min und Max Werte zu setzen.
 * @return
 */
QWidget* SpinBoxDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &/*option*/,
                                       const QModelIndex &index) const
{

    QSpinBox *editor = new QSpinBox(parent);
    if(index.column() == USERMGMNT_MASK)
    {
        editor->setMinimum(FACULTY_MIN);
        editor->setMaximum(FACULTY_MAX);
    }
    else if (index.column() == DESIREDITEMS_MASK)
    {
        const QAbstractItemModel * model = index.model();
        QSqlQuery wantedResource = Resource().getBy("id", model->data(model->index(index.row(), RESOURCE_ID)).toInt());
        wantedResource.next();
        int remainingQuantity = wantedResource.value("quantity").toInt();
        if(remainingQuantity > 0)
        {
            editor->setMinimum(1);
            editor->setMaximum(remainingQuantity);
        }
    }
    return editor;
}

/**
 * @brief Setzt den Wert der Spinbox.
 * @param editor: Das QWidget Objekt das als Editor fungiert (in diesem Fall QSpinBox).
 * @param index: QModelIndex der ausgewählten Tabellenzelle. Wird benötigt um die einzutragenden Daten aus dem darunterliegenden model zu holen (bei uns QSqlTableModel).
 */
void SpinBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    int value = index.model()->data(index, Qt::EditRole).toInt();
    QSpinBox *spinbox = static_cast<QSpinBox*>(editor);
    spinbox->setValue(value);
}

/**
 * @brief Schreibt die Veränderungen zurück in das model.
 * Entsprechend dieser Veränderungen wird die Menge der jeweiligen Ressource aktualisiert.
 * @param editor: QSpinBox, aus der der neue Wert ausgelesen wird.
 * @param model: model, in das die Daten geschrieben werden.
 * @param index: QModelIndex der ausgewählten Tabellenzelle.
 */
void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QSpinBox *spinbox = static_cast<QSpinBox*>(editor);
    spinbox->interpretText();
    int value = spinbox->value();
    model->setData(index, value, Qt::EditRole);
}

/**
 * @brief Setzt Geometrie der SpinBox
 * @param editor Die SpinBox
 * @param option Paramter die beschreiben, wie ein Widget gerendert werden soll
 */
void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}

