#ifndef LOANMANAGEMENT_H
#define LOANMANAGEMENT_H

#include <QDialog>
#include <QItemSelectionModel>
#include <QSqlTableModel>
#include "delegates\loanpositiondelegate.h"


#define IS_ACTIVE_COL 7
#define POS_ID_COL 0
#define RESOURCE_ID_COL 2
#define POS_RET_QTY_COL 4
#define POS_QTY_COL 3
#define ENDTIME_COL 5

namespace Ui {
class LoanManagement;
}

/**
 * @brief Formularklasse zum Verwalten bestehender Ausleihen
 */

class LoanManagement : public QDialog
{
    Q_OBJECT

private:
    /// Zeiger auf die Oberfläche
    Ui::LoanManagement *ui;
    /// QSqlTableModel, in dem die bestehenden Raumreservierungen repräsentiert werden
    QSqlTableModel * m_roomsModel;
    /// QSqlTableModel, in dem die bestehenden Ausleihen repräsentiert werden
    QSqlTableModel * m_allLoansModel;
    /// QSqlTableModel, in dem die bestehenden Ausleihpositionen repräsentiert werden
    QSqlTableModel * m_allLoanPositionsModel;
    /// Delegate, mit dem Die Mengen (Verleihene Menge und Retourmenge) in Ausleihpositionen manipuliert und kontrolliert werden
    LoanPositionDelegate * m_delegate;
    /**
     * @brief Wird benötigt, wenn der Benutzer vom Hauptfenster aus eine Ressource editieren möchte. Hält die ID des Datensatzes der markiert werden soll. Gilt nur
     * für das Model, in dem die Ausleihen festgehalten werden.
     */
    int m_loansModelRow;
    /**
     * @brief Wird benötigt, wenn der Benutzer vom Hauptfenster aus eine Ressource editieren möchte. Hält die ID des Datensatzes der markiert werden soll. Gilt nur
     * für das Model, in dem die Raumreservierungen festgehalten werden.
     */
    int m_roomsModelRow;
    /// Hält den FilterStatus für allLoansmodel. wechselt zwischen active = 0 und active = 1
    QString m_activeFilterState;


protected:
    void showEvent(QShowEvent * event);

public:
    explicit LoanManagement(QWidget *parent = 0);
    ~LoanManagement();
    void init();
    int setInactive();
    void setloansModelRow(int info);
    int getloansModelRow() const;
    int getRoomsModelRow() const;
    void setRoomsModelRow(int value);
    QString getActiveFilterState() const;
    void setActiveFilterState(const QString &value);

private slots:
    void on_radioButtonEditingRooms_toggled(bool checked);
    void on_radioButtonResources_toggled(bool checked);
    void on_checkBox_toggled(bool checked);
    void on_tableViewAllLoans_clicked(const QModelIndex &index);
    void on_buttonReturnAllPositions_clicked();
    void on_buttonPrintPage_clicked();
    void on_tableViewRooms_clicked(const QModelIndex &index);
    void on_buttonSubmitChangesRoom_clicked();
    void on_buttonClearFilter_clicked();
    void on_buttonApplyFilter_clicked();
    void on_checkBoxSearchByDate_toggled(bool checked);
    void on_checkBoxDoneWith_clicked();
    void on_buttonPrintPdf_clicked();
    void on_lineEditSearchResource_textChanged(const QString &arg1);
    void on_lineEditSearchUser_textChanged(const QString &arg1);
    void on_pushButtonExtendToDateTime_clicked();
};





#endif // LOANMANAGEMENT_H
