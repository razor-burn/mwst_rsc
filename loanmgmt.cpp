#include "loanmgmt.h"
#include "ui_loanmgmt.h"


#include "dbinterface\dbcon.h"
#include "pdfgenerator.h"
#include "dbinterface\location.h"
#include "dbinterface\loan.h"
#include "dbinterface\resource.h"
#include <QMessageBox>
#include <QSqlRecord>
#include <QSqlField>

#include <QSqlError>



/**
 * @brief Gibt den Filterstatus für allLoansmodel zurück
 * @return Filter als QString
 */
QString loanmgmt::getActiveFilterState() const
{
    return this->m_activeFilterState;
}

/**
 * @brief Setzt den Filterstatus für allLoansmdel
 * @param Der neue Filterstauts (entweder active = 1 oder active = 0)
 */
void loanmgmt::setActiveFilterState(const QString &value)
{
    m_activeFilterState = value;
}

/**
 * @brief Überladen von QDialog::showevent.
 * Wird jedes Mal aufgerufen, wenn der Dialog anzeigt wird.
 * Setzt alle input Widgets zurück und ruft init() auf.
 * @param event
 */
void loanmgmt::showEvent(QShowEvent *event)
{
    ui->tableViewAllLoanPositions->clearSelection();
    QWidget::showEvent(event);
}

/**
 * @brief Konstruktor. Initialisiert alle Member von loanmgmt
 * @param parent
 */
loanmgmt::loanmgmt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::loanmgmt),
    m_roomsModel(NULL),
    m_allLoansModel(NULL),
    m_allLoanPositionsModel(NULL),
    m_delegate(NULL),
    m_loansModelRow(-1),
    m_roomsModelRow(-1),
    m_activeFilterState(" isactive = 1")
{
    ui->setupUi(this);
    ui->radioButtonResources->setChecked( true );
    ui->dateTimeSearchFrom->setEnabled(false);
    ui->dateTimeSearchFrom->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEditSearchTo->setEnabled(false);
    ui->dateTimeEditSearchTo->setDateTime(QDateTime::currentDateTime());
    ui->checkBoxSearchByDate->setChecked(false);
    ui->dateTimeEditExtendToDateTime->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEditExtendToDateTime->setMinimumDateTime(QDateTime::currentDateTime());

    if(!dbcon::connecttodb())
        return;
    if (!m_roomsModel)
        m_roomsModel = new QSqlTableModel(this, QSqlDatabase::database());

    m_roomsModel->setTable("allroomloans_view");
    m_roomsModel->select();
    m_roomsModel->setFilter("ende >= NOW()");
    ui->tableViewRooms->setModel(m_roomsModel);
    ui->tableViewRooms->hideColumn(0);
    ui->tableViewRooms->hideColumn(1);
    ui->tableViewRooms->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    if (!m_allLoansModel)
        m_allLoansModel = new QSqlTableModel(this, QSqlDatabase::database());

    m_allLoansModel->setTable("allloans_view");
    m_allLoansModel->select();
    m_allLoansModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    ui->tableViewAllLoans->setModel(m_allLoansModel);
    m_allLoansModel->setFilter(" isactive = 1");
    ui->tableViewAllLoans->hideColumn(0);
    ui->tableViewAllLoans->hideColumn(1);
    ui->tableViewAllLoans->hideColumn(6);
    ui->tableViewAllLoans->hideColumn(7);
    ui->tableViewAllLoans->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    if(!m_allLoanPositionsModel)
        m_allLoanPositionsModel = new QSqlTableModel(this, QSqlDatabase::database());

    m_allLoanPositionsModel->setTable("allloanpositions_view");
    ui->tableViewAllLoanPositions->setModel(m_allLoanPositionsModel);
    ui->tableViewAllLoanPositions->hideColumn(0);
    ui->tableViewAllLoanPositions->hideColumn(1);
    ui->tableViewAllLoanPositions->hideColumn(6);
    ui->tableViewAllLoanPositions->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    if(!m_delegate)
        m_delegate = new LoanPositionDelegate(this);
    ui->tableViewAllLoanPositions->setItemDelegate(m_delegate);
    ui->dateTimeSearchFrom->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEditSearchTo->setDateTime(QDateTime::currentDateTime());

    ui->comboBoxSearchByRooms->clear();
    ui->comboBoxSearchByRooms->addItem("Alle Räume", 0);

    QSqlQuery editingRooms = EditingRoom().getAll();
    while(editingRooms.next())
        ui->comboBoxSearchByRooms->addItem(editingRooms.value("name").toString(), editingRooms.value("id").toInt());
}

