#include "dbinterface\dbcon.h"
#include "dbinterface\loan.h"
#include "dbinterface\resource.h"
#include "dbinterface\user.h"
#include "delegates\spinboxdelegate.h"


#include <QSqlField>
#include <QSqlRecord>
#include <QMessageBox>
#include <QAbstractItemModel>
#include <pdfgenerator.h>
#include <QInputDialog>

#define ID_COLUMN 7
#define QTY_COLUMN 8
#define MIN_QTY_COLUMN 9
#define DESIRED_QTY_COLUMN 10

/**
 * @brief Überladen von QDialog::showevent.
 * Wird jedes Mal aufgerufen, wenn der Dialog anzeigt wird.
 * Setzt alle input Widgets zurück und ruft init() auf.
 * @param event
 */
void AddLoan::showEvent(QShowEvent *event)
{
    ui->dateTimeEditEnd->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEditStart->setDateTime(QDateTime::currentDateTime());
    ui->stackedWidget->setCurrentIndex(2);
    if(ui->radioButtonResources->isChecked())
        ui->radioButtonResources->setChecked(false);
    else
        ui->radioButtonRooms->setChecked(false);

    this->on_buttonDelAllLoanPositions_clicked();
    ui->tableWidgetDesiredItems->clearResourcesList();
    ui->lineEditSearchText->clear();

    this->init();

    QDialog::showEvent(event);
}


/**
 * @brief Konstruktor
 * Initialisiert alle Member mit NULL
 * @param parent
 */
AddLoan::AddLoan(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddLoan),
    m_usersModel(NULL),
    m_editingroomsModel(NULL),
    m_resourcesModel(NULL),
    m_mySpinBox(NULL)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Close)->setText("Schließen");
}

/**
 * @brief Destruktor
 */
AddLoan::~AddLoan()
{
    delete ui;
}

/**
 * @brief Sorgt dafür das die QSqlTableModels erstellt werden (falls noch nicht geschehen) und weist diese den tableViews zu.
 * Auch das DropDown Menü in welchem der Admin User ausgewählt wird wird jedes Mal neu befüllt
 * Wird jedes Mal wenn das Fenster neu angezeigt wird aufgerufen, da sich der Inhalt der Datenbank geändert haben kann
 */
void AddLoan::init()
{
    ui->stackedWidget->setCurrentIndex(2);

    if(!dbcon::connecttodb())
        return;

    if(!m_usersModel)
        m_usersModel = new QSqlTableModel(this, QSqlDatabase::database());

    m_usersModel->setTable("users");
    m_usersModel->select();
    ui->tableViewUsers->setModel(m_usersModel);
    ui->tableViewUsers->hideColumn(0);
    ui->tableViewUsers->hideColumn(3);
    ui->tableViewUsers->hideColumn(4);
    ui->tableViewUsers->hideColumn(5);
    ui->tableViewEditingrooms->resizeColumnsToContents();
    ui->tableViewUsers->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewUsers->clearSelection();

    if(!m_editingroomsModel)
        m_editingroomsModel = new QSqlTableModel(this, QSqlDatabase::database());

    m_editingroomsModel->setTable("editingrooms");
    m_editingroomsModel->select();
    ui->tableViewEditingrooms->setModel(m_editingroomsModel);
    ui->tableViewEditingrooms->hideColumn(0);
    ui->tableViewEditingrooms->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewEditingrooms->clearSelection();

    if(!m_resourcesModel)
        m_resourcesModel = new QSqlTableModel(this, QSqlDatabase::database());

    m_resourcesModel->setTable("allresources_view");
    m_resourcesModel->select();
    ui->tableViewResources->setModel(m_resourcesModel);
    ui->tableViewResources->hideColumn(1);
    ui->tableViewResources->hideColumn(7);
    ui->tableViewResources->hideColumn(9);
    ui->tableViewResources->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewResources->setSortingEnabled(true);

    ui->tableWidgetDesiredItems->setColumnCount(11);
    ui->tableWidgetDesiredItems->setHorizontalHeaderLabels(QStringList() << "Kategorie" << "KategorieID" << "Raum" << "Schrank" <<
                                                           "Hersteller" << "Bezeichnung" << "Beschreibung" <<
                                          "ResourceID" << "Lagernde Menge" << "Minimale Menge" << "Zu verleihende Menge");

    ui->tableWidgetDesiredItems->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    if(!m_mySpinBox)
        m_mySpinBox = new SpinBoxDelegate(ui->tableWidgetDesiredItems);

    ui->tableWidgetDesiredItems->setItemDelegateForColumn(10, m_mySpinBox);
    ui->tableWidgetDesiredItems->hideColumn(1);
    ui->tableWidgetDesiredItems->hideColumn(2);
    ui->tableWidgetDesiredItems->hideColumn(3);
    ui->tableWidgetDesiredItems->hideColumn(7);
    ui->tableWidgetDesiredItems->hideColumn(9);

    ui->comboBoxEmployee->clear();
    QSqlQuery allAdmins = AdminUser().getAll();
    //QSqlQuery tmpqry = AdminUser::getAdminUsers();
    while(allAdmins.next())
        ui->comboBoxEmployee->addItem(allAdmins.value("username").toString(), allAdmins.value("id").toString());
}

