#include "resourcetree.h"
#include "dbinterface\dbcon.h"
#include "dbinterface\resource.h"
#include "dbinterface\location.h"
#include "dbinterface\category.h"
#include "rscaddupdate.h"
#include <QDropEvent>
#include <QMimeData>
#include <iterator>
#include <QTreeWidgetItemIterator>
#include <QInputDialog>
#include <QMenu>

//TODO: einträge ausklappen bei mouseover
// subkategorien
// drag and drop an subkategorien anpassen
// hersteller von sachen in eigene tabelle auslagern oder zumindest auf nummer sicher gehen, dass keine duplikate vorkommen
// hinzufügen von ressourcen in subkategorien

/**
 * @brief Gibt den Root-Eintrag mit der gewünschten id zurück. Ist dieser noch nicht vorhanden, wird er durch insertRootItem() hinzugefügt
 * @param id ID des gewünschten Root-Eintrages
 * @return Zeiger auf den gewünschten Eintrag
 */
ResourceTreeEntry * ResourceTree::checkForContainerItem(int id,ResourceTreeEntry *parent)
{
    for ( int i = 0; i < parent->childCount(); i++ )
        if ( ((ResourceTreeEntry*)parent->child(i))->id() == id )
            return (ResourceTreeEntry *)parent->child(i);
    return NULL;
}

/**
 * @brief Fügt den Root-Eintrag mit id in den Baum ein und gibt einen Zeiger darauf zurück.
 * Die benötigten Daten werden über Room oder Category aus der DB gelesen
 * @param id ID des gewünschten Root-Eintrages
 * @return Zeiger auf den hinzugefügten Eintrag
 */
ResourceTreeEntry *ResourceTree::getContainerItem(int id)
{
    if ( id == -1 )
        return (ResourceTreeEntry *)this->invisibleRootItem();

    QSqlQuery rootQuery;
    if(m_entryRootType == ResourceTreeEntry::Category)
    {
        Category category(id);
        rootQuery = category.get();
    }
    else
    {
        Room room(id);
        rootQuery = room.get();
    }

    if (!rootQuery.isValid())
        return (ResourceTreeEntry *)this->invisibleRootItem();

    ResourceTreeEntry * parent = this->getContainerItem( rootQuery.value(CONTAINER_PARENT_COLUMN).toInt() );

    /* suche in entry nach id */
    ResourceTreeEntry * entry = this->checkForContainerItem(id,parent);

    if ( entry == NULL )
        entry = new ResourceTreeEntry(rootQuery.value("id").toInt(),
                                     m_entryRootType,
                                     QStringList() << rootQuery.value("name").toString(),
                                     parent);


    emit itemInserted(entry);
    return entry;
}

void ResourceTree::insertRecursive(ResourceTreeEntry *parent, int type, int id)
{
    QSqlQuery rootQuery;
    if(m_entryRootType == ResourceTreeEntry::Category)
        rootQuery = Category().getBy(CONTAINER_PARENT_COLUMN, parent == NULL ? -1 : parent->id() );
    else
        rootQuery = Room().getBy(CONTAINER_PARENT_COLUMN, parent == NULL ? -1 : parent->id() );

    while(rootQuery.next())
    {
        ResourceTreeEntry * container = new ResourceTreeEntry(rootQuery.value("id").toInt(),
                                                              m_entryRootType,
                                                              QStringList() << rootQuery.value("name").toString(),
                                                              parent ? parent : this->invisibleRootItem());
        emit itemInserted(container);
        if( type == container->type() && id == container->id() )
            this->setCurrentItem(container);
        if( this->expandedParentsContains(container->id()) )
            container->setExpanded(true);

        this->insertRecursive(container, type, id);
        this->insertEntries(container, type, id);
    }
}

/**
 * @brief Überladen von QTreeWidget::dropEvent. Wird aufgerufen, wenn ein Drag and Drop Objekt fallen gelassen wird.
 * @param event
 */