/**
 * @brief Destruktor
 */
loanmgmt::~loanmgmt()
{
    delete ui;
}


/**
 * @brief Öffnet die Maske zum Verwalten von Raumreservierungen
 */
void loanmgmt::on_radioButtonEditingRooms_toggled(bool checked)
{
    if (checked)
        ui->stackedWidget->setCurrentIndex(0);
}

/**
 * @brief Öffnet die Maske zum Verwalten von Ressourcenausleihen
 */
void loanmgmt::on_radioButtonResources_toggled(bool checked)
{
    if(checked)
        ui->stackedWidget->setCurrentIndex(1);
}

/**
 * @brief Erlaubt dem Benutzer Ausleihen anzuzeigen, die bereits als erledigt markiert wurde
 * @param Wenn checked = true werden inaktive Ausleihen mitanzeigt
 */
void loanmgmt::on_checkBox_toggled(bool checked)
{
    if(!dbcon::connecttodb())
        return;
    if (checked == true)
        this->setActiveFilterState(" isactive = 0");
    else
        this->setActiveFilterState(" isactive = 1");
    this->on_lineEditSearchResource_textChanged(ui->lineEditSearchResource->text());
    this->on_lineEditSearchUser_textChanged(ui->lineEditSearchUser->text());
}

/**
 * @brief Wurde auf eine Ausleihe geklickt, so werden ihre Positionen im unteren tableView angezeigt
 * @param QModelIndex der ausgewählten Ausleihe
 */
void loanmgmt::on_tableViewAllLoans_clicked(const QModelIndex &index)
{
    //qDebug() << ui->tableViewAllLoans->selectionModel()->currentIndex();
    if(!dbcon::connecttodb())
        return;
    ui->tableViewAllLoans->selectRow(index.row());
    m_allLoanPositionsModel->setFilter(" loanid = " + index.sibling(index.row(), 0).data().toString());
    m_allLoanPositionsModel->select();
    ui->dateTimeEditExtendToDateTime->setEnabled(true);
    ui->dateTimeEditExtendToDateTime->setMinimumDateTime(index.sibling(index.row(), ENDTIME_COL).data().toDateTime());

    if(index.sibling(index.row(), IS_ACTIVE_COL).data().toInt() == 1)
        ui->checkBoxDoneWith->setChecked(false);
    else
        ui->checkBoxDoneWith->setChecked(true);
}

/**
 * @brief Überprüft, ob eine Ausleihe ausgewählt wurde und setzt diese auf Erledigt, unabhängig davon, ob alle Ressourcen zurück gebracht wurden
 */
void loanmgmt::on_checkBoxDoneWith_clicked()
{
    if(!ui->checkBoxDoneWith->isChecked())
        return;

    if(!ui->tableViewAllLoans->selectionModel()->hasSelection())
    {
        QMessageBox::critical(this, "Es ist keine Ausleihe ausgewählt", "Es ist keine Ausleihe ausgewählt");
        return;
    }

    QMessageBox::StandardButton result;
    result = QMessageBox::question(this, "Änderungen übernehmen", "Hiermit wird die Ausleihe als erledigt gesetzt, AUCH WENN NICHT ALLE RESSOURCEN ZURÜCK GEKOMMEN SIND! \nFortfahren?", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::No)
        return;

    if(!dbcon::connecttodb())
        return;

    ui->checkBoxDoneWith->setChecked(true);
    this->setInactive();
    m_allLoansModel->select();
    m_allLoanPositionsModel->select();
}

/**
 * @brief Setzt bei allen Positionen die Retourmenge gleich der ausgeliehenen Menge und setzt die Ausleihe als erledigt
 */
