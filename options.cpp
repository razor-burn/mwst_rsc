#include "options.h"
#include "ui_options.h"
#include "dbinterface\user.h"
#include "dbinterface\location.h"
#include "dbinterface\faculty.h"

/**
 * @brief Konstruktor
 * @param parent
 */

options::options(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::options)
{
    ui->setupUi(this);
}

/**
 * @brief Destruktor
 */

options::~options()
{
    delete ui;
}

/**
 * @brief Befüllt die DropDowns für AdminUser und Schnitträume neu falls sich etwas geändert hat.
 */

void options::init()
{
    ui->comboBoxEditingRooms->clear();
    QSqlQuery editingrooms = EditingRoom().getAll();
    while(editingrooms.next())
        ui->comboBoxEditingRooms->addItem(editingrooms.value("name").toString(), editingrooms.value("id").toInt());

    ui->comboBoxAdminUsers->clear();
    QSqlQuery admins = AdminUser().getAll();
    while(admins.next())
        ui->comboBoxAdminUsers->addItem(admins.value("username").toString(), admins.value("id").toInt());

    ui->comboBoxFaculties->clear();
    QSqlQuery faculties = Faculty().getAll();
    while(faculties.next())
        ui->comboBoxFaculties->addItem(faculties.value("name").toString(), faculties.value("id").toInt());

    ui->comboBoxNewFacultyNumber->clear();
    int max = Faculty().getMax("id").value("max").toInt();
    ui->comboBoxNewFacultyNumber->addItem(QString::number(max + 1), max + 1);
}

/**
 * @brief Wird jedes Mal aufgerufen, wenn das Fenster angezeigt wird. Ruft init() auf
 * @param event
 */

void options::showEvent(QShowEvent *event)
{
    this->init();
    QWidget::showEvent(event);
}

/**
 * @brief Löscht ausgewählten Adminbenutzer aus der Datenbank, fragt Benutzer zuvor nach Bestätigung
 */

void options::on_buttonDelAdminUser_clicked()
{
    QMessageBox::StandardButton result;
    result = QMessageBox::question(this, "Bestätigen", "Bestätigen", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::No)
        return;
    AdminUser delUser(ui->comboBoxAdminUsers->currentData().toInt());
    if(delUser.del() != 0)
        QMessageBox::critical(this, "Fehler", QSqlDatabase::database().lastError().text());
    else
        QMessageBox::information(this, "Erfolgreich", "Löschen erfolgreich");
    this->init();
}

/**
 * @brief Fügt neuen Adminbenutzer in die Datenbank ein, fragt Benutzer zuvor nach Bestätigung
 */

void options::on_buttonAddAdmin_clicked()
{
    if(AdminUser(ui->lineEditNewAdmin->text()).insert() != -1)
        QMessageBox::information(this, "Erfolgreich", "Hinzufügen erfolgreich");
    else
        QMessageBox::critical(this, "Fehler", "Fehler");
    ui->lineEditNewAdmin->clear();
    this->init();
}

/**
 * @brief Löscht ausgewählten Schnittraum aus der Datenbank, fragt Benutzer zuvor nach Bestätigung
 */

void options::on_buttonDelEditingroom_clicked()
{
    QMessageBox::StandardButton result;
    result = QMessageBox::question(this, "Bestätigen", "Bestätigen", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::No)
        return;
    EditingRoom oldRoom(ui->comboBoxEditingRooms->currentData().toInt());
    if(oldRoom.del() == 0)
        QMessageBox::information(this, "Erfolgreich", "Löschen erfolgreich");
    else
        QMessageBox::critical(this, "Fehler", "Fehler");
    this->init();
}

/**
 * @brief Fügt neuen Schnittraum in die Datenbank ein, fragt Benutzer zuvor nach Bestätigung
 */

void options::on_pushButtonAddEditingroom_clicked()
{
    if(EditingRoom(ui->lineEditNewEditingRoom->text()).insert() != -1)
        QMessageBox::information(this, "Erfolgreich", "Hinzufügen erfolgreich");
    else
        QMessageBox::critical(this, "Fehler", "Fehler");
    this->init();
    ui->lineEditNewEditingRoom->clear();
}

void options::on_buttonDelFaculty_clicked()
{
    QMessageBox::StandardButton result;
    result = QMessageBox::question(this, "Bestätigen", "Bestätigen", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::No)
        return;
    Faculty oldFac(ui->comboBoxFaculties->currentData().toInt());
    if(oldFac.del() == 0)
        QMessageBox::information(this, "Erfolgreich", "Löschen erfolgreich");
    else
        QMessageBox::critical(this, "Fehler", "Fehler");
    this->init();
}

void options::on_ButtonAddFaculty_clicked()
{
    if(Faculty(ui->comboBoxNewFacultyNumber->currentData().toInt(), ui->lineEditNewFaculty->text()).insert() != -1)
    {
        QMessageBox::information(this, "Erfolgreich", "Hinzufügen erfolgreich");
        ui->lineEditNewFaculty->clear();
    }
    else
        QMessageBox::critical(this, "Fehler", "Fehler");
    this->init();
}
