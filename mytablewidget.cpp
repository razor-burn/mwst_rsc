#include "mytablewidget.h"
#include <QHeaderView>


/**
 * @brief Konstruktor. Wird nicht benötigt. Lediglich m_currentResources wird leer initialisiert
 * @param parent
 */
MyTableWidget::MyTableWidget(QWidget *parent) :
    QTableWidget(parent),
    m_currentResources(QList<int>())

{
}

/**
 * @brief Fügt eine neue Zeile hinzu. Setzt alle Zellen auf nicht editierbar außer der Zelle zum Einstellen der gewünschten Menge
 * @param items: Liste von Strings, die hinzugefügt werden sollen.
 * @return Falls das Einfügen fehlschlug wird -1 zurück gegeben. Sonst wird die ID der Ressource zurück gegeben.
 */
int MyTableWidget::addRow(QList<QString > items)
{
    int res = -1;
    this->insertRow(this->rowCount());

    items.append("1");

    for(int i = 0; i < items.count(); i++)
    {
        QTableWidgetItem * tmpItem = new QTableWidgetItem(items[i]);
        if(i != 10)
            tmpItem->setFlags(Qt::ItemIsEditable);

        if(i == 7)
            res = items[i].toInt();

        this->setItem(this->rowCount() - 1, i, tmpItem);
    }
    return res;
}

/**
 * @brief Hängt die ID einer Ressource an die Liste der auszuleihenden Ressourcen an
 * @param ID der neuen Ressource
 */
void MyTableWidget::appendToResourcesList(const int item)
{
    this->m_currentResources.append(item);
}

/**
 * @brief Überprüft, ob eine Ressourcen ID bereits in der Liste vorhanden ist
 * @param ID der Ressource, die überprüft werden soll
 * @return Wurde die ID gefunden wird false zurück gegeben. Ansonsten true.
 */
bool MyTableWidget::checkResourcesListForEntry(const int entry) const
{
    bool result = false;
    if(m_currentResources.contains(entry))
        result = true;
    return result;
}

/**
 * @brief Löscht alle Einträge der Ressourcen Id Liste
 */
void MyTableWidget::clearResourcesList()
{
    this->m_currentResources.clear();
}

/**
 * @brief Löscht einzelnen Eintrag der Ressourcen Id Liste
 * @param Eintrag, der gelöscht werden soll
 */
void MyTableWidget::removeFromResourcesList(const int item)
{
    if(this->m_currentResources.isEmpty())
        return;
    int i = this->m_currentResources.indexOf(item);
    this->m_currentResources.removeAt(i);
}