void ResourceTree::dropEvent(QDropEvent *event)
{
    event->setDropAction(Qt::IgnoreAction);

    ResourceTreeEntry * target = (ResourceTreeEntry*)this->itemAt(event->pos());

    if(!target)
        return;

    if(target->type() == ResourceTreeEntry::Resource)
        target = (ResourceTreeEntry*)target->parent();

    const QMimeData * qMimeData = event->mimeData();
    QByteArray encoded = qMimeData->data(this->mimeTypes().first()); 
    QDataStream stream(&encoded, QIODevice::ReadOnly);

    int targetType, targetId;

    while(!stream.atEnd())
    {
        if(!dbcon::connecttodb())
            return;
			
        int type, id;
        stream >> type >> id;
        
        if(type == ResourceTreeEntry::Resource)
        {
            Resource resource(id);
            if(target->type() == ResourceTreeEntry::Room)
                resource.moveToRoom(target->id());
            else if(target->type() == ResourceTreeEntry::Category)
                resource.moveToCategory(target->id());
        }
        else if(type == ResourceTreeEntry::Category)
        {
            if(id == 0)
                return;
            Category category(id);
//            category.moveResources(target->id());
            category.moveToCategory( target->id() );
        }
        else if (type == ResourceTreeEntry::Room)
        {
            if(id == 0)
                return;
            Room room(id);
//            room.moveResources(target->id());
            room.moveToRoom( target->id() );
        }
        targetType = type;
        targetId = id;
    }
    emit treeNeedsUpdate(targetType,targetId);

//    this->updateTree(ResourceTreeEntry::InvalidType, 0);
}


/**
 * @brief Überladen von QTreeWidget::mimeData. Wandelt eine Liste aus ResourceTreeEntries in QMimeData um. Diese Daten werden benötigt, um beim Drop event bestimmen zu können, was für Daten fallen gelassen wurden.
 *
 * @param items Umzuwandelnde ResourceTreeEntries
 * @return Umgewandelte Daten
 */
QMimeData * ResourceTree::mimeData(const QList<QTreeWidgetItem*> items) const
{
    if (items.isEmpty())
        return NULL;

    QMimeData * qMimeData = new QMimeData();

    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);
    for(int i = 0; i < items.count(); i++)
    {
        ResourceTreeEntry * item = static_cast<ResourceTreeEntry*>(items[i]);
        stream << item->type() << item->id();
    }
    qMimeData->setData(this->mimeTypes().first(), encoded);
    return qMimeData;
}


/**
 * @brief Konstruktor. Verbindet das Signal des Baumes currentItemChanged mit dem Slot currentChanged
 * @param parent
 */
ResourceTree::ResourceTree(QWidget *parent) :
    QTreeWidget(parent),
    m_entryRootType(ResourceTreeEntry::Category)
{
    newCategory = new QAction("Neue Kategorie" ,this);
    newCategory->setStatusTip("Neue Kategorie hinzufügen");
    connect(newCategory, SIGNAL(triggered()), SLOT(onActionNewCategory()));

    newSubCategory = new QAction("Neue Subkategorie", this);
    newSubCategory->setStatusTip("Neue Subkategorie hinzufügen");
    connect(newSubCategory, SIGNAL(triggered()), SLOT(onActionNewSubCategory()));

    newLocation = new QAction("Neuer Ort", this);
    newLocation->setStatusTip("Neuen Ort erstellen");
    connect(newLocation, SIGNAL(triggered()), SLOT(onActionNewLocation()));

    newSubLocation = new QAction("Neuer Unterort", this);
    newSubLocation->setStatusTip("Neuen Unterort erstellen");
    connect(newSubLocation, SIGNAL(triggered()), SLOT(onActionNewSubLocation()));

    newResource = new QAction("Neue Ressource", this);
    newResource->setStatusTip("Neue Resource anlegen");
    connect(newResource, SIGNAL(triggered()), SLOT(onActionNewResource()));

    delCategory = new QAction("Kategorie löschen", this);
    delCategory->setStatusTip("Kategorie löschen");
    connect(delCategory, SIGNAL(triggered()), SLOT(onActionDelCategory()));

    delRoom = new QAction("Ort löschen", this);
    delRoom->setStatusTip("Ort löschen");
    connect(delRoom, SIGNAL(triggered()), SLOT(onActionDelRoom()));

    delResource = new QAction("Resource löschen", this);
    delResource->setStatusTip("Resource löschen");
    connect(delResource, SIGNAL(triggered()), SLOT(onActionDelResource()));

    delMultipleResources = new QAction("Ausgewählte Resourcen löschen", this);
    delMultipleResources->setStatusTip("Ausgewählte Resourcen löschen");
    connect(delMultipleResources, SIGNAL(triggered()), SLOT(onActionDelMultipleResources()));

    updateCategory = new QAction("Kategorie umbenennen", this);
    updateCategory->setStatusTip("Kategorie umbenennen");
    connect(updateCategory, SIGNAL(triggered()), SLOT(onActionUpdateCategory()));

    updateRoom = new QAction("Ort umbenennen", this);
    updateRoom->setStatusTip("Ort umbenennen");
    connect(updateRoom, SIGNAL(triggered()), SLOT(onActionUpdateRoom()));

    updateResource = new QAction("Resource ändern", this);
    updateResource->setStatusTip("Resource ändern");
    connect(updateResource, SIGNAL(triggered()), SLOT(onActionUpdateResource()));



    this->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(itemInserted(ResourceTreeEntry*)), SLOT(on_itemInserted(ResourceTreeEntry*)));
    connect(this,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            SLOT(currentChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(on_customContextMenuRequested(QPoint)));
}

