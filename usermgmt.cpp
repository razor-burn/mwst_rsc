#include "usermgmt.h"
#include "ui_usermgmt.h"
#include "dbinterface\dbcon.h"
#include "dbinterface\user.h"

#include <QMessageBox>
#include <QSqlRecord>
#include <QItemSelectionModel>
#include <QSqlField>
#include <QInputDialog>

#define TEL_COLUMN 5
#define EMAIL_COLUMN 3
#define FAC_COLUMN 4


/**
 * @brief Überladen von QDialog::showevent.
 * Wird jedes Mal aufgerufen, wenn der Dialog anzeigt wird.
 * Setzt alle input Widgets zurück und aktualisiert das TableModel
 * @param event
 */
void usermgmt::showEvent(QShowEvent *event)
{
    m_model->select();
    QWidget::showEvent(event);
    populateComboBoxFaculties();
}

/**
 * @brief Konstruktor. Ruft initView() auf und verbindet das Signal dataChanged von model mit dem Slot on_mode_datachanged
 * @param parent
 */
usermgmt::usermgmt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::usermgmt),
    m_model(NULL),
    mySpinBoxDelegate(NULL),
    m_lecturersModel(NULL),
    m_modulesModel(NULL)
{
    ui->setupUi(this);

    if(!dbcon::connecttodb())
        return;

    if(!m_model)
        m_model = new QSqlTableModel(this, QSqlDatabase::database());

    m_model->setTable("users");
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_model->select();
    m_model->setHeaderData(1, Qt::Horizontal, "Name");
    m_model->setHeaderData(2, Qt::Horizontal, "Vorname");
    m_model->setHeaderData(3, Qt::Horizontal, "Email");
    m_model->setHeaderData(4, Qt::Horizontal, "Fachbereich");
    m_model->setHeaderData(5, Qt::Horizontal, "Telefon");
    m_model->setHeaderData(6, Qt::Horizontal, "Adresse");

    if(!mySpinBoxDelegate)
        mySpinBoxDelegate = new SpinBoxDelegate(this);

    ui->tableView->setModel(m_model);
    ui->tableView->hideColumn(0);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setItemDelegateForColumn(FAC_COLUMN, mySpinBoxDelegate);

    if(!m_lecturersModel)
        m_lecturersModel = new QSqlTableModel(this, QSqlDatabase::database());

    m_lecturersModel->setTable("lecturers");
    m_lecturersModel->select();
    ui->tableViewLecturers->setModel(m_lecturersModel);
    ui->tableViewLecturers->hideColumn(0);
    ui->tableViewLecturers->hideColumn(3);
    ui->tableViewLecturers->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    if(!m_modulesModel)
        m_modulesModel = new QSqlTableModel(this, QSqlDatabase::database());
    m_modulesModel->setTable("modules");
    m_modulesModel->select();
    ui->tableViewModules->setModel(m_modulesModel);
    ui->tableViewModules->hideColumn(0);
    ui->tableViewModules->hideColumn(2);
    ui->tableViewModules->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    initView();
    connect(m_model, SIGNAL(dataChanged(const QModelIndex, const QModelIndex)), SLOT(on_model_dataChanged(const QModelIndex, const QModelIndex)));


}

/**
 * @brief Destruktor
 */
usermgmt::~usermgmt()
{
    delete ui;
}


/**
 * @brief Wird jedes Mal aufgerufen, wenn sich Daten im Modell geändert haben, sprich wenn der Benutzer die Daten eines bestehenden Benutzers geändert hat.
 * @param topLeft topLeft und bottomRight stellen zusammen einen Rechteckigen Bereich dar, in welchem sich die Daten geändert haben. Da der Benutzer immer nur
 * eine Zelle editieren kann, ist topLeft == bottomRight
 * @param bottomRight
 */
void usermgmt::on_model_dataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight)
{
    if(topLeft != bottomRight)
        return;

    QItemSelectionModel * selectionModel = ui->tableView->selectionModel();

    if(selectionModel->hasSelection())
    {
        if(topLeft.column() == TEL_COLUMN)
        {
            if(this->checkTelFormat(ui->tableView->model()->data(topLeft).toString()))
                m_model->submitAll();
            else
                m_model->revertAll();
        }
        else if(topLeft.column() == EMAIL_COLUMN)
        {
            if(this->checkMailFormat(ui->tableView->model()->data(topLeft).toString()))
                m_model->submitAll();
            else
                m_model->revertAll();
        }
        else
            m_model->submitAll();
    }
}


/**
 * @brief Erstellt das QSqlTableModel (falls noch nicht geschehen) und weist es dem tableView zu
 */
void usermgmt::initView()
{


}

/**
 * @brief Leert alle lineedits
 */
void usermgmt::clearLineEdits()
{

    ui->lineEditEmail->clear();
    ui->lineEditfirstName->clear();
    ui->lineEditSurName->clear();
    ui->lineEditTelephone->clear();
    ui->lineEditUserSearch->clear();
    //ui->lineEditPostAddress->clear();
}