/**
 * @brief Öffnet die Maske zum Hinzufügen von Schnittraum-Reservierungen
 */
void AddLoan::on_radioButtonRooms_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

/**
 * @brief Öffnet die Maske zum Hinzufügen von Ressourcen-Reservierungen
 */
void AddLoan::on_radioButtonResources_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

/**
 * @brief Ermöglicht das Suchen nach Benutzern nach Vor und Nachnamen (mit Substrings)
 * @param Benutzer, nach dem gesucht werden soll
 */
void AddLoan::on_lineEditUserSearch_textChanged(const QString & arg1)
{
    if(!dbcon::connecttodb())
        return;
    m_usersModel->setFilter(QString("name LIKE '%%1%' OR firstname LIKE '%%1%'").arg(arg1));
}

/**
 * @brief Setzt das Eingabefeld zum Suchen nach Benutzern zurück auf leer
 */
void AddLoan::on_buttonClearUserSearch_clicked()
{
    ui->lineEditUserSearch->clear();
}

/**
 * @brief Wird aufgerufen, wenn eine Schnittraum-Reservierung abgesendet wurde.
 * Überprüft die Validität von Start und Endzeit und ob Benutzer und Raum ausgewählt wurden.
 * Überprüft, ob der gewünschte Schnittraum zum gewünschten Zeitraum noch frei ist.
 * Ist dieser frei: Reservierung wird eingetragen. Ist er nicht frei: Fehler und return.
 */