void ResourceTree::onActionNewCategory()
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
            emit treeNeedsUpdate(ResourceTreeEntry::Category,newId);
//            this->updateTree(ResourceTreeEntry::Category ,newId);
        }
        else
            QMessageBox::critical(this, "Fehler", QSqlDatabase::database().lastError().text());
    }
}

void ResourceTree::onActionNewSubCategory()
{
    ResourceTreeEntry * category = this->currentItem(ResourceTreeEntry::Category);
    if (!category)
        return;

    if(!dbcon::connecttodb())
        return;

    bool ok;
    QString text = QInputDialog::getText(this, "Neue Subkategorie", "Bitte Name angeben:", QLineEdit::Normal,"", &ok);
    if(text.isEmpty())
    {
        QMessageBox::critical(this, "Ungültige Angabe", "Leere Namen werden nich akzeptiert");
        return;
    }
    int newId = Category(text, 0, category->id()).insert();
    if(newId > 0)
        emit treeNeedsUpdate(ResourceTreeEntry::Category,newId);
//        this->updateTree(ResourceTreeEntry::Category, newId);
    else
        QMessageBox::critical(this, "Fehler", dbcon::lastError());
}

void ResourceTree::onActionNewSubLocation()
{
    ResourceTreeEntry * location = this->currentItem(ResourceTreeEntry::Room);
    if (!location)
        return;

    if(!dbcon::connecttodb())
        return;

    bool ok;
    QString text = QInputDialog::getText(this, "Neuer Unterort", "Bitte Name angeben:", QLineEdit::Normal,"", &ok);
    if(text.isEmpty())
    {
        QMessageBox::critical(this, "Ungültige Angabe", "Leere Namen werden nich akzeptiert");
        return;
    }
    int newId = Room(text, 0, location->id()).insert();
    if(newId > 0)
        emit treeNeedsUpdate(ResourceTreeEntry::Room,newId);
//        this->updateTree(ResourceTreeEntry::Category, newId);
    else
        QMessageBox::critical(this, "Fehler", dbcon::lastError());
}

void ResourceTree::onActionNewLocation()
{
    if(!dbcon::connecttodb())
        return;

    bool ok;
    QString text = QInputDialog::getText(this, "Neuer Ort", "Bitte Name des Ortes angeben:", QLineEdit::Normal,"", &ok);
    if(text.isEmpty())
    {
        QMessageBox::critical(this, "Ungültige Angabe", "Leere Namen werden nich akzeptiert");
        return;
    }

    int newId = Room(text).insert();
    if(newId > 0)
        emit treeNeedsUpdate(ResourceTreeEntry::Room,newId);
//        this->updateTree(ResourceTreeEntry::Room, newId);
}

