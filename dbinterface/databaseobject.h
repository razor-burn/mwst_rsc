#ifndef DATABASEOBJECT_H
#define DATABASEOBJECT_H

#include "dbcon.h"
#include <QDateTime>
#include <QString>
#include <QVariant>
#include <QDebug>

///Assoziatives Array für entweder einzufügende zu aktualisierende Daten in der Datenbank. QString hält den Namen des Datenbankfeldes, QVariant die Daten die geschrieben werden sollen.
typedef QHash<QString,QVariant> DataHash;

/**
 * @brief Klasse zum Abstrahieren von Datenbankoperationen
 */
class DatabaseObject
{
private:
    /// Tabellenname, in dem sich Objekte des entsprechenden Typs befinden
    QString m_table;
    /// ID des Objektes (-1 = nicht gesetzt)
    int m_id;

protected:
    /**
     * @brief Virtuelle Methode die dazu dient, die einzufügenden Daten zu holen und in insert() in ein query zu packen. Wird von allen Klassen die von
     * DatabaseObject erben implementiert, da alle diese Klassen Einfügen können müssen.
     * @return Der DataHash mit den einzufügenden Daten
     */
    virtual DataHash insertData() const = 0;
    /**
     * @brief Virtuelle Methode die dazu dienst, zu aktualisierende Daten zu holen und in update() in ein query zu packen. Wird
     * nicht von allen Klassen die von DatabaseObject erben implementiert, da das Aktualisieren von Daten
     * an einigen Stellen über Views realisiert wird
     * @return Der DataHash mit den zu aktualisierenden Daten
     */
    virtual DataHash updateData() const { return DataHash(); }

public:
    DatabaseObject(const QString & table, int id = -1) : m_table(table), m_id(id) {  }

    /**
     * @brief Gibt den gewünschten Tabellennamen zurück
     * @return Den Tabellennamen als QString
     */
    const QString & table(void) const { return m_table; }
    /**
     * @brief Gibt die gewünschte ID zurück
     * @return Die ID als int
     */
    int id(void) const { return m_id; }
    QSqlQuery get();
    QSqlQuery getMax(const QString &column);
    int insert();
    bool update();
    QSqlQuery getAll();
    int del();
    QSqlQuery getBy(const QString &field, const QVariant & value);
    QSqlQuery getColumn(const QString & column, bool distinct);
};


#endif // DATABASEOBJECT_H
