#include "adminarea.h"
#include "ui_adminarea.h"
#include "dbinterface\user.h"
#include "dbinterface\location.h"

/**
 * @brief Konstruktor
 * @param parent
 */

Adminarea::Adminarea(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Adminarea)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Close)->setText("Schließen");
}

/**
 * @brief Destruktor
 */

Adminarea::~Adminarea()
{
    delete ui;
}

/**
 * @brief Befüllt die DropDowns für AdminUser und Schnitträume neu falls sich etwas geändert hat.
 */

void Adminarea::init()
{
    ui->comboBoxEditingRooms->clear();
    QSqlQuery editingrooms = EditingRoom().getAll();
    while(editingrooms.next())
        ui->comboBoxEditingRooms->addItem(editingrooms.value("name").toString(), editingrooms.value("id"));

    ui->comboBoxAdminUsers->clear();
    //admins = AdminUser::getAdminUsers();
    QSqlQuery admins = AdminUser().getAll();
    while(admins.next())
        ui->comboBoxAdminUsers->addItem(admins.value("username").toString(), admins.value("id"));
}

/**
 * @brief Wird jedes Mal aufgerufen, wenn das Fenster angezeigt wird. Ruft init() auf
 * @param event
 */

void Adminarea::showEvent(QShowEvent *event)
{
    this->init();
    QDialog::showEvent(event);
}

/**
 * @brief Löscht ausgewählten Adminbenutzer aus der Datenbank, fragt Benutzer zuvor nach Bestätigung
 */

void Adminarea::on_buttonDelAdminUser_clicked()
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

void Adminarea::on_buttonAddAdmin_clicked()
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

void Adminarea::on_buttonDelEditingroom_clicked()
{
    QMessageBox::StandardButton result;
    result = QMessageBox::question(this, "Bestätigen", "Bestätigen", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::No)
        return;
    qDebug() << ui->comboBoxEditingRooms->currentData();
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

void Adminarea::on_pushButtonAddEditingroom_clicked()
{
    if(EditingRoom(ui->lineEditNewEditingRoom->text()).insert() != -1)
        QMessageBox::information(this, "Erfolgreich", "Hinzufügen erfolgreich");
    else
        QMessageBox::critical(this, "Fehler", "Fehler");
    this->init();
    ui->lineEditNewEditingRoom->clear();
}