void ResourceTree::onActionNewResource()
{
    rscaddupdate * dialog;

    if(this->currentItem()->type() == ResourceTreeEntry::Category)
        dialog = new rscaddupdate(0, m_entryRootType, Category(this->currentItem()->id()));

    else if(this->currentItem()->type() == ResourceTreeEntry::Room)
        dialog = new rscaddupdate(0, m_entryRootType, Room(this->currentItem()->id()));

    connect(dialog, SIGNAL(addResourceFinished(int)), this, SLOT(onAddResourceFinished(int)));
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setModal(true);
    dialog->exec();
}

void ResourceTree::onActionDelCategory()
{
    QMessageBox::StandardButton result;
    result = QMessageBox::question(this, "Bestätigen", "Diese Kategorie wirklich löschen?", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::No)
        return;

        ResourceTreeEntry * category = this->currentItem(ResourceTreeEntry::Category);
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
            emit treeNeedsUpdate(0,0);
//            this->updateTree(0,0);
        }
}

void ResourceTree::onActionDelRoom()
{
    QMessageBox::StandardButton result;
    result = QMessageBox::question(this, "Bestätigen", "Diesen Ort wirklich löschen?", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::No)
        return;

    ResourceTreeEntry * room = this->currentItem(ResourceTreeEntry::Room);
    if (!room)
        return;

    if(!dbcon::connecttodb())
        return;

    if(room->id() == 0)
    {
        QMessageBox::critical(this, "Fehler", "Der Ort \"Nicht zugeordnet\" kann nicht gelöscht werden");
        return;
    }

    Room delRoom(room->id());

    if(delRoom.del() == 0)
    {
        QMessageBox::information(this, "Erfolgreich", "Ort wurde erfolgreich gelöscht");
        emit treeNeedsUpdate(0,0);
//        this->updateTree(0,0);
    }
    else if(delRoom.del() == -2)
        QMessageBox::critical(this, "Fehler", "Der Ort kann nicht gelöscht werden, solange sich noch Ressourcen darin befinden");
    else
        QMessageBox::critical(this, "Fehler", QSqlDatabase::database().lastError().text());
}

void ResourceTree::onActionDelResource(void)
{
    QMessageBox::StandardButton result;
    result = QMessageBox::question(this, "Bestätigen", "Diese Ressource wirklich löschen?", QMessageBox::Yes | QMessageBox::No);

    if(result == QMessageBox::Yes)
    {
        ResourceTreeEntry * resource = this->currentItem(ResourceTreeEntry::Resource);
        if (!resource)
            return;

        if(!dbcon::connecttodb())
            return;

        int category_id = resource->parent()->id();
        Resource oldResource(resource->id());
        if( oldResource.del() == 0 )
        {
            QMessageBox::information(this,"Erfolgreich", "Ressource wurde erfolgreich gelöscht");

            emit treeNeedsUpdate(ResourceTreeEntry::Category, category_id );
//            this->updateTree( ResourceTreeEntry::Category, category_id );
        }
        else
            QMessageBox::critical(this, "Fehler beim Löschen", QSqlDatabase::database().lastError().text());
    }
}

void ResourceTree::onActionDelMultipleResources()
{
    QMessageBox::StandardButton result;
    result = QMessageBox::question(this, "Bestätigen", "Diese Ressourcen wirklich löschen?", QMessageBox::Yes | QMessageBox::No);

    if(result == QMessageBox::Yes)
    {
        int i, type, parent;
        QList<ResourceTreeEntry *> list;
        QList<QTreeWidgetItem *> selItems = this->selectedItems();

        for(i = 0; i < selItems.count(); i++)
            list << static_cast<ResourceTreeEntry *>(selItems[i]);

        type = list[i-1]->parent()->type();
        parent = list[i-1]->parent()->id();

        if(!dbcon::connecttodb())
            return;

        for(int j = 0; j < list.count(); j++)
            Resource(list[j]->id()).del();

        QMessageBox::information(this, "Löschen erfolgreich" , "Löschen erfolgreich");

        emit treeNeedsUpdate(type,parent);
//        this->updateTree(type,parent);
    }
}



