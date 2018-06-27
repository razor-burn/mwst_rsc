#include "rscmanagement.h"
#include "ui_rscmanagement.h"
#include "dbinterface\dbcon.h"
#include "dbinterface\category.h"
#include "dbinterface\resource.h"
#include "dbinterface\databaseobject.h"
#include "dbinterface\location.h"
#include <QStandardItemModel>
#include <QSqlQuery>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QInputDialog>
#include <QMessageBox>
#include <QShowEvent>
#include <QSqlError>

#define CATEGORY_FILTER 0
#define ROOM_FILTER 1

/**
 * @brief Überladen von QDialog::showevent.
 * Wird jedes Mal aufgerufen, wenn der Dialog anzeigt wird.
 * Ruft clear() und updateTree() auf
 * @param event
 */
void RscManagement::showEvent(QShowEvent *event)
{
    this->clear();
    this->updateTree();
    QDialog::showEvent(event);
}

/**
 * @brief Konstruktor
 * @param parent
 */
RscManagement::RscManagement(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RscManagement)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Close)->setText("Schließen");
}

/**
 * @brief Destruktor
 */
RscManagement::~RscManagement()
{
    delete ui;
}

/**
 * @brief Überprüft, ob der Benutzer einen Suchbegriff eingegeben hat und ruft Entsprechend des Ergebnisses die Methode updateTree von resourcetree.cpp auf
 * @param type Typ des Eintrages, der zuletzt ausgewählt war
 * @param id Id des Eintrages, der zuletzt ausgewählt war
 */
void RscManagement::updateTree(int type, int id)
{
    if (ui->lineEditSearch->text().isEmpty())
    {
        ui->treeWidget->updateTree(type, id, ui->comboBoxSetFilter->currentIndex() == 0 ? ResourceTreeEntry::Category : ResourceTreeEntry::Room);
    }
    else
    {
        QStringList tmpList;
        if(ui->make->isChecked())
            tmpList << ui->make->objectName();
        if(ui->type->isChecked())
            tmpList << ui->type->objectName();
        if(ui->description->isChecked())
            tmpList << ui->description->objectName();
        ui->treeWidget->updateTree(ui->lineEditSearch->text(), tmpList, type, id, ui->comboBoxSetFilter->currentIndex() == 0 ? ResourceTreeEntry::Category : ResourceTreeEntry::Room);
    }
}

/**
 * @brief RscManagement::clear
 * Setzt die Eingabezeile zum Suchen von Ressourcen zurück
 */
void RscManagement::clear()
{
    ui->lineEditSearch->clear();
    this->setupView();
}

/**
 * Aktiviert und deaktiviert die Buttons zum Hinzufügen / Löschen von Ressourcen, Räumen und Kategorien
 * Zeigt die Entsprechende Maske zum Hinzufügen / Löschen / Umbenennen an
 */
void RscManagement::setupView()
{
    int type = ResourceTreeEntry::InvalidType;
    if (ui->treeWidget->currentItem() != NULL)
        type = ui->treeWidget->currentItem()->type();
    int treeFilter = ui->comboBoxSetFilter->currentIndex();

    ui->buttonAddCat->setEnabled(treeFilter == CATEGORY_FILTER);
    ui->buttonDelCat->setEnabled(type == ResourceTreeEntry::Category);
    ui->buttonAddResource->setEnabled(type != ResourceTreeEntry::InvalidType);
    ui->buttonDelResource->setEnabled(type == ResourceTreeEntry::Resource);
    ui->buttonAddRoom->setEnabled(treeFilter == ROOM_FILTER);
    ui->buttonDelRoom->setEnabled(type == ResourceTreeEntry::Room);

    int index = 3;
    switch (type)
    {
        case ResourceTreeEntry::Resource: index = 0; break;
        case ResourceTreeEntry::Category: index = 1; break;
        case ResourceTreeEntry::Room: index = 2; break;
    }

    ui->stackedWidget->setCurrentIndex(index);
}

