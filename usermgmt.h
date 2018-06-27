#ifndef USERMGMT_H
#define USERMGMT_H

#include <QWidget>
#include <QSqlTableModel>
#include <QItemDelegate>
#include "delegates\spinboxdelegate.h"
#include "dbinterface\faculty.h"
#include "dbinterface\module.h"

namespace Ui {
class usermgmt;
}

class usermgmt : public QWidget
{
    Q_OBJECT

protected:

    void showEvent(QShowEvent * event);

public:

    explicit usermgmt(QWidget *parent = 0);
    ~usermgmt();
    void initView();
    void clearLineEdits();
    bool checkMailFormat(const QString & mail);
    bool checkTelFormat(const QString & tel);
    void populateComboBoxFaculties(void);


private slots:

    void on_lineEditUserSearch_textChanged(const QString & arg1);
    void on_buttonClear_clicked();
    void on_buttonAdd_clicked();
    void on_buttonDel_clicked();
    void on_model_dataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight);

    void on_comboBoxFaculties_2_currentIndexChanged(int index);

    void on_buttonAddLecturer_clicked();

    void on_lineEditSearchLecturers_textChanged(const QString &arg1);

    void on_buttonDelLecturer_clicked();

    void on_buttonAddModule_clicked();

    void on_lineEditSearchModules_textChanged(const QString &arg1);

    void on_buttonDelModule_clicked();

private:
    /// Zeiger auf die Oberfläche
    Ui::usermgmt *ui;
    /// QSqlTableModel, in dem die Benutzer abgebildet werden
    QSqlTableModel * m_model;
    /// Delegate, mit dem die Fachbereichsangabe kontrolliert wird
    SpinBoxDelegate * mySpinBoxDelegate;

    QSqlTableModel * m_lecturersModel;
    QSqlTableModel * m_modulesModel;

};

#endif // USERMGMT_H