void loanmgmt::on_buttonReturnAllPositions_clicked()
{
    QMessageBox::StandardButton result;
    result = QMessageBox::question(this, "Änderungen übernehmen", "Hiermit wird die Ausleihe als erledigt gesetzt und alle Ressourcen als zurückgekommen eingetragen. \nFortfahren?", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::No)
        return;

    if(!dbcon::connecttodb())
        return;

    if(m_allLoansModel->data(m_allLoansModel->index(ui->tableViewAllLoans->currentIndex().row(), IS_ACTIVE_COL)).toInt() == 0)
    {
        QMessageBox::critical(this, "Fehler", "Die ausgewählte Ausleihe wurde bereits als erledigt gesetzt, Vorgang wird abgebrochen");
        return;
    }

    if(!dbcon::connecttodb())
        return;

    while(m_allLoanPositionsModel->canFetchMore())
        m_allLoanPositionsModel->fetchMore();

    if(!dbcon::connecttodb())
        return;

    for(int i = 0; i < m_allLoanPositionsModel->rowCount(); i++)
    {
        int tmpcurr; //menge = old
        int tmpreturn; //retourmenge = new

        tmpcurr = m_allLoanPositionsModel->data(m_allLoanPositionsModel->index(i, 2)).toInt();
        tmpreturn = m_allLoanPositionsModel->data(m_allLoanPositionsModel->index(i, 3)).toInt();
        m_allLoanPositionsModel->setData(m_allLoanPositionsModel->index(i,3), m_allLoanPositionsModel->data(m_allLoanPositionsModel->index(i, 2)));
        m_allLoanPositionsModel->submit();
        Resource::updateQuantity(m_allLoanPositionsModel->data(m_allLoanPositionsModel->index(i, 6)).toInt(), tmpcurr, tmpreturn, 0, Resource::ReturnAll);
    }

    m_allLoanPositionsModel->select();
    m_allLoanPositionsModel->setTable("allloanpositions_view");

    ui->tableViewAllLoanPositions->setModel(m_allLoanPositionsModel);
    this->setInactive();
    ui->tableViewAllLoanPositions->clearSelection();
    ui->tableViewAllLoans->clearSelection();
}

/**
 * @brief Setzt momentane Ausleihe als inaktiv
 * @return Wurde Ausleihe erfolgreich inaktiv gesetzt wird 0 zurück gegeben, ansonsten -1
 */
int loanmgmt::setInactive()
{
    if(!dbcon::connecttodb())
        return -1;

    if(m_allLoansModel->data(m_allLoansModel->index(ui->tableViewAllLoans->currentIndex().row(), IS_ACTIVE_COL)).toInt() == 0)
        return -1;
    else
        m_allLoansModel->setData(m_allLoansModel->index(ui->tableViewAllLoans->currentIndex().row(), IS_ACTIVE_COL), 0);

    if(dbcon::connecttodb() && m_allLoansModel->submitAll())
    {
        QMessageBox::information(this, "Ausleihe wurde als erledigt gesetzt", "Ausleihe wurde als erledigt gesetzt");
        m_allLoansModel->setTable("allloans_view");
        m_allLoansModel->select();
        m_allLoansModel->setFilter("isactive = 1");
        ui->tableViewAllLoans->setModel(m_allLoansModel);
    }
    else
    {
        QMessageBox::critical(this, "Fehler", m_allLoansModel->lastError().text());
        m_allLoansModel->revertAll();
    }

    return 0;
}

/**
 * @brief Liest die aktuell ausgewählte Ausleihe samt Positionen aus und sendet diese als Druckjob ab.
 */
void loanmgmt::on_buttonPrintPage_clicked()
{
    QItemSelectionModel * selMod = ui->tableViewAllLoans->selectionModel();
    if(!selMod->hasSelection())
    {
        QMessageBox::critical(this, "Nichts ausgewählt", "Es wurde keine Ausleihe ausgewählt");
        return;
    }

    QSqlRecord recId = m_allLoansModel->record(selMod->currentIndex().row());
    QSqlField fieldId = recId.field(0);
    PdfGenerator pdfgen(fieldId.value().toInt());
    pdfgen.printToPrinter();
}

/**
 * @brief Liest die aktuell ausgewählte Ausleihe samt Positionen aus und schreibt diese in PDF Datei.
 */
void loanmgmt::on_buttonPrintPdf_clicked()
{
    QItemSelectionModel * selMod = ui->tableViewAllLoans->selectionModel();
    if(!selMod->hasSelection())
    {
        QMessageBox::critical(this, "Nichts ausgewählt", "Es wurde keine Ausleihe ausgewählt");
        return;
    }

    QMessageBox::StandardButton result;
    result = QMessageBox::question(this, "Bestätigen", "Bestätigen", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::No)
        return;

    QSqlRecord recId = m_allLoansModel->record(selMod->currentIndex().row());
    QSqlField fieldId = recId.field(0);
    PdfGenerator pdfgen(fieldId.value().toInt());
    pdfgen.printToPdfFile();
}