void ResourceTree::onActionUpdateCategory()
{
    ResourceTreeEntry * resource = this->currentItem(ResourceTreeEntry::Category);
    if (!resource)
        return;

    if(!dbcon::connecttodb())
        return;

    bool ok;
    QString text = QInputDialog::getText(this, "Neuen Namen eingeben", "Bitte den neuen Namen angeben", QLineEdit::Normal,"", &ok);
    if(text.isEmpty())
    {
        QMessageBox::critical(this, "Ungültige Angabe", "Leere Namen werden nich akzeptiert");
        return;
    }

    if(Category(text, resource->id()).update())
        emit treeNeedsUpdate(ResourceTreeEntry::Category,resource->id());
//        this->updateTree(ResourceTreeEntry::Category, resource->id());
}

void ResourceTree::onActionUpdateRoom()
{
    ResourceTreeEntry * room = this->currentItem(ResourceTreeEntry::Room);
    if (!room)
        return;

    if(room->id() == 0)
    {
        QMessageBox::critical(this, "Fehler", "Dieser Ort kann nicht umbenannt werden");
        return;
    }

    if(!dbcon::connecttodb())
        return;

    bool ok;
    QString text = QInputDialog::getText(this, "Neuen Namen eingeben", "Bitte den neuen Namen angeben", QLineEdit::Normal,"", &ok);
    if(text.isEmpty())
    {
        QMessageBox::critical(this, "Ungültige Angabe", "Leere Namen werden nich akzeptiert");
        return;
    }

    if(Room(text, room->id()).update())
        emit treeNeedsUpdate(ResourceTreeEntry::Room, room->id());
//        this->updateTree(ResourceTreeEntry::Room, room->id());
}

void ResourceTree::onActionUpdateResource()
{
    ResourceTreeEntry * item = static_cast<ResourceTreeEntry*>(QTreeWidget::currentItem());

    if(item->type() != ResourceTreeEntry::Resource)
        return;

    rscaddupdate * dialog;
    dialog = new rscaddupdate(0, m_entryRootType, Resource(item->id()));
    connect(dialog, SIGNAL(editResourceFinished(bool,int)), this, SLOT(onEditResourceFinished(bool,int)));
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setModal(true);
    dialog->show();
}

void ResourceTree::onAddResourceFinished(int id)
{
    if(id == -1)
    {
        QMessageBox::information(this, "Fehler beim Hinzufügen", dbcon::lastError());
            return;
    }

    emit treeNeedsUpdate(ResourceTreeEntry::Resource, id);
//    this->updateTree(ResourceTreeEntry::Resource, id);
}

void ResourceTree::onEditResourceFinished(bool result, int id)
{
    if(result == false)
        QMessageBox::critical(this, "Fehler beim aktualisieren der Resource", dbcon::lastError());

    emit treeNeedsUpdate(ResourceTreeEntry::Resource, id);
//    this->updateTree(ResourceTreeEntry::Resource, id);
}


/**
 * @brief Gibt den aktuell ausgewählten Eintrag des Baumes zurück. Type gibt den erwarteten Typ des Eintrages an. Falls eine Ressource ausgewählt ist, aber ein anderer type
 * benötigt wird (Room oder Category), so wird in den Elterneinträgen nach dem entsprechenden Typ gesucht
 * @param type Erwarteter Rückgabetyp des ResourceTreeEntry
 * @return Der gewünschte Eintrag,
 */
ResourceTreeEntry *ResourceTree::currentItem(ResourceTreeEntry::ItemType type) const
{
    ResourceTreeEntry * item = static_cast<ResourceTreeEntry*>(QTreeWidget::currentItem());

    if(type == ResourceTreeEntry::InvalidType)
        return item;

    while(item && item->type() != type)
        item = static_cast<ResourceTreeEntry*>(item->parent());

    return item;
}