/**
 * @brief Wird aufgerufen, wenn im Baum eine Kategorie ausgewählt wurde. Zeigt die Infos dieser an.
 * @param cat Id der Kategorie
 */
void RscManagement::on_treeWidget_categorySelected(int cat)
{
    if(!dbcon::connecttodb())
        return;
    this->setupView();
    Category selCat(cat);
    ui->lineEditCatName->setText(selCat.get().value("name").toString());
}

/**
 * @brief Wird aufgerufen, wenn im Baum eine Ressource ausgewählt wurde. Zeigt die Infos dieser an.
 * @param item Id der Kategorie
 */
void RscManagement::on_treeWidget_itemSelected(int item)
{
    if(!dbcon::connecttodb())
        return;

    this->setupView();

    ui->comboBoxMake->clear();
    ui->comboBoxRoom->clear();
    ui->comboBoxCategory->clear();
    ui->comboBoxLocker->clear();

    QSqlQuery allMakes = Resource().getColumn("make",true);
    while(allMakes.next())
        ui->comboBoxMake->addItem(allMakes.value("make").toString(), allMakes.value("make"));

    QSqlQuery allRooms = Room().getAll();
    while(allRooms.next())
        ui->comboBoxRoom->addItem(allRooms.value("name").toString(), allRooms.value("id"));

    QSqlQuery allCategories = Category().getAll();
    while(allCategories.next())
        ui->comboBoxCategory->addItem(allCategories.value("name").toString(), allCategories.value("id"));

    QSqlQuery allLockers = Resource().getColumn("locker", true);
    while(allLockers.next())
        ui->comboBoxLocker->addItem(allLockers.value("locker").toString(), allLockers.value("locker"));

    QSqlQuery allCostLocs = Resource().getColumn("costlocation", true);
    while(allCostLocs.next())
        ui->comboBoxCostLoc->addItem(allCostLocs.value("costlocation").toString(), allCostLocs.value("costlocation").toString());

    Resource res(item);
    QSqlQuery allInfos = res.get();
    ui->comboBoxMake->setCurrentIndex(ui->comboBoxMake->findData(allInfos.value("make")));
    ui->comboBoxRoom->setCurrentIndex(ui->comboBoxRoom->findData(allInfos.value("room")));
    ui->comboBoxCategory->setCurrentIndex(ui->comboBoxCategory->findData(allInfos.value("category")));
    ui->comboBoxLocker->setCurrentIndex(ui->comboBoxLocker->findData(allInfos.value("locker")));
    ui->comboBoxCostLoc->setCurrentIndex(ui->comboBoxCostLoc->findData(allInfos.value("costlocation")));

    ui->lineEditEquipmentnumber->setText(allInfos.value("equipmentnr").toString());

    if(allInfos.value("activedate").isNull())
        ui->dateEditActiveDate->setDate(QDate());
    else
        ui->dateEditActiveDate->setDate(QDate(allInfos.value("activedate").toDate()));

    ui->lineEditType              ->setText(allInfos.value("type").toString());
    ui->textEditDescription       ->setText(allInfos.value("description").toString());
    ui->lineEditSerialnumber      ->setText(allInfos.value("serialnr").toString());
    ui->doubleSpinBoxPurchasePrice->setValue(allInfos.value("purchaseprice").toDouble());
    ui->spinBoxTotalQty           ->setValue(allInfos.value("totalquantity").toInt());
    ui->spinBoxQtyInStock         ->setValue(allInfos.value("quantity").toInt());
    ui->spinBoxMinQty             ->setValue(allInfos.value("minquantity").toInt());
}

/**
 * @brief Ermöglicht das Hinzufügen neuer Kategorien durch den Benutzer.
 * Wird ein leerer Name für die neue Kategorie angegeben: return
 */
