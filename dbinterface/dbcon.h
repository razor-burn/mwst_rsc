#ifndef DBCON_H
#define DBCON_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

#define MANUFACTURER_COLUMN "make"
#define RESOURCETYPE_COLUMN "type"
#define DESCRIPTION_COLUMN "description"

#define CONTAINER_PARENT_COLUMN "parent"
#define CATEGORY_PARENT_COLUMN (CONTAINER_PARENT_COLUMN)
#define ROOM_PARENT_COLUMN (CONTAINER_PARENT_COLUMN)

/**
 * @brief Klasse zur Verwaltung der Datenbankverbindung
 */
class dbcon
{
private:
    dbcon();
    ~dbcon();
    /// Repräsentiert die Verbindung zur Datenbank
    QSqlDatabase db;
    /// Instanz von dbcon
    static dbcon * instance;
    static dbcon * getInstance();

public:
    static bool connecttodb();
    static QString lastError();
};

#endif // DBCON_H