void AddLoan::on_buttonSubmitLoanEditingroom_clicked()
{
    /*
     *  Es muss einfacher sein als so, um an die daten ranzukommen
     */
    QItemSelectionModel * selModRooms = ui->tableViewEditingrooms->selectionModel();
    QItemSelectionModel * selModUsers = ui->tableViewUsers->selectionModel();
    QDateTime start = ui->dateTimeEditStart->dateTime();
    QDateTime end = ui->dateTimeEditEnd->dateTime();

    if(!start.isValid() || !end.isValid())
    {
        QMessageBox::critical(this, "Falsche Zeitangabe", "Sie haben bei Start bzw. Endzeit ungültige Angaben gemacht");
        return;
    }
    if(end <= start)
    {
        QMessageBox::critical(this, "Falsche Zeitangabe", "Die Startzeit kann nicht größer oder gleich der Endzeit sein");
        return;
    }
    if(!selModRooms->hasSelection() || !selModUsers->hasSelection())
    {
        QMessageBox::critical(this, "Bitte Benutzer und Raum auswählen", "Bitte Benutzer und Raum auswählen");
        return;
    }

    if(!dbcon::connecttodb())
        return;

    QSqlRecord recRooms = m_editingroomsModel->record(selModRooms->currentIndex().row());
    QSqlField fieldRoomId = recRooms.field(0);
    QSqlRecord recUsers = m_usersModel->record(selModUsers->currentIndex().row());
    QSqlField fieldUserId = recUsers.field(0);

    RoomLoan newRoomLoan(fieldRoomId.value().toInt(), fieldUserId.value().toInt(), ui->dateTimeEditStart->dateTime(), ui->dateTimeEditEnd->dateTime(),
                         ui->comboBoxEmployee->currentData().toInt());
    QList<int> tmpList = newRoomLoan.checkOverlaps(ui->dateTimeEditStart->dateTime(), ui->dateTimeEditEnd->dateTime());

    if(tmpList.isEmpty())
    {
        int result = newRoomLoan.insert();
        if(result != -1)
        {
            QMessageBox::information(this, "Erfolgreich", "Die Ausleihe wurde hinzugefügt");
            ui->tableViewUsers->clearSelection();
            ui->tableViewEditingrooms->clearSelection();
        }
        else
            QMessageBox::critical(this, "Fehler", "Fehler beim Einfügen");
    }
    else
    {
        QMessageBox::critical(this, "Schnittaum bereits vergeben",
                              QString("Der gewünschte Schnittraum ist zum gewünschten Zeitraum bereits vergeben"));
    }
}

/**
 * @brief Ermöglicht das Suchen nach Ressourcen nach den DB-Feldern make, type und description (mit Substrings)
 * @param make, type oder description, nach der gesucht werden soll als QString.
 */

void AddLoan::on_lineEditSearchText_textChanged(const QString &arg1)
{
    if(!dbcon::connecttodb())
        return;

    QString tmp = QString("Hersteller LIKE '%%1%' OR Typ LIKE '%%1%' OR Beschreibung LIKE '%%1%'").arg(arg1);
    m_resourcesModel->setFilter(tmp);
}

/**
 * @brief Fügt eine gewünschte Ressource in eine temporäre Liste ein (grafisch dargestellt durch MyTableWidget) bevor die gesamte Ausleihe abgesendet werden kann.
 * Überprüft, eine Ressource ausgewählt wurde, ob von dieser noch genug da ist (>0) und ob diese schon einmal hinzugefügt wurde.
 * Im Erfolgsfall wird Ressource eingetragen, ansonsten: return
 */

void AddLoan::on_buttonAdd_clicked()
{

    QItemSelectionModel * tmpSelMod  = ui->tableViewResources->selectionModel();

    if(!tmpSelMod->hasSelection())
    {
        QMessageBox::critical(this, "Nichts ausgewählt", "Bitte Ressourcen auswählen");
        return;
    }
    else
    {
        QSqlRecord tmpRec = m_resourcesModel->record(tmpSelMod->currentIndex().row());
        QList<QString> tmpList;

        for(int i = 0; i < m_resourcesModel->columnCount(); i++)
        {
            QSqlField tmpField = tmpRec.field(i);
            tmpList << tmpField.value().toString();
        }

        if(tmpRec.field(QTY_COLUMN).value().toInt() == 0)
        {
            QMessageBox::critical(this, "Nichts mehr vorhanden", "Item kann nicht hinzugefügt werden, es ist nichts mehr da");
            return;
        }

        if(!ui->tableWidgetDesiredItems->checkResourcesListForEntry(tmpRec.field(ID_COLUMN).value().toInt()))
        {
            if(!dbcon::connecttodb())
                return;

            int newId = ui->tableWidgetDesiredItems->addRow(tmpList);
            if(newId > 0)
                ui->tableWidgetDesiredItems->appendToResourcesList(newId);
            else
                QMessageBox::critical(this, "Fehler", "Unbekannter Fehler");
        }
        else
        {
            QMessageBox::critical(this, "Doppelte Einträge nicht erlaubt", "Dieselbe Ressource 2 Mal in einer Ausleihe ist verboten"
                                 "\nErhöhen Sie stattdessen die Auszuleihende Menge");
        }
    }
}

