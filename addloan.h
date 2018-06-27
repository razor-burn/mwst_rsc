#ifndef ADDLOAN_H
#define ADDLOAN_H

#include <QDialog>
#include <QSqlTableModel>
#include <QStandardItemModel>
#include <QItemDelegate>
#include "mytablewidget.h"

namespace Ui {
class AddLoan;
}

/**
 * @brief Formularklasse zum hinzufügen neuer Ausleihen für Schnitträume und für Ressourcen
 */

class AddLoan : public QDialog
{
    Q_OBJECT

protected:

    void showEvent(QShowEvent * event);

public:

    explicit AddLoan(QWidget *parent = 0);
    ~AddLoan();
    void init();

private slots:

    void on_radioButtonRooms_clicked();
    void on_radioButtonResources_clicked();
    void on_lineEditUserSearch_textChanged(const QString & arg1);
    void on_buttonClearUserSearch_clicked();
    void on_buttonSubmitLoanEditingroom_clicked();
    void on_lineEditSearchText_textChanged(const QString &arg1);
    void on_buttonAdd_clicked();
    void on_buttonDelLoanPosition_clicked();
    void on_buttonDelAllLoanPositions_clicked();
    void on_buttonSubmitLoanResources_clicked();
    void on_tableWidgetDesiredItems_itemChanged(QTableWidgetItem *item);
    void on_tableViewResources_doubleClicked(const QModelIndex &);

    void on_buttonClearSearchText_clicked();

private:
    /// Zeiger auf die Oberfläche
    Ui::AddLoan *ui;
    /// QSqlTableModel, in dem die Benutzer abgebildet werden
    QSqlTableModel * m_usersModel;
    /// QSqlTableModel, in dem die Schnitträume abgebildet werden
    QSqlTableModel * m_editingroomsModel;
    /// QSqlTableModel, in dem die Ressourcen abgebildet werden
    QSqlTableModel * m_resourcesModel;
    /// QSqlTableModel, in dem die Ressourcen abgebildet werden
    SpinBoxDelegate * m_mySpinBox;

};

#endif // ADDLOAN_H