/**
 * @brief Überprüft einen gegebenen String darauf, ob dieser eine gültige Email Adresse darstellt, es muss also ein "@" Zeichen und ein "." Zeichen darin vorkommen
 * @param mail Zu überprüfender String
 * @return Ist der String eine gültige Email Adresse: return true, ansonsten: return false
 */
bool usermgmt::checkMailFormat(const QString &mail)
{
    bool result = false;
    if(mail.contains(QRegExp(".*\\@.*\\..*")))
        result = true;
    else
        QMessageBox::critical(this, "Falsches Emailformat!", "Falsches Emailformat!");
    return result;
}

/**
 * @brief Überprüft einen gegebenen String darauf, ob dieser eine gültige Handynummer oder deutsche Festnetznummer darstellt.
 * @param tel Zu überprüfender String
 * @return Ist der String eine gültige Telefonnummer: return true. ansonsten: return false
 */
bool usermgmt::checkTelFormat(const QString &tel)
{
    bool result = false;
    if(tel.contains(QRegExp("^(\\+49|0)[0-9]{2,5}.{0,3}[0-9]{3,8}")))
        result = true;
    else
        QMessageBox::critical(this, "Falsches Telefonnummerformat!", "Falsches Telefonnummerformat!");
    return result;
}

void usermgmt::populateComboBoxFaculties()
{
    ui->comboBoxFaculties->clear();
    ui->comboBoxFaculties_2->clear();
    QSqlQuery faculties = Faculty().getAll();
    while(faculties.next())
    {
        ui->comboBoxFaculties->addItem(faculties.value("name").toString(), faculties.value("id").toInt());
        ui->comboBoxFaculties_2->addItem(faculties.value("id").toString(), faculties.value("id").toInt());
    }
}

/**
 * @brief Erlaubt das Suchen von Benutzern
 * @param arg1 Name oder Vorname des zu suchenden Benutzers
 */
void usermgmt::on_lineEditUserSearch_textChanged(const QString & arg1)
{
    m_model->setFilter(QString("name LIKE '%%1%' OR firstname LIKE '%%1%'").arg(arg1));
    m_model->select();
}

/**
 * @brief Leert das Lineedit zum Suchen von Benutzern
 */
void usermgmt::on_buttonClear_clicked()
{
    ui->lineEditUserSearch->clear();
    m_model->select();
}

/**
 * @brief Fügt Benutzer in Datenbank ein. Überprüft vorher, ob Telefon und Emailangaben valide sind.
 */
void usermgmt::on_buttonAdd_clicked()
{
    if(!this->checkTelFormat(ui->lineEditTelephone->text()) || !this->checkMailFormat(ui->lineEditEmail->text()))
        return;

    if(!dbcon::connecttodb())
        return;

    User newUser(ui->lineEditSurName->text(), ui->lineEditfirstName->text(), ui->lineEditEmail->text(), ui->comboBoxFaculties->currentData().toInt(),
                 ui->lineEditTelephone->text(), ui->lineEditStreet->text());
    if(newUser.insert() != -1)
    {
        QMessageBox::information(this, "Erfolgreich", "Benutzer wurde hinzugefügt");
        this->clearLineEdits();
    }
    m_model->select();
}

/**
 * @brief Löscht Benutzer aus Datenbank
 */
void usermgmt::on_buttonDel_clicked()
{
    if(!ui->tableView->selectionModel()->hasSelection())
        return;

    QMessageBox::StandardButton result;
    result = QMessageBox::question(this, "Bestätigen", "Benutzer wirklich löschen?", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::No)
        return;

    if(!dbcon::connecttodb())
        return;

    User oldUser(ui->tableView->currentIndex().sibling(ui->tableView->currentIndex().row(), 0).data().toInt());
    if(oldUser.del() == 0)
        QMessageBox::information(this, "Erfolgreich", "Benutzer wurde gelöscht");
    else
        QMessageBox::critical(this, "Benutzer kann nicht gelöscht werden", "Der ausgewählte Benutzer hat noch etwas ausgeliehen und kann daher nicht gelöscht werden");
    m_model->select();
}

void usermgmt::on_comboBoxFaculties_2_currentIndexChanged(int index)
{
    m_lecturersModel->setFilter(QString("faculty = %1").arg(ui->comboBoxFaculties_2->itemData(index).toInt()));
    on_lineEditSearchLecturers_textChanged(ui->lineEditSearchLecturers->text());
    //m_lecturersModel->select();
    m_modulesModel->setFilter(QString("faculty = %1").arg(ui->comboBoxFaculties_2->itemData(index).toInt()));
    on_lineEditSearchModules_textChanged(ui->lineEditSearchModules->text());
    //m_modulesModel->select();
}

