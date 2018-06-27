#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dbinterface\dbcon.h"
#include <QModelIndexList>
#include <QDebug>
#include <QActionGroup>
#include <QInputDialog>

/**
 * @brief Konstruktor. Initialisiert alle Member mit NULL. Ruft init() auf
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    UserMgmt(NULL),
    allDueLoansModel(NULL),
    allEditingRoomsModel(NULL),
    updateTimer(NULL)
{
    ui->setupUi(this);
    this->init();
    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(selectModels()));
    updateTimer->start(5000);
    connect(ui->actionBenutzer_verwalten, SIGNAL(triggered()), this, SLOT(on_actionBenutzer_verwalten_clicked()));
    connect(ui->actionHauptfenster, SIGNAL(triggered()), this, SLOT(on_actionHauptfenster_clicked()));
    connect(ui->actionRessourcen_verwalten, SIGNAL(triggered()), this, SLOT(on_actionRessourcen_verwalten_clicked()));
    connect(ui->actionNeue_Ausleihe, SIGNAL(triggered()), this, SLOT(on_actionNeue_Ausleihe_clicked()));
    connect(ui->actionAusleihen_verwalten, SIGNAL(triggered()), this, SLOT(on_actionAusleihen_verwalten_clicked()));
    connect(ui->actionOptionen, SIGNAL(triggered()), this, SLOT(on_actionOptionen_clicked()));

    UserMgmt = new usermgmt(this);
    RscMgmt = new rscmgmt(this);
    LoanAdd = new loanadd(this);
    LoanMgmt = new loanmgmt(this);
    Options = new options(this);

    ui->stackedWidget->addWidget(UserMgmt);
    ui->stackedWidget->addWidget(RscMgmt);
    ui->stackedWidget->addWidget(LoanAdd);
    ui->stackedWidget->addWidget(LoanMgmt);
    ui->stackedWidget->addWidget(Options);
    connect(this, SIGNAL(buttonEditLoanPushed(QModelIndex)), LoanMgmt, SLOT(on_buttonEditLoanPushed(QModelIndex)));
    connect(this, SIGNAL(buttonEditRoomLoanPushed(QModelIndex)), LoanMgmt, SLOT(on_buttonEditRoomLoanPushed(QModelIndex)));
    ui->stackedWidget->setCurrentIndex(MASK_MAIN);
}


/**
 * @brief Destruktor
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief Erstellt QSqlTableModels und befüllt die tableViews mit deren Daten
 */
void MainWindow::init()
{
    if(!dbcon::connecttodb())
        return;

    if(!allDueLoansModel)
        allDueLoansModel = new QSqlTableModel(this, QSqlDatabase::database());
    allDueLoansModel->setTable("allloans_view");
    allDueLoansModel->setFilter(" Ende <= NOW() AND isactive = 1 ORDER BY Ende");
    ui->tableViewLoansDueToday->setModel(allDueLoansModel);
    ui->tableViewLoansDueToday->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewLoansDueToday->hideColumn(0);
    ui->tableViewLoansDueToday->hideColumn(1);
    ui->tableViewLoansDueToday->hideColumn(6);
    ui->tableViewLoansDueToday->hideColumn(7);

    if(!allEditingRoomsModel)
        allEditingRoomsModel = new QSqlTableModel(this, QSqlDatabase::database());
    allEditingRoomsModel->setTable("allroomloans_view");
    allEditingRoomsModel->setFilter(" NOW() BETWEEN Start AND Ende AND Ende >= NOW() ORDER BY Ende");
    ui->tableViewEditingRoomsOccupance->setModel(allEditingRoomsModel);
    ui->tableViewEditingRoomsOccupance->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewEditingRoomsOccupance->hideColumn(0);
    ui->tableViewEditingRoomsOccupance->hideColumn(1);
    ui->tableViewEditingRoomsOccupance->hideColumn(7);
    ui->tableViewEditingRoomsOccupance->hideColumn(8);
    ui->tableViewEditingRoomsOccupance->hideColumn(9);
    allDueLoansModel->select();
    allEditingRoomsModel->select();
}