void ResourceTree::on_itemInserted(ResourceTreeEntry *item)
{
    if(!item)
        return;

    if(item->type() == ResourceTreeEntry::Category || item->type() == ResourceTreeEntry::Room)
        for(int i = 0; i < item->columnCount(); i++)
                item->setBackground(i, CATEGORY_BACKGROUND_COLOR);

    if(item->type() == ResourceTreeEntry::Resource)
        for(int i = 0; i < item->columnCount(); i++)
                item->setBackground(i, RESOURCE_BACKGROUND_COLOR);
}


void ResourceTree::getExpandedParents(void)
{
    QTreeWidgetItemIterator it(this);
    while(*it)
    {
        if((*it)->isExpanded() == true)
            this->pushExpandedParent(static_cast<ResourceTreeEntry*>(*it)->id());
        ++it;
    }
}



void ResourceTree::insertEntries(ResourceTreeEntry *entry, int type, int id)
{
    if(!dbcon::connecttodb())
        return;

    // Resourcen
    QSqlQuery resourcesQuery;
    if ( m_entryRootType == ResourceTreeEntry::Category )
        resourcesQuery = Resource::getByCategory(entry->id());
    else
        resourcesQuery = Resource::getByRoom(entry->id());

    while(resourcesQuery.next())
    {
        ResourceTreeEntry * resourceEntry = new ResourceTreeEntry(resourcesQuery.value("id").toInt(),
                                                                  ResourceTreeEntry::Resource,
                                                                  QStringList() << Make(resourcesQuery.value("make").toInt()).get().value("name").toString()
                                                                                << resourcesQuery.value("type").toString(),
                                                                  entry);

        emit itemInserted(resourceEntry);
        if(resourceEntry->type() == type && resourceEntry->id() == id)
            this->setCurrentItem(resourceEntry);
    }
}


/**
 * @brief Lädt den Inhalt des Baumes neu nach. Da beim Aktualisieren des Baumes die Informationen über ausgewählte Einträge
 * verloren gehen können type und id gesetzt werden um die vorherige Auswahl wieder herzustellen.
 * rootType gibt an, ob sich die Darstellung (nach Ort oder nach Kategorie) geändert hat.
 * @param type Auszuwählender Typ (InvalidType für keine Auswahl)
 * @param id Auszuwählende ID
 * @param rootType Neue Wurzel des Baumes oder InvalidType für die bisherige Wurzel
 */
void ResourceTree::updateTree(int type, int id,  ResourceTreeEntry::ItemType rootType)
{
    if(rootType == m_entryRootType || rootType == ResourceTreeEntry::InvalidType) // Liste nur erstellen, wenn der Wurzeleintrags-Type gleich bleibt
        this->getExpandedParents();

    // Wenn siche der Objekttyp der Wurzeleinträge geändert hat (Ort->Kategorie / Kategorie->Ort), erfasse dies
    if(rootType == ResourceTreeEntry::Category || rootType == ResourceTreeEntry::Room)
        m_entryRootType = rootType;

    this->clear();

    if(!dbcon::connecttodb())
        return;

    this->insertRecursive(NULL, type, id);

    // Einträge sortieren
    this->sortItems(0, Qt::AscendingOrder);
    this->clearExpandedParents();
}


/**
 * @brief Erweitert updateTree(int, int, ItemType) um die Möglichkeit, nach bestimmten Ressourcen zu suchen. Suchkriterien werden
 * in searchtext und searchfields angegeben
 * @param searchtext Zu suchender Text
 * @param searchfields Datenbankfelder, in denen gesucht werden soll
 * @param type Auszuwählender Typ (InvalidType für keine Auswahl)
 * @param id Auszuwählende ID
 * @param rootType Neue Wurzel des Baumes oder InvalidType für die bisherige Wurzel
 */