void usermgmt::on_buttonAddLecturer_clicked()
{
    if(ui->lineEditNewLecturerFirstName->text().isEmpty() || ui->lineEditNewLecturerLastName->text().isEmpty())
        return;
    if(Lecturer(ui->lineEditNewLecturerLastName->text(), ui->lineEditNewLecturerFirstName->text(), ui->comboBoxFaculties_2->currentData().toInt()).insert() != -1)
    {
        QMessageBox::information(this, "Erfolgreich", QString("Die/der DozentIn %1, %2 wurde für Fachbereich %3 hinzugefügt")
                                 .arg(ui->lineEditNewLecturerLastName->text())
                                 .arg(ui->lineEditNewLecturerFirstName->text()).arg(ui->comboBoxFaculties_2->currentData().toInt()));
        ui->lineEditNewLecturerFirstName->clear();
        ui->lineEditNewLecturerLastName->clear();
    }
    else
        QMessageBox::critical(this, "Fehler", "Fehler");
    m_lecturersModel->select();
}

void usermgmt::on_lineEditSearchLecturers_textChanged(const QString &arg1)
{
    QString filter2;
    //Kein Suchbegriff, FB ist gesetzt
    if(arg1 == "" && m_lecturersModel->filter() != "")
        filter2 = QString("faculty = %1").arg(ui->comboBoxFaculties_2->currentData().toInt());
    //Suchbegriff, FB ist gesetzt
    if(arg1 != "" && m_lecturersModel->filter() != "")
        filter2 = QString("faculty = %1").arg(ui->comboBoxFaculties_2->currentData().toInt()).append(QString(" AND (name LIKE '%%1%' OR firstname LIKE '%%1%')").arg(arg1));
    m_lecturersModel->setFilter(filter2);
    filter2.clear();
    m_lecturersModel->select();
}

void usermgmt::on_buttonDelLecturer_clicked()
{
    if(!ui->tableViewLecturers->selectionModel()->hasSelection())
        return;

    QMessageBox::StandardButton result;
    result = QMessageBox::question(this, "Bestätigen", "Dozent wirklich löschen?", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::No)
        return;

    if(!dbcon::connecttodb())
        return;

    Lecturer oldLecturer(ui->tableViewLecturers->currentIndex().sibling(ui->tableViewLecturers->currentIndex().row(), 0).data().toInt());
    if(oldLecturer.del() == 0)
        QMessageBox::information(this, "Erfolgreich", "Dozent wurde gelöscht");
    else
        QMessageBox::critical(this, "Dozent kann nicht gelöscht werden", "Fehler");
    m_lecturersModel->select();
}

void usermgmt::on_buttonAddModule_clicked()
{
    if(ui->lineEditNewModule->text().isEmpty())
        return;

    if(Module(ui->lineEditNewModule->text(), ui->comboBoxFaculties_2->currentData().toInt()).insert() != -1)
    {
        QMessageBox::information(this, "Erfolgreich", QString("Das Modul %1 wurde für Fachbereich %2 erfolgreich hinzugefügt")
                                 .arg(ui->lineEditNewModule->text()).arg(ui->comboBoxFaculties_2->currentData().toInt()));
        ui->lineEditNewModule->clear();
    }
    else
        QMessageBox::critical(this, "Fehler", "Fehler");

    m_modulesModel->select();
}

void usermgmt::on_lineEditSearchModules_textChanged(const QString &arg1)
{
    qDebug() << m_modulesModel->filter();
    QString filter;
    //Kein Suchbegriff, FB ist gesetzt
    if(arg1 == "" && m_modulesModel->filter() != "")
        filter = QString("faculty = %1").arg(ui->comboBoxFaculties_2->currentData().toInt());
    //Suchbegriff, FB ist gesetzt
    if(arg1 != "" && m_modulesModel->filter() != "")
        filter = QString("faculty = %1").arg(ui->comboBoxFaculties_2->currentData().toInt()).append(QString(" AND name LIKE '%%1%'").arg(arg1));
    m_modulesModel->setFilter(filter);
    qDebug() << m_modulesModel->filter();
    filter.clear();
    m_modulesModel->select();
}

void usermgmt::on_buttonDelModule_clicked()
{
    if(!ui->tableViewModules->selectionModel()->hasSelection())
        return;

    QMessageBox::StandardButton result;
    result = QMessageBox::question(this, "Bestätigen", "Modul wirklich löschen?", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::No)
        return;

    if(!dbcon::connecttodb())
        return;

    Module oldModule(ui->tableViewModules->currentIndex().sibling(ui->tableViewModules->currentIndex().row(), 0).data().toInt());
    if(oldModule.del() == 0)
        QMessageBox::information(this, "Erfolgreich", "Modul wurde gelöscht");
    else
        QMessageBox::critical(this, "Modul kann nicht gelöscht werden", "Fehler");
    m_modulesModel->select();
}