void RscManagement::on_buttonAddCat_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Neue Kategorie", "Bitte Name der neuen Kategorie angeben:", QLineEdit::Normal,"", &ok);
    if(text.isEmpty())
    {
        QMessageBox::critical(this, "Ungültige Angabe", "Leere Namen werden nich akzeptiert");
        return;
    }
    else
    {
        if(!dbcon::connecttodb())
            return;
        int newId = Category(text).insert();
        if(newId != -1)
        {
            QMessageBox::information(this, "Erfolgreich", "Kategorie wurde hinzugefügt");
            this->updateTree(ResourceTreeEntry::Category ,newId);
        }
        else
            QMessageBox::critical(this, "Fehler", QSqlDatabase::database().lastError().text());
    }
}

/**
 * @brief Sendet Änderungen an Kategorienamen an die Datenbank
 */
void RscManagement::on_buttonSubmitChangesCat_clicked()
{
    if(!dbcon::connecttodb())
        return;
    ResourceTreeEntry * category = ui->treeWidget->currentItem(ResourceTreeEntry::Category);
    if (!category)
        return;

    if(category->id() == 0)
    {
        QMessageBox::critical(this, "Fehler", "Diese Kategorie kann nicht umbenannt werden");
        ui->lineEditCatName->setText(category->data(0, Qt::DisplayRole).toString());
        return;
    }

    ResourceTreeEntry * selected = ui->treeWidget->currentItem();

    if(Category(ui->lineEditCatName->text(), category->id()).update())
    {
        QMessageBox::information(this, "Erfolgreich", "Kategorie wurde umbenannt");
        emit ui->treeWidget->categorySelected(category->id());
    }
    else
        QMessageBox::critical(this, "Fehler", "Unbekannter Fehler");

    this->updateTree(selected->type(), selected->id());
}

/**
 * @brief Sendet Änderungen an einer Ressource an die Datenbank
 */
void RscManagement::on_buttonSubmitResourceChanges_clicked()
{
    QMessageBox::StandardButton result;
    result = QMessageBox::question(this, "Änderungen übernehmen", "Änderungen bestätigen", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::No)
        return;

    ResourceTreeEntry * selected = ui->treeWidget->currentItem();

    if(!dbcon::connecttodb())
        return;

    if(Resource(selected->id(),
                 ui->lineEditEquipmentnumber    ->text().toInt(),
                 ui->dateEditActiveDate         ->date(),
                 ui->comboBoxCostLoc            ->currentText().toInt(),
                 ui->comboBoxLocker             ->currentText(),
                 ui->comboBoxMake               ->currentText(),
                 ui->lineEditType               ->text(),
                 ui->comboBoxCategory           ->currentData().toInt(),
                 ui->comboBoxRoom               ->currentData().toInt(),
                 ui->textEditDescription        ->toPlainText(),
                 ui->lineEditSerialnumber       ->text(),
                 ui->doubleSpinBoxPurchasePrice ->value(),
                 ui->spinBoxTotalQty            ->value(),
                 ui->spinBoxQtyInStock          ->value(),
                 ui->spinBoxMinQty              ->value())
            .update())
    {
        QMessageBox::information(this, "Änderungen wurden übernommen", "Änderungen wurden übernommen");
        this->updateTree(ResourceTreeEntry::Resource, selected->id());
    }
    else
        QMessageBox::critical(this, "Fehler", QSqlDatabase::database().lastError().text());
}

/**
 * @brief Fügt eine neue Ressource in die Datenbank ein. Je nachdem ob nach Räumen oder nach Kategorien gefiltert ist, wird die neue Ressource
 * direkt einer der beiden zugeordnet.
 */
