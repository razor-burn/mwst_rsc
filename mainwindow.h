#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QTimer>
#include "mainstackedwidget.h"
#include "usermgmt.h"
#include "rscmgmt.h"
#include "loanadd.h"
#include "loanmgmt.h"
#include "options.h"

namespace Ui {
class MainWindow;
}


#define MASK_MAIN 0
#define MASK_USERS 1
#define MASK_RESOURCES 2
#define MASK_ADDLOAN 3
#define MASK_MANAGELOANS 4
#define MASK_OPTIONS 5

/**
 * @brief Formularklasse für das Hauptfenster
 */

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init();



private slots:
    //void on_buttonManageUsers_clicked();
    //void on_buttonManageRessources_clicked();
    //void on_buttonNewLoan_clicked();
    //void on_buttonManageLoans_clicked();
    //void on_buttonAdminArea_clicked();

    void on_buttonEditLoan_clicked();
    void selectModels();
    void on_buttonEditRoomLoan_clicked();

    void on_actionBenutzer_verwalten_clicked();
    void on_actionHauptfenster_clicked();
    void on_actionRessourcen_verwalten_clicked();
    void on_actionNeue_Ausleihe_clicked();
    void on_actionAusleihen_verwalten_clicked();
    void on_actionOptionen_clicked();

private:
    /// Zeiger auf die Oberfläche
    Ui::MainWindow *ui;
    /// Fenster Benutzerverwaltung
    /*UserManagement * myUserManagement;
    /// Fenster Ressourcenverwaltung
    RscManagement * myRscManagement;
    /// Fenster Ausleihe hinzufügen
    AddLoan * myAddLoan;
    /// Fenster Ausleihen verwalten
    LoanManagement * myLoanManagement;*/
    /// Benutzerverwaltungs-Widget
    usermgmt * UserMgmt;

    /// Ressourcenverwaltungs-Widget
    rscmgmt * RscMgmt;

    /// Ausleihne hinzufügen-Widget
    loanadd * LoanAdd;

    /// Ausleihe editieren-Widget
    loanmgmt * LoanMgmt;

    /// Fenster Adminbereich
    options * Options;

    /// QSqlTableModel zum anzeigen aller Ausleihen, die ab dem jeweils heutigen Tag fällig sind
    QSqlTableModel * allDueLoansModel;

    /// QSqlTableModel zum anzeigen der Schnittraumreservierungen für den jeweils heutigen Tag
    QSqlTableModel * allEditingRoomsModel;

    /// Timer der verwendet wird um allDueLoansModel und allEditingRoomsModel alle 10 Sekunden zu aktualisieren
    QTimer * updateTimer;

signals:
    void buttonEditLoanPushed(QModelIndex id);
    void buttonEditRoomLoanPushed(QModelIndex id);


};

#endif // MAINWINDOW_H
