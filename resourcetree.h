#ifndef ITEMMANAGEMENTTREE_H
#define ITEMMANAGEMENTTREE_H

#include "dbinterface\resource.h"

#include <QAction>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QColor>
#include <QBrush>

/**
 * @brief Klasse zur Repräsentation von Einträgen im Ressourcenverwaltungsbaum
 */

class ResourceTreeEntry : public QTreeWidgetItem
{
#define RESOURCE_BACKGROUND_COLOR QBrush(QColor(0, 204, 255, 200))
#define CATEGORY_BACKGROUND_COLOR QBrush(QColor(255,255,153, 200))


public:
    /// Typ des Baumeintrages
    enum ItemType { InvalidType = -1, Category = QTreeWidgetItem::UserType, Room = QTreeWidgetItem::UserType + 1, Resource = QTreeWidgetItem::UserType + 2 };

private:
    /// Id des Baumeintrages (also entweder Id der Ressource, der Kategorie oder des Raumes)
    int m_id;

public:
    /**
     * @brief Konstruktor. Erstellt einen neuen Wurzeleintrag (QTreeWidget ist parent)
     * @param id ID des Eintrages
     * @param type Typ des Eintrages (ItemType)
     * @param strings Daten des Eintrages
     * @param parent Elternteil des Eintrages (in diesem Fall der Baum selbst)
     */
    ResourceTreeEntry (int id, ItemType type, const QStringList & strings, QTreeWidget * parent)
        : QTreeWidgetItem(parent, strings, type), m_id(id) { }
    /**
     * @brief Konstruktor. Erstellt einen neuen Kindeintrag (QTreeWidgetItem ist parent)
     * @param id ID des Eintrages
     * @param type Typ des Eintrages (ItemType)
     * @param strings Daten des Eintrages
     * @param parent Elternteil des Eintrages (in diesem Fall ein anderer Eintrag)
     */
    ResourceTreeEntry (int id, ItemType type, const QStringList & strings, QTreeWidgetItem * parent)
        : QTreeWidgetItem(parent, strings, type), m_id(id) { }

    /**
     * @brief Gibt die Id eines Eintrages zurück
     * @return Die gewünschte Id
     */
    int id() const { return m_id; }
    /**
     * @brief Gibt ein parent-Eintrag zurück, gecasted als ResourceTreeEntry
     * @return Ist der ResourceTreeEntry ein Root Item wird NULL zurück gegeben, ansonsten den parent-Eintrag
     */
    ResourceTreeEntry * parent() { return static_cast<ResourceTreeEntry*>(QTreeWidgetItem::parent()); }
};

/**
 * @brief Ressourcenbaum zur Darstellung und Veränderung von Ressourcen
 */

class ResourceTree : public QTreeWidget
{
    Q_OBJECT

private:
    ///  Enthält die aktuell dargestellte Wurzel des Baumes (Category oder Room)
    ResourceTreeEntry::ItemType m_entryRootType;
    /// Enthält alle z.Zt. aufgeklappten Entries
    QList<int> expandedParents;

    QAction * newCategory;
    QAction * newSubCategory;
    QAction * newLocation;
    QAction * newSubLocation;
    QAction * newResource;
    QAction * delCategory;
    QAction * delRoom;
    QAction * delResource;
    QAction * delMultipleResources;
    QAction * updateCategory;
    QAction * updateRoom;
    QAction * updateResource;

protected:

    ResourceTreeEntry *checkForContainerItem(int id,ResourceTreeEntry *parent);
    ResourceTreeEntry * getContainerItem(int id);
    void dropEvent (QDropEvent * event);
    QMimeData * mimeData (const QList<QTreeWidgetItem*> items) const;
    /**
     * @brief Überladen von QTreeWidget::mimeTypes() Gibt eine Liste von unterstützten MimeTypes zurück. Da wir nur mimeData für Drag and Drop zulassen wollen, wird ein neuer,
     * sonst unbekannter Typ übergeben, der nur zum Bearbeiten unserer Daten verwendet wird
     * @return Unterstützte MimeTypes als QStringList
     */
    QStringList mimeTypes (void) const { return QStringList() << "application/x-itemmodeldatalist"; }

    void insertRecursive(ResourceTreeEntry *parent, int type, int id);

public:
    explicit ResourceTree(QWidget *parent = 0);

    ResourceTreeEntry * currentItem(ResourceTreeEntry::ItemType type = ResourceTreeEntry::InvalidType) const;
    void insertEntries(ResourceTreeEntry * entry, int type, int id);
    void getExpandedParents(void);
    void pushExpandedParent(int parentid);
    void clearExpandedParents(void);
    bool expandedParentsContains(int value) const;

signals:
    /**
     * @brief Wird emitiert, wenn im Baum eine Kategorie ausgewählt wurde
     * @param ID der Kategorie
     */
    void categorySelected(int id);
    /**
     * @brief Wird emitiert, wenn im Baum eine Ressource ausgewählt wurde
     * @param ID der Ressource
     */
    void itemSelected(int id);
    /**
     * @brief Wird emitiert, wenn im Baum ein Raum ausgewählt wurde
     * @param ID des Raumes
     */
    void roomSelected(int id);
    /**
     * @brief Wird emitiert, wenn Auswahl aufgehoben wurde
     */
    void allDeSelected();
    /**
      * @brief Wenn ein Item eingefügt wurde
      */
    void itemInserted(ResourceTreeEntry * item);

public slots:
    void updateTree(int type, int id, ResourceTreeEntry::ItemType rootType = ResourceTreeEntry::InvalidType );
    void updateTree(const QString & searchtext, const QList<SearchFieldDelegate> & searchfields, int type, int id,  ResourceTreeEntry::ItemType rootType );
    void currentChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous);
    void on_customContextMenuRequested(const QPoint&pos);
    void on_itemInserted(ResourceTreeEntry * item);
    void onActionNewCategory(void);
    void onActionNewSubCategory(void);
    void onActionNewLocation(void);
    void onActionNewSubLocation(void);
    void onActionNewResource(void);
    void onActionDelCategory(void);
    void onActionDelRoom(void);
    void onActionDelResource(void);
    void onActionDelMultipleResources(void);
    void onActionUpdateCategory(void);
    void onActionUpdateRoom(void);
    void onActionUpdateResource(void);
    void onAddResourceFinished(int id);
    void onEditResourceFinished(bool result, int id);

signals:
    void treeNeedsUpdate(int type,int id);
};

#endif // ITEMMANAGEMENTTREE_H