void RscManagement::on_buttonAddResource_clicked()
{
    if(!dbcon::connecttodb())
        return;

    if(ui->comboBoxSetFilter->currentIndex() == 0)
    {
        ResourceTreeEntry * category = ui->treeWidget->currentItem(ResourceTreeEntry::Category);

        if (!category)
            return;

        int newId = Resource(Category(category->id())).insert();
        if(newId > 0)
        {
            QMessageBox::information(this, "Erfolgreich", "Neue Ressource wurde erfolgreich hinzugefügt");
            this->updateTree(ResourceTreeEntry::Resource, newId);
        }
        else
            QMessageBox::critical(this, "Fehler beim Hinzufügen", QSqlDatabase::database().lastError().text());
    }
    else
    {
        ResourceTreeEntry * room = ui->treeWidget->currentItem(ResourceTreeEntry::Room);

        if (!room)
            return;
        int newId = Resource(Room(room->id())).insert();
        if(newId > 0)
        {
            QMessageBox::information(this, "Erfolgreich", "Neue Ressource wurde erfolgreich hinzugefügt");
            this->updateTree(ResourceTreeEntry::Resource, newId);
        }
        else
            QMessageBox::critical(this, "Fehler beim Hinzufügen", QSqlDatabase::database().lastError().text());
    }
}

/**
 * @brief Löscht eine Ressource aus der Datenbank
 */
void RscManagement::on_buttonDelResource_clicked()
{
    QMessageBox::StandardButton result;
    result = QMessageBox::question(this, "Bestätigen", "Diese Ressource wirklich löschen?", QMessageBox::Yes | QMessageBox::No);

    if(result == QMessageBox::Yes)
    {
        ResourceTreeEntry * resource = ui->treeWidget->currentItem(ResourceTreeEntry::Resource);
        if (!resource)
            return;

        if(!dbcon::connecttodb())
            return;

        int category_id = resource->parent()->id();
        Resource oldResource(resource->id());
        if( oldResource.del() == 0 )
        {
            QMessageBox::information(this,"Erfolgreich", "Ressource wurde erfolgreich gelöscht");
            this->updateTree( ResourceTreeEntry::Category, category_id );
        }
        else
            QMessageBox::critical(this, "Fehler beim Löschen", QSqlDatabase::database().lastError().text());
    }
}

/**
 * @brief Löscht eine Kategorie aus der Datenbank. Wenn der Benutzer versucht die Kategorie "Nicht zugeordnet" zu löschen wird dies verworfen
 */
void RscManagement::on_buttonDelCat_clicked()
{
    QMessageBox::StandardButton result;
    result = QMessageBox::question(this, "Bestätigen", "Diese Kategorie wirklich löschen?", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::No)
        return;

        ResourceTreeEntry * category = ui->treeWidget->currentItem(ResourceTreeEntry::Category);
        if (!category)
            return;

        if(category->id() == 0)
        {
            QMessageBox::critical(this, "Fehler", "Die Kategorie \"Nicht zugeordnet\" kann nicht gelöscht werden");
            return;
        }

        if(!dbcon::connecttodb())
            return;

        Category oldCat(category->id());

        int res = oldCat.del();

        if(res < 0 && QSqlDatabase::database().lastError().type() == QSqlError::StatementError)
        {
            QMessageBox::critical(this, "Fehler", "Kategorie kann nicht gelöscht werden, wenn noch Ressourcen zugeordnet sind");
            return;
        }
        else if(res < 0)
        {
            QMessageBox::critical(this, "Fehler", QSqlDatabase::database().lastError().text());
            return;
        }
        else
        {
            QMessageBox::information(this, "Erfolgreich", "Kategorie wurde erfolgreich gelöscht");
            this->updateTree();
            ui->stackedWidget->setCurrentIndex(3);
        }
}

/**
 * @brief Ruft updateTree() auf. Zeigt keine Infos an.
 */
void RscManagement::on_pushButton_clicked()
{
    this->updateTree();
    ui->stackedWidget->setCurrentIndex(3);
}

/**
 * @brief Ruft updateTree() auf
 * @param arg1: Nicht verwendet
 */
void RscManagement::on_comboBoxSetFilter_currentTextChanged(const QString &/*arg1*/)
{
    this->updateTree();
    this->setupView();
}

/**
 * @brief Wird aufgerufen, wenn im Baum ein Raum ausgewählt wurde. Zeigt die Infos dieses an.
 * @param room Id des Raumes
 */
