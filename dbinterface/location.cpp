#include "dbinterface\location.h"
#include <QVariant>

/**
 * @brief Implementiert von DatabaseObject. Gibt alle Daten zum Einfügen in die Datenbank zurück
 * @return Hash mit den einzufügenden Daten
 */
DataHash Room::insertData() const
{
    DataHash data;
    data.insert("name", m_roomName);
    if(m_parent != -1)
        data.insert(ROOM_PARENT_COLUMN, m_parent);
    else
        data.insert(ROOM_PARENT_COLUMN, -1);
    return data;
}

/**
 * @brief Implementiert von DatabaseObject. Gibt alle Daten zum Aktualisieren eines Raumes in der Datenbank zurück.
 * @return Hash mit den zu aktualisierenden Daten. Im Falle von Room ist das nur der Name
 */
DataHash Room::updateData() const
{
    DataHash data;
    if ( m_update.isEmpty() )
        data.insert("name", m_roomName);
    else
        data = m_update;

    return data;
}

/**
 * @brief Verschiebt Ressourcen von einem Raum in einen anderen. Wird benötigt im ResourceTree
 * @param other Der Raum, in den verschoben werden soll
 * @return Im Erfolgsfall true, ansonsten false
 */
bool Room::moveResources(const Room & other)
{
    QSqlQuery query(QSqlDatabase::database());
    query.prepare(QString("UPDATE resources SET room = %1 WHERE room = %2").arg(other.id()).arg(this->id()));
    if (!query.exec())
        return false;
    return true;
}
