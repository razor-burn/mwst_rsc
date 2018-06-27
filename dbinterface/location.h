#ifndef ROOM_H
#define ROOM_H

#include "databaseobject.h"


/**
 * @brief Klasse zur Repräsentation eines Raumes
 */

class Room : public DatabaseObject
{
private:
    /// Name des Raumes
    QString m_roomName;
    int m_parent;
    /// DataHash zum festhalten der gewünschten Änderungen bzgl. Kategorie oder Raum
    DataHash m_update;

protected:
    DataHash insertData() const;
    DataHash updateData() const;

public:
    /**
     * @brief Konstruktor, benötigt zum Löschen
     * @param id zu löschender Raum
     */
    Room(int id = -1) : DatabaseObject("rooms", id) {  }

    /*
     * @brief Konstruktor, benötigt zum Hinzufügen
     * @param roomName Name des hinzuzufügenden Raumes

    Room(const QString & roomName) : DatabaseObject("rooms",-1), m_roomName(roomName) {  }
    */

    /**
     * @brief Konstruktor, benötigt zum Einfügen
     * @param id ID des Raumes
     */
    //Room(const QString & roomName, int id = -1) : DatabaseObject("rooms", id), m_roomName(roomName) {  }
    /**
     * @brief Konstruktor, benötigt damit EditingRoom seinen Tabellennamen an DatabaseObject weitergeben kann
     * @param roomName Name des Schnittraumes
     * @param table Datenbanktabelle
     * @param id Id des Raumes
     */
    Room(const QString & roomName, int id = -1, int parent = -1) : DatabaseObject("rooms", id), m_roomName(roomName), m_parent(parent) {  }

    Room(const QString & roomName, const QString & table, int id = -1) : DatabaseObject(table, id), m_roomName(roomName) {  }
    /**
     * @brief Getter für m_roomName
     * @return Raumname
     */
    const QString roomName() const { return this->m_roomName; }
    /**
     * @brief Verschiebt einen Raum (Schrank, Kiste, ...) in einen Anderen (Raum, Schrank, Kiste, ...). Ruft dazu DatabaseObject::update() auf. Wird im ResourceTree benötigt.
     * @param id ID des Kategorie, in die verschoben werden soll
     */
    void moveToRoom(int roomId) { m_update.insert(ROOM_PARENT_COLUMN,roomId); this->update(); m_update.clear(); }
    bool moveResources(const Room &other);
};

/**
 * @brief Klasse zur Repräsentation eines Schnittraumes
 */
class EditingRoom : public Room
{
public:

    /**
     * @brief Konstruktor. Wird benötigt um einen Schnittraum zu manipulieren.
     * @param Id des Schnittraumes
     */
    EditingRoom(int roomid = -1, const QString &name = QString()) : Room(name,"editingrooms",roomid) {  }
    /**
     * @brief Konstruktor. Wird benötigt um einen Schnittraum einzufügen
     * @param name Name des neuen Schnittraumes
     */
    EditingRoom(const QString &name) : Room(name,"editingrooms") {  }
};

#endif // ROOM_H