/**
 * @brief Erlaubt das Löschen aus der Liste der auszuleihenden Ressourcen.
 * Überprüft, ob etwas zum Löschen vorhanden ist und ob etwas ausgewählt wurde.
 * Im Erfolgsfall wird Eintrag entfernt, ansonsten: return
 */

void AddLoan::on_buttonDelLoanPosition_clicked()
{
    if(ui->tableWidgetDesiredItems->rowCount() == 0)
    {
        QMessageBox::critical(this, "Nichts vorhanden", "Es ist nichts zum Löschen vorhanden");
        return;
    }

    if(!ui->tableWidgetDesiredItems->selectionModel()->hasSelection())
    {
        QMessageBox::critical(this, "Nichts ausgewählt", "Es wurde nichts zum Löschen ausgewählt");
        return;
    }
    ui->tableWidgetDesiredItems->removeFromResourcesList(ui->tableWidgetDesiredItems->
                                                         item(ui->tableWidgetDesiredItems->currentRow(), ID_COLUMN)->text().toInt());

    if(ui->tableWidgetDesiredItems->rowCount() > 0)
        ui->tableWidgetDesiredItems->removeRow(ui->tableWidgetDesiredItems->currentRow());

}

/**
 * @brief Entfernt alle Einträge aus der Liste der auszuleihenden Ressourcen
 */

void AddLoan::on_buttonDelAllLoanPositions_clicked()
{
    ui->tableWidgetDesiredItems->clearResourcesList();
    while(ui->tableWidgetDesiredItems->rowCount() > 0)
        ui->tableWidgetDesiredItems->removeRow(0);
}

/**
 * @brief Wird aufgerufen, wenn eine Ressourcenreservierung abgesendet wurde.
 * Überprüft, ob Ressourcen zum Ausleihen angegeben wurden, ob bei diesen jeweils eine Menge >= 1 angegeben wurde, ob Start und Endzeit valide sind.
 * Falls eines davon nicht der Fall ist: return
 *
 * Wenn Radiobutton_printtoPdf aktiviert ist wird eine pdf Datei zur Bestätigung geschrieben.
 * Bei Radiobutton_printtoPrinter wird ein Druckerdialog angezeigt der es ermöglicht, die Bestätigung auf Papier auszudrucken
 */