void ResourceTree::updateTree(const QString & searchtext, const QList<SearchFieldDelegate> & searchfields, int type, int id,  ResourceTreeEntry::ItemType rootType)
{
    if(rootType == ResourceTreeEntry::Category || rootType == ResourceTreeEntry::Room)
        m_entryRootType = rootType;

    this->clear();

    if(!dbcon::connecttodb())
        return;

    QSqlQuery resourcesQuery = Resource::search(searchtext, searchfields);

    while(resourcesQuery.next())
    {

        ResourceTreeEntry * root = this->getContainerItem(resourcesQuery.value(m_entryRootType==ResourceTreeEntry::Category?"category":"room").toInt());
        if(root == NULL)
        {
            continue;
        }
        if(root->type() == type && root->id() == id)
            this->setCurrentItem(root);
        root->setExpanded(true);
        ResourceTreeEntry * item = new ResourceTreeEntry(resourcesQuery.value("id").toInt(),
                                                          ResourceTreeEntry::Resource,
                                                          QStringList()
                                                          << Make(resourcesQuery.value("make").toInt()).get().value("name").toString()
                                                          << resourcesQuery.value("type").toString(), root);
        emit itemInserted(item);
         if(item->type() == type && item->id() == id)
             this->setCurrentItem(item);
    }
}

/**
 * @brief Wird aufgerufen, wenn sich die aktuelle Auswahl im Baum ändert
 * Emitiert die Signale categorySelected, roomSelected, itemSelected und allDeSelected
 * @param current
 */
void ResourceTree::currentChanged(QTreeWidgetItem *current, QTreeWidgetItem *)
{
    ResourceTreeEntry * item = (ResourceTreeEntry *)current;

    if (item == NULL)
        emit allDeSelected();
    else if (item->type() == ResourceTreeEntry::Category)
        emit categorySelected(item->id());
    else if (item->type() == ResourceTreeEntry::Room)
        emit roomSelected(item->id());
    else
        emit itemSelected(item->id());

    for(int i = 0; i < this->columnCount(); i++)
        this->resizeColumnToContents(i);
}

void ResourceTree::on_customContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);
    ResourceTreeEntry * entry = NULL;
    entry = (ResourceTreeEntry *)this->itemAt(pos);
    if(this->selectedItems().count() > 1)
    {
        QList<ResourceTreeEntry *> list;
        QList<QTreeWidgetItem *> selItems = this->selectedItems();
        for(int i = 0; i < selItems.count(); i++)
            list << static_cast<ResourceTreeEntry *>(selItems[i]);

        for(int j = 0; j < list.count(); j++)
        {
            if(list[j]->type() != ResourceTreeEntry::Resource)
            {
                QMessageBox::critical(this, "Fehler", "Mehrere Dinge können nicht gleichzeitig bearbeitet werden");
                return;
            }
        }
        menu.addAction(delMultipleResources);
        goto END;
    }
    if(!entry)
    {
        if(m_entryRootType == ResourceTreeEntry::Category)
            menu.addAction(newCategory);

        if(m_entryRootType == ResourceTreeEntry::Room)
            menu.addAction(newLocation);
        goto END;
    }

    if(entry->type() == ResourceTreeEntry::Category)
    {
        menu.addAction(newSubCategory);
        menu.addSeparator();
        menu.addAction(updateCategory);
        menu.addAction(delCategory);
        menu.addSeparator();
        menu.addAction(newResource);
    }
    if(entry->type() == ResourceTreeEntry::Room)
    {
        menu.addAction(newSubLocation);
        menu.addSeparator();
        menu.addAction(updateRoom);
        menu.addAction(delRoom);
        menu.addSeparator();
        menu.addAction(newResource);
    }
    if(entry->type() == ResourceTreeEntry::Resource)
    {
        menu.addAction(updateResource);
        menu.addAction(delResource);
    }

END:
    menu.exec(QWidget::mapToGlobal(pos));
}


/**
 * @brief ResourceTree::pushExpandedParent
 * @param parentid
 */
void ResourceTree::pushExpandedParent(int parentid)
{
    this->expandedParents << parentid;
}

void ResourceTree::clearExpandedParents()
{
    this->expandedParents.clear();
}

bool ResourceTree::expandedParentsContains(int value) const
{
    if(this->expandedParents.contains(value))
        return true;
    else
        return false;
}