/**
 * @brief Dialog Ausleihenverwaltung öffnen und gewünschte Ressourcenreservierung auswählen
 */

void MainWindow::on_buttonEditLoan_clicked()
{
    if(!ui->tableViewLoansDueToday->selectionModel()->hasSelection())
        return;
    this->on_actionAusleihen_verwalten_clicked();
    emit buttonEditLoanPushed(ui->tableViewLoansDueToday->selectionModel()->currentIndex());
}

/**
 * @brief Dialog Ausleihenverwaltung öffnen und gewünschte Schnittraumreservierung auswählen
 */

void MainWindow::on_buttonEditRoomLoan_clicked()
{
    if(!ui->tableViewEditingRoomsOccupance->selectionModel()->hasSelection())
        return;
    this->on_actionAusleihen_verwalten_clicked();
    emit buttonEditRoomLoanPushed(ui->tableViewEditingRoomsOccupance->selectionModel()->currentIndex());
}

/**
 * @brief Models neu laden falls sich etwas geändert hat. Vorher den Index der ausgewählten Zeilen festhalten und diesen nachher wieder setzen
 */
void MainWindow::selectModels()
{
    if(!dbcon::connecttodb())
        return;

    int tmpIdLoansModel = -1;
    int tmpIdRoomsModel = -1;

    if(ui->tableViewLoansDueToday->selectionModel()->hasSelection())
        tmpIdLoansModel = ui->tableViewLoansDueToday->currentIndex().sibling(ui->tableViewLoansDueToday->currentIndex().row(),0).data().toInt();
    if(ui->tableViewEditingRoomsOccupance->selectionModel()->hasSelection())
        tmpIdRoomsModel = ui->tableViewEditingRoomsOccupance->currentIndex().sibling(ui->tableViewEditingRoomsOccupance->currentIndex().row(), 0).data().toInt();

    allDueLoansModel->select();
    allEditingRoomsModel->select();

    if(tmpIdLoansModel != -1)
    {
        QModelIndexList indexes = allDueLoansModel->match(allDueLoansModel->index(0,0), Qt::DisplayRole, tmpIdLoansModel, 1, Qt::MatchExactly);
        if (indexes.count() != 0)
            ui->tableViewLoansDueToday->setCurrentIndex(indexes[0]);
    }
    if(tmpIdRoomsModel != -1)
    {
        QModelIndexList indexes = allEditingRoomsModel->match(allEditingRoomsModel->index(0,0), Qt::DisplayRole, tmpIdRoomsModel, 1, Qt::MatchExactly);
        if (indexes.count() != 0)
            ui->tableViewEditingRoomsOccupance->setCurrentIndex(indexes[0]);
    }
}

void MainWindow::on_actionHauptfenster_clicked()
{
    ui->stackedWidget->setCurrentIndex(MASK_MAIN);
}

void MainWindow::on_actionBenutzer_verwalten_clicked()
{
    ui->stackedWidget->setCurrentIndex(MASK_USERS);
}

void MainWindow::on_actionRessourcen_verwalten_clicked()
{
    ui->stackedWidget->setCurrentIndex(MASK_RESOURCES);
}

void MainWindow::on_actionNeue_Ausleihe_clicked()
{
    ui->stackedWidget->setCurrentIndex(MASK_ADDLOAN);
}

void MainWindow::on_actionAusleihen_verwalten_clicked()
{
    ui->stackedWidget->setCurrentIndex(MASK_MANAGELOANS);
}

void MainWindow::on_actionOptionen_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Passwort", "", QLineEdit::Password,"", &ok);
    if(text != "password")
        return;
    ui->stackedWidget->setCurrentIndex(MASK_OPTIONS);
}