void AddLoan::on_buttonSubmitLoanResources_clicked()
{
    if(ui->tableWidgetDesiredItems->rowCount() == 0)
    {
        QMessageBox::critical(this, "Nichts ausgewählt", "Bitte Resourcen auswählen");
        return;
    }

    for(int i = 0; i < ui->tableWidgetDesiredItems->rowCount(); i++)
    {
        if(ui->tableWidgetDesiredItems->item(i, DESIRED_QTY_COLUMN)->text().isEmpty() || ui->tableWidgetDesiredItems->item(i, DESIRED_QTY_COLUMN)->text().toInt() == 0)
        {
            QMessageBox::critical(this, "Falsche Mengenangaben", "Sie haben bei mindestens einem Item eine unzulässige Menge angegeben");
            return;
        }
    }

    QItemSelectionModel * selModUsers = ui->tableViewUsers->selectionModel();
    if(!selModUsers->hasSelection())
    {
        QMessageBox::critical(this, "Kein Benutzer", "Es wurde kein Benutzer ausgewählt");
        return;
    }

    QDateTime start = ui->dateTimeEditStart->dateTime();
    QDateTime end = ui->dateTimeEditEnd->dateTime();

    if(!start.isValid() || !end.isValid())
    {
        QMessageBox::critical(this, "Falsche Zeitangabe", "Sie haben bei Start bzw. Endzeit ungültige Angaben gemacht");
        return;
    }
    if(end <= start)
    {
        QMessageBox::critical(this, "Falsche Zeitangabe", "Die Startzeit kann nicht größer oder gleich der Endzeit sein");
        return;
    }

    QSqlRecord recUsers = m_usersModel->record(selModUsers->currentIndex().row());
    QSqlField userIdField = recUsers.field(0);

    if(!dbcon::connecttodb())
        return;

    int goesabroad = (ui->checkBoxGoesAbroad->isChecked()) ? 1 : 0;
    ResourceLoan newLoan(userIdField.value().toInt(), ui->dateTimeEditStart->dateTime(), ui->dateTimeEditEnd->dateTime(),
                         QDateTime::currentDateTime(), ui->comboBoxEmployee->currentData().toInt(), goesabroad, 1);
    QSqlDatabase::database().transaction();
    int newLoanId = newLoan.insert();
    if(newLoanId < 0)
    {
        QMessageBox::critical(this, "Fehler beim Einfügen", QSqlDatabase::database().lastError().text());
        QSqlDatabase::database().rollback();
        return;
    }

    for(int i = 0; i < ui->tableWidgetDesiredItems->rowCount(); i++)
    {
        LoanPosition loanPos(newLoanId, ui->tableWidgetDesiredItems->item(i, ID_COLUMN)->text().toInt(), ui->tableWidgetDesiredItems->item(i, DESIRED_QTY_COLUMN)->text().toInt(), 0);
        if(loanPos.insert() != -1)
            Resource::updateQuantity(loanPos.resourceId(), 0, loanPos.quantity(), 0, Resource::OnLoanPositionInserted);
        else
        {
            qDebug() << "posinserted";
            QSqlDatabase::database().rollback();
            QMessageBox::critical(this, "Fehler beim einfügen", QSqlDatabase::database().lastError().text());
            return;
        }
    }

    PdfGenerator pdfGen(newLoanId);

    QMessageBox::StandardButton result;
    result = QMessageBox::question(this, "Bestätigen", "Bestätigen", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::No)
    {
        QMessageBox::critical(this, "Abbruch durch Benutzer", "Vorgang wurde abgebrochen, Änderungen in der Datenbank wurden rückgängig gemacht");
        QSqlDatabase::database().rollback();
        return;
    }
    int retVal;
    if(ui->radioButtonPrintPdf->isChecked())
        retVal = pdfGen.printToPdfFile();
    else
        retVal = pdfGen.printToPrinter();

    if(retVal != 0)
    {
        QMessageBox::critical(this, "Fehler beim Erstellen des Dokumentes", QSqlDatabase::database().lastError().text());
        QSqlDatabase::database().rollback();
        return;
    }
    QSqlDatabase::database().commit();

    ui->tableViewResources->clearSelection();
    ui->tableViewUsers->clearSelection();
    m_resourcesModel->select();
    this->on_buttonDelAllLoanPositions_clicked();
    ui->tableWidgetDesiredItems->clearResourcesList();
}

/**
 * @brief Überprüft nach dem Aufruf von on_buttonAdd_clicked(), ob die vorhandene Menge der gewünschten Ressourcen <= der in der Datenbank eingetragenen Minimalen Menge ist
 * Wenn ja, wird der Benutzer informiert, das von der gewünschten Ressource nichtmehr viel da ist.
 */

void AddLoan::on_tableWidgetDesiredItems_itemChanged(QTableWidgetItem *item)
{
    if(item->column() != DESIRED_QTY_COLUMN)
        return;

    if(ui->tableWidgetDesiredItems->item(item->row(), QTY_COLUMN)->text().toInt() - item->data(Qt::DisplayRole).toInt()
            <= ui->tableWidgetDesiredItems->item(item->row(), MIN_QTY_COLUMN)->text().toInt()
       && item->data(Qt::DisplayRole).toInt() > 0)
    {

        QMessageBox::information(this, "Minimale Menge erreicht", "Von der geüwnschten Ressource ist nichtmehr viel da");
    }
}

/**
 * @brief Ruft on_buttonAdd_clicked() auf.
 */

void AddLoan::on_tableViewResources_doubleClicked(const QModelIndex &)
{
    this->on_buttonAdd_clicked();
}

void AddLoan::on_buttonClearSearchText_clicked()
{
    ui->lineEditSearchText->clear();
}
