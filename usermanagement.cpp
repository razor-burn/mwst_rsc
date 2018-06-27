#include "usermanagement.h"
#include "ui_usermanagement.h"
#include "dbinterface\dbcon.h"
#include "dbinterface\user.h"

#include <QMessageBox>
#include <QSqlRecord>
#include <QItemSelectionModel>
#include <QSqlField>

#define TEL_COLUMN 5
#define EMAIL_COLUMN 3
#define FAC_COLUMN 4


/**
 * @brief Überladen von QDialog::showevent.
 * Wird jedes Mal aufgerufen, wenn der Dialog anzeigt wird.
 * Setzt alle input Widgets zurück und aktualisiert das TableModel
 * @param event
 */
void UserManagement::showEvent(QShowEvent *event)
{
    ui->lineEditEmail->clear();
    ui->lineEditfirstName->clear();
    ui->lineEditSurName->clear();
    ui->lineEditTelephone->clear();
    ui->lineEditUserSearch->clear();
    this->initView();
    QDialog::showEvent(event);
}

/**
 * @brief Konstruktor. Ruft initView() auf und verbindet das Signal dataChanged von model mit dem Slot on_mode_datachanged
 * @param parent
 */
UserManagement::UserManagement(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserManagement),
    m_model(NULL),
    mySpinBoxDelegate(NULL)
{
    ui->setupUi(this);
    initView();
    connect(m_model, SIGNAL(dataChanged(const QModelIndex, const QModelIndex)), SLOT(on_model_dataChanged(const QModelIndex, const QModelIndex)));
    ui->buttonBox->button(QDialogButtonBox::Close)->setText("Schließen");
}

/**
 * @brief Wird jedes Mal aufgerufen, wenn sich Daten im Modell geändert haben, sprich wenn der Benutzer die Daten eines bestehenden Benutzers geändert hat.
 * @param topLeft topLeft und bottomRight stellen zusammen einen Rechteckigen Bereich dar, in welchem sich die Daten geändert haben. Da der Benutzer immer nur
 * eine Zelle editieren kann, ist topLeft == bottomRight
 * @param bottomRight
 */
void UserManagement::on_model_dataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight)
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
 * @brief Destruktor
 */
UserManagement::~UserManagement()
{
    delete ui;
}

/**
 * @brief Erstellt das QSqlTableModel (falls noch nicht geschehen) und weist es dem tableView zu
 */
void UserManagement::initView()
{
        if(!dbcon::connecttodb())
            return;

        if(!m_model)
            m_model = new QSqlTableModel(this, QSqlDatabase::database());
        m_model->setTable("users");
        m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        m_model->select();

        if(!mySpinBoxDelegate)
            mySpinBoxDelegate = new SpinBoxDelegate(this);

        ui->tableView->setModel(m_model);
        ui->tableView->hideColumn(0);
        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableView->setSortingEnabled(true);
        ui->tableView->setItemDelegateForColumn(FAC_COLUMN, mySpinBoxDelegate);
        m_model->setHeaderData(1, Qt::Horizontal, "Name");
        m_model->setHeaderData(2, Qt::Horizontal, "Vorname");
        m_model->setHeaderData(3, Qt::Horizontal, "Email");
        m_model->setHeaderData(4, Qt::Horizontal, "Fachbereich");
        m_model->setHeaderData(5, Qt::Horizontal, "Telefon");

}

/**
 * @brief Leert alle lineedits
 */
void UserManagement::clearLineEdits()
{
    ui->lineEditEmail->clear();
    ui->lineEditfirstName->clear();
    ui->lineEditSurName->clear();
    ui->lineEditTelephone->clear();
    ui->lineEditUserSearch->clear();
}

/**
 * @brief Überprüft einen gegebenen String darauf, ob dieser eine gültige Email Adresse darstellt, es muss also ein "@" Zeichen und ein "." Zeichen darin vorkommen
 * @param mail Zu überprüfender String
 * @return Ist der String eine gültige Email Adresse: return true, ansonsten: return false
 */
bool UserManagement::checkMailFormat(const QString &mail)
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
bool UserManagement::checkTelFormat(const QString &tel)
{
    bool result = false;
    if(tel.contains(QRegExp("^(\\+49|0)[0-9]{2,5}.{0,3}[0-9]{3,8}")))
        result = true;
    else
        QMessageBox::critical(this, "Falsches Telefonnummerformat!", "Falsches Telefonnummerformat!");
    return result;
}

/**
 * @brief Erlaubt das Suchen von Benutzern
 * @param arg1 Name oder Vorname des zu suchenden Benutzers
 */
void UserManagement::on_lineEditUserSearch_textChanged(const QString & arg1)
{
    m_model->setTable("users");
    m_model->select();
    m_model->setFilter(QString("name LIKE '%%1%' OR firstname LIKE '%%1%'").arg(arg1));
}

/**
 * @brief Leert das Lineedit zum Suchen von Benutzern
 */
void UserManagement::on_buttonClear_clicked()
{
    ui->lineEditUserSearch->clear();
    m_model->select();
}

/**
 * @brief Fügt Benutzer in Datenbank ein. Überprüft vorher, ob Telefon und Emailangaben valide sind.
 */
void UserManagement::on_buttonAdd_clicked()
{
    if(!this->checkTelFormat(ui->lineEditTelephone->text()) || !this->checkMailFormat(ui->lineEditEmail->text()))
        return;

    if(!dbcon::connecttodb())
        return;

    User newUser(ui->lineEditSurName->text(), ui->lineEditfirstName->text(), ui->lineEditEmail->text(), ui->comboBox->currentText().toInt(), ui->lineEditTelephone->text());
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
void UserManagement::on_buttonDel_clicked()
{
    if(!ui->tableView->selectionModel()->hasSelection())
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
