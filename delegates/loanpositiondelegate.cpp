#include "delegates\loanpositiondelegate.h"
#include "dbinterface\dbcon.h"
#include "dbinterface\loan.h"
#include "dbinterface\resource.h"

#include <QAbstractItemModel>

#define LOAN_POSITION_RETOUR_AMOUNT_COLUMN 3
#define LOAN_POSITION_AMOUNT_COLUMN 2
#define LOAN_POSITION_RESSOURCE_ID_COLUMN 6
#define LOAN_POSITION_POS_ID_COLUMN 0

/**
 * @brief Erstellt Editor in Tabellenzelle (in Form einer Spinbox) und setzt die Minimalen und Maximalen Werte abhängig davon in welcher Spalte wir uns befinden
 * @param parent. Wird nicht benötigt
 * @param option. Wird nicht benötigt
 * @param index. QModelIndex der ausgewählten Tabellenzelle. Wird benötigt um die Spalte zu identifizieren und somit die Min und Max Werte zu setzen
 * @return
 */
QWidget* LoanPositionDelegate::createEditor(QWidget *parent,
                                  const QStyleOptionViewItem &/*option*/,
                                  const QModelIndex &index) const
{
    if(index.column() != LOAN_POSITION_RETOUR_AMOUNT_COLUMN && index.column() != LOAN_POSITION_AMOUNT_COLUMN)
        return NULL;

    QSpinBox *editor = new QSpinBox(parent);
    dbcon::connecttodb();
    if(index.column() == LOAN_POSITION_RETOUR_AMOUNT_COLUMN)
    {
        editor->setMinimum(0);
        editor->setMaximum(index.sibling(index.row(),LOAN_POSITION_AMOUNT_COLUMN).data().toInt());
    }
    else if (index.column() == LOAN_POSITION_AMOUNT_COLUMN)
    {
        QSqlQuery wantedResource = Resource().getBy("id", index.sibling(index.row(), LOAN_POSITION_RESSOURCE_ID_COLUMN)
                                               .data().toInt());
        wantedResource.next();
        int remainingQuantity = wantedResource.value("quantity").toInt();
        if(remainingQuantity  > 0)
        {
            editor->setMinimum(1);
            editor->setMaximum(remainingQuantity + index.sibling(index.row(),LOAN_POSITION_AMOUNT_COLUMN).data().toInt());
        }
    }
    return editor;
}

 /**
  * @brief Setzt den Wert der Spinbox
  * @param editor: Das QWidget Objekt das als Editor fungiert (in diesem Fall QSpinBox)
  * @param index: QModelIndex der ausgewählten Tabellenzelle. Wird benötigt um die einzutragenden Daten aus dem darunterliegenden model zu holen (bei uns QSqlTableModel)
  */
void LoanPositionDelegate::setEditorData(QWidget *editor,
                               const QModelIndex &index) const
{
    if(!dbcon::connecttodb())
        return;

    int value = index.model()->data(index, Qt::EditRole).toInt();
    QSpinBox *spinbox = static_cast<QSpinBox*>(editor);
    spinbox->setValue(value);
}


/**
 * @brief Schreibt die Veränderungen der Mengen und Retourmengen zurück in das model allLoanpositions.
 * Entsprechend dieser Veränderungen wird die Menge der jeweiligen Ressource aktualisiert.
 * @param editor QSpinBox, aus der der neue Wert ausgelesen wird
 * @param model model, in das die Daten geschrieben werden
 * @param index QModelIndex der ausgewählten Tabellenzelle. Wird benötigt um die Spalte zu identifizieren um entsprechend die Gesamtmenge der Ressource höher oder niedriger zu setzen
 */
void LoanPositionDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                              const QModelIndex &index) const
{
    if(!dbcon::connecttodb())
        return;

    QSpinBox *spinbox = static_cast<QSpinBox*>(editor);
    spinbox->interpretText();
    int value = spinbox->value();

    if(index.data().toInt() == value)
        return;

    Resource::updateQuantity(index.model()->index(index.row(), LOAN_POSITION_RESSOURCE_ID_COLUMN).data().toInt(),
                             index.data().toInt(), value, index.column(), Resource::ReturnOrIncrease);


    model->setData(index, value, Qt::EditRole);
}

/**
 * @brief Setzt Geometrie der SpinBox
 * @param editor Die SpinBox
 * @param option Paramter die beschreiben, wie ein Widget gerendert werden soll
 */
void LoanPositionDelegate::updateEditorGeometry(QWidget *editor,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}

