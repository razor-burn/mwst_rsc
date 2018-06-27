#ifndef USERMANAGEMENT_H
#define USERMANAGEMENT_H

#include <QDialog>
#include <QSqlTableModel>
#include <QItemDelegate>
#include "delegates\spinboxdelegate.h"

namespace Ui {
class UserManagement;
}

/**
 * @brief Formularklasse zum Verwalten von Benutzern
 */

class UserManagement : public QDialog
{
    Q_OBJECT

protected:

    void showEvent(QShowEvent * event);

public:

    explicit UserManagement(QWidget * parent = 0);
    void initView();
    void clearLineEdits();
    bool checkMailFormat(const QString & mail);
    bool checkTelFormat(const QString & tel);
    ~UserManagement();

private slots:

    void on_lineEditUserSearch_textChanged(const QString & arg1);
    void on_buttonClear_clicked();
    void on_buttonAdd_clicked();
    void on_buttonDel_clicked();
    void on_model_dataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight);

private:
    /// Zeiger auf die Oberfläche
    Ui::UserManagement * ui;
    /// QSqlTableModel, in dem die Benutzer abgebildet werden
    QSqlTableModel * m_model;
    /// Delegate, mit dem die Fachbereichsangabe kontrolliert wird
    SpinBoxDelegate * mySpinBoxDelegate;

};

#endif // USERMANAGEMENT_H