void RscManagement::on_treeWidget_roomSelected(int room)
{
    if(!dbcon::connecttodb())
        return;
    this->setupView();
    Room selRoom(room);
    ui->lineEditRoomName->setText(selRoom.get().value("name").toString());
}

/**
 * @brief Ruft setupView() auf, wenn kein Item ausgewählt wurde.
 */
void RscManagement::on_treeWidget_allDeSelected()
{
    this->setupView();
}

/**
 * @brief Sendet Änderungen an Raumnamen an die Datenbank
 */
void RscManagement::on_buttonSubmitChangesRoom_clicked()
{
    if(!dbcon::connecttodb())
        return;

    ResourceTreeEntry * room = ui->treeWidget->currentItem(ResourceTreeEntry::Room);

    if (!room)
        return;

    if(room->id() == 0)
    {
        QMessageBox::critical(this, "Fehler", "Dieser Raum kann nicht umbenannt werden");
        ui->lineEditRoomName->setText(room->data(0, Qt::DisplayRole).toString());
        return;
    }

    ResourceTreeEntry * selected = ui->treeWidget->currentItem();

    if(Room(ui->lineEditRoomName->text(), room->id()).update())
    {
        QMessageBox::information(this, "Erfolgreich", "Raum wurde umbenannt");
        this->updateTree(selected->type(), selected->id());
        emit ui->treeWidget->roomSelected(room->id());
    }
    else
        QMessageBox::critical(this, "Fehler", "Ein Fehler ist aufgetreten");
}

/**
 * @brief Ermöglicht das Hinzufügen eines neuen Raumes durch den Benutzer.
 * Wird ein leerer Name für den neuen Raum angegeben: return
 */
void RscManagement::on_buttonAddRoom_clicked()
{
    if(!dbcon::connecttodb())
        return;

    bool ok;
    QString text = QInputDialog::getText(this, "Neue Kategorie", "Bitte Name des Raumes angeben:", QLineEdit::Normal,"", &ok);
    if(text.isEmpty())
    {
        QMessageBox::critical(this, "Ungültige Angabe", "Leere Namen werden nich akzeptiert");
        return;
    }

    int newId = Room(text).insert();
    if(newId > 0)
    {
        this->updateTree(ResourceTreeEntry::Room, newId);
        this->on_treeWidget_roomSelected(newId);
    }
}

/**
 * @brief Löscht einen Raum aus der Datenbank. Wenn der Benutzer versucht den Raum "Nicht zugeordnet" zu löschen wird dies verworfen
 */
void RscManagement::on_buttonDelRoom_clicked()
{
    ResourceTreeEntry * room = ui->treeWidget->currentItem(ResourceTreeEntry::Room);
    if (!room)
        return;

    if(!dbcon::connecttodb())
        return;

    if(room->id() == 0)
    {
        QMessageBox::critical(this, "Fehler", "Der Raum \"Nicht zugeordnet\" kann nicht gelöscht werden");
        return;
    }

    Room delRoom(room->id());

    if(delRoom.del() == 0)
    {
        QMessageBox::information(this, "Erfolgreich", "Raum wurde erfolgreich gelöscht");
        this->on_pushButton_clicked();
        ui->stackedWidget->setCurrentIndex(3);
    }
    else if(delRoom.del() == -2)
        QMessageBox::critical(this, "Fehler", "Der Raum kann nicht gelöscht werden, solange sich noch Ressourcen darin befinden");
    else
        QMessageBox::critical(this, "Fehler", QSqlDatabase::database().lastError().text());
}

/**
 * @brief Ruft on_pushButton_clicked() auf
 */
void RscManagement::on_lineEditSearch_returnPressed()
{
    this->on_pushButton_clicked();
}

/**
 * @brief Überträgt Änderungen an der Gesamtmenge auf die lagernde Menge
 * @param arg1 Neue Menge
 */
void RscManagement::on_spinBoxTotalQty_valueChanged(int arg1)
{
    ui->spinBoxQtyInStock->setValue(ui->spinBoxQtyInStock->value() + arg1 - ui->spinBoxQtyInStock->value());
}