/**
 * @brief Verwendet bei der Editierung von Schnittraumreservierungen. Holt start und endzeit der angeklickten Reservierung und setzt deren Werte in die Felder der Maske
 * @param index
 */
void loanmgmt::on_tableViewRooms_clicked(const QModelIndex &index)
{
    ui->dateTimeEditStart->setDateTime(index.sibling(index.row(), 3).data().toDateTime());
    ui->dateTimeEditEnd->setDateTime(index.sibling(index.row(), 4).data().toDateTime());
}

/**
 * @brief Überprüft die Eingabedaten zur Änderung einer Raumreservierung
 * Überprüft, ob der gewünschte Raum verfügbar ist.
 * Wenn ja wird die Änderung geschrieben. Wenn nein wird die Änderung verworfen.
 */
void loanmgmt::on_buttonSubmitChangesRoom_clicked()
{
    QItemSelectionModel * selMod = ui->tableViewRooms->selectionModel();
    if(!selMod->hasSelection())
        return;

    if(ui->dateTimeEditEnd->dateTime() < ui->dateTimeEditStart->dateTime())
    {
        QMessageBox::critical(this, "Falsche Zeitangabe", "Falsche Zeitangabe");
        this->on_tableViewRooms_clicked(selMod->currentIndex());
        return;
    }

    QSqlRecord tmpRec = m_roomsModel->record(selMod->currentIndex().row());
    QSqlField roomidField = tmpRec.field(1);
    QSqlField idField = tmpRec.field(0);

    RoomLoan LoanToBChanged(idField.value().toInt());
    QList<int> overlapList = LoanToBChanged.checkOverlaps(ui->dateTimeEditStart->dateTime(), ui->dateTimeEditEnd->dateTime(), roomidField.value().toInt());

    if(overlapList.isEmpty() || ((overlapList.count() == 1) && (overlapList[0]) == idField.value().toInt()))
    {
        LoanToBChanged.setStart(ui->dateTimeEditStart->dateTime());
        LoanToBChanged.setEnd(ui->dateTimeEditEnd->dateTime());
        if(LoanToBChanged.update())
        {
            QMessageBox::information(this, "Änderungen übernommen", "Änderungen übernommen");
            m_roomsModel->select();
            ui->dateTimeEditStart->setDateTime(QDateTime());
            ui->dateTimeEditEnd->setDateTime(QDateTime());
        }
        else
            QMessageBox::critical(this, "Fehler", "Fehler");
    }
    else
    {
        QMessageBox::critical(this, "Schnittaum bereits vergeben",
                              QString("Der gewünschte Schnittraum ist zum gewünschten Zeitraum bereits vergeben"));
        return;
    }
}

/**
 * @brief Setzt den Filter zum Suchen von Raumausleihen zurück
 */
void loanmgmt::on_buttonClearFilter_clicked()
{
    m_roomsModel->setFilter("ende >= NOW()");
    ui->checkBoxSearchByDate->setChecked(false);
}

/**
 * @brief Wendet den Filter zum Suchen von Raumausleihen an.
 */
void loanmgmt::on_buttonApplyFilter_clicked()
{
    QString filter = "";

    if(ui->comboBoxSearchByRooms->currentData().toInt() == 0)
    {
        filter.append("RoomID LIKE '%' ");
        if(ui->checkBoxSearchByDate->isChecked())
        {
            //filter.append("AND ");
            filter.append(" AND start <= '" + ui->dateTimeSearchFrom->dateTime().toString("yyyy-MM-dd hh:mm") + "' AND ende >= '" +
                          ui->dateTimeEditSearchTo->dateTime().toString("yyyy-MM-dd hh:mm") + "'");
        }
        filter.append(" AND ende >= NOW()");
    }

    else if(ui->comboBoxSearchByRooms->currentData().toInt() != 0)
    {
        filter.append("RoomID = " + ui->comboBoxSearchByRooms->currentData().toString());
        if(ui->checkBoxSearchByDate->isChecked())
        {
            //filter.append(" AND ");
            filter.append(" AND start <= '" + ui->dateTimeSearchFrom->dateTime().toString("yyyy-MM-dd hh:mm") + "' AND ende >= '" +
                          ui->dateTimeEditSearchTo->dateTime().toString("yyyy-MM-dd hh:mm") + "'");
        }
        filter.append(" AND ende >= NOW()");
    }
    qDebug() << filter;
    m_roomsModel->setFilter(filter);
}

/**
 * @brief Ermöglicht das Suchen von Raumausleihen nach Zeitraum
 * @param checked
 */
void loanmgmt::on_checkBoxSearchByDate_toggled(bool checked)
{
    if(checked == true)
    {
        ui->dateTimeSearchFrom->setEnabled(true);
        ui->dateTimeEditSearchTo->setEnabled(true);
    }
    else
    {
        ui->dateTimeSearchFrom->setEnabled(false);
        ui->dateTimeEditSearchTo->setEnabled(false);
    }
}

/**
 * @brief Ermöglicht das Suchen von Ressourcen in allen vorhandenen Ausleihen
 * @param Suchtext als QString
 */
void loanmgmt::on_lineEditSearchResource_textChanged(const QString &arg1)
{
    if(!dbcon::connecttodb())
        return;

    m_allLoansModel->setFilter(QString(" %1 AND (Name LIKE '%%2%' OR Vorname LIKE '%%2%')"
                                     "AND loanid IN (SELECT loanid FROM allloanpositions_view WHERE Hersteller LIKE '%%3%' OR Typ LIKE '%%3%')")
                               .arg(this->getActiveFilterState()).arg(ui->lineEditSearchUser->text()).arg(arg1));
}

/**
 * @brief Ermöglicht das Suchen von Benutzern in allen möglichen Ausleihen
 * @param Suchtext als QString
 */
void loanmgmt::on_lineEditSearchUser_textChanged(const QString &arg1)
{
    if(!dbcon::connecttodb())
        return;

    m_allLoansModel->setFilter(QString(" %1 AND (Name LIKE '%%2%' OR Vorname LIKE '%%2%')"
                                         "AND loanid IN (SELECT loanid FROM allloanpositions_view WHERE Hersteller LIKE '%%3%' OR Typ LIKE '%%3%')")
                               .arg(this->getActiveFilterState()).arg(arg1).arg(ui->lineEditSearchResource->text()));
}

/**
 * @brief Verlängert ausgewählte Ressourcenausleihe bis zu gewünschtem Zeitpunkt
 */
void loanmgmt::on_pushButtonExtendToDateTime_clicked()
{
    QItemSelectionModel * selMod = ui->tableViewAllLoans->selectionModel();
    if(!selMod->hasSelection())
        return;

    QMessageBox::StandardButton result;
    result = QMessageBox::question(this, "Bestätigen", "Verlängerung bestätigen", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::No)
        return;

    QSqlRecord tmpRec = m_allLoansModel->record(selMod->currentIndex().row());
    QSqlField tmpField = tmpRec.field(0);

    if(!dbcon::connecttodb())
        return;
    ResourceLoan loanToBeExtended(tmpField.value().toInt());
    if(loanToBeExtended.extend(ui->dateTimeEditExtendToDateTime->dateTime()))
    {
        QMessageBox::information(this, "Änderungen übernommen", "Änderungen übernommen");
        m_allLoansModel->select();
        ui->dateTimeEditExtendToDateTime->setEnabled(false);
    }
    else
        QMessageBox::critical(this, "Fehler beim Aktualisieren", QSqlDatabase::database().lastError().text());
}

void loanmgmt::on_buttonEditLoanPushed(QModelIndex id)
{
    if(!dbcon::connecttodb())
        return;
    ui->stackedWidget->setCurrentIndex(1);
    ui->tableViewAllLoans->selectionModel()->setCurrentIndex(id, QItemSelectionModel::Select);
    this->on_tableViewAllLoans_clicked(id);
}

void loanmgmt::on_buttonEditRoomLoanPushed(QModelIndex id)
{
    qDebug() << Q_FUNC_INFO;
    if(!dbcon::connecttodb())
        return;
    ui->stackedWidget->setCurrentIndex(0);
    ui->tableViewRooms->selectionModel()->setCurrentIndex(id, QItemSelectionModel::Select);
    this->on_tableViewRooms_clicked(id);
}



