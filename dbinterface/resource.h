#ifndef RESOURCE_H
#define RESOURCE_H
#include "dbcon.h"
#include "databaseobject.h"

#include "location.h"
#include "category.h"


class SearchFieldDelegate
{
    private:
        QString _searchString;

    public:
        SearchFieldDelegate ( const QString & searchString ) : _searchString( searchString ) {  }

        QString toString(const QString & searchtext) const { return QString(_searchString).replace("%s",searchtext); }
};

class TextSearchFieldDelegate : public SearchFieldDelegate
{
    public:
        TextSearchFieldDelegate ( const QString & field )
            : SearchFieldDelegate( field + " LIKE '%%s%'" )
        {}
};

class TableSearchFieldDelegate : public SearchFieldDelegate
{
    public:
        TableSearchFieldDelegate ( const QString & field, const QString & table, const QString & tableSelectField, const QString & tableSearchField )
            : SearchFieldDelegate( field + " = (SELECT " + tableSelectField + " FROM " + table + " WHERE " + tableSearchField + " LIKE '%%s%')" )
        {}
};


/**
 * @brief Klasse zur Repräsentation einer Ressource
 */

class Resource : public DatabaseObject
{

private:
    /// Ausstattungsnummer
    QString m_eqno;
    /// Aktivierungsdatum
    QDate m_adate;
    /// Kostenstelle
    QString m_costloc;
    /// IVS
    QString m_ivs;
    /// Hersteller
    int m_make;
    /// Typ
    QString m_type;
    /// Kategorie
    int m_category;
    /// Raum
    int m_room;
    /// Beschreibung
    QString m_descr;
    /// Seriennummer
    QString m_serialnr;
    /// Einkaufspreis
    double m_pp;
    /// Gesamtmenge
    int m_totalqty;
    /// Z. Zt. vorhandene Menge
    int m_qty;
    /// Minimale Menge
    int m_minqty;
    /// DataHash zum festhalten der gewünschten Änderungen bzgl. Kategorie oder Raum
    DataHash m_update;

protected:

//    bool updateField(const QString & field, const int id);
    DataHash updateData() const;
    DataHash insertData() const;


public:
    /// Beschreibt, nach welchem Muster die lagernde Menge einer Ressource aktualisiert werden muss
    enum QuantityUpdateType { ReturnOrIncrease = 0, ReturnAll = 1, OnLoanPositionInserted = 2 };
    /**
     * @brief Konstruktor. Wird benötigt, um eine bestehende Ressource zu manipulieren.
     * @param id ID der Ressource
     */
    Resource(int id = -1) : DatabaseObject("resources", id), m_room(-1), m_category(-1) { }
    /**
     * @brief Konstruktor. Wird benötigt um eine neue Ressource zu erstellen, die direkt einem Raum zugeordnet werden soll
     * @param room Der gewünschte Raum
     */
    Resource(const Room & room) : DatabaseObject("resources"), m_room(room.id()), m_category(-1) { }
    /**
     * @brief Konstruktor. Wird benötigt um eine neue Ressource zu erstellen, die direkt einer Kategorie zugeordnet werden soll
     * @param category Die gewünschte Kategorie
     */
    Resource(const Category & category) : DatabaseObject("resources"), m_room(-1), m_category(category.id()) { }
    /**
     * @brief Konstruktor. Wird benötigt um eine Ressource zu manipulieren
     * @param id ID der Ressource
     * @param eqno Ausstattungsnummer
     * @param adate Aktivierungsdatum
     * @param costloc Kostenstelle
     * @param ivs IVS Nummer
     * @param locker Schrank
     * @param make Hersteller
     * @param type Typ
     * @param category Kategorie
     * @param room Raum
     * @param descr Beschreibung
     * @param serialnr Seriennummer
     * @param pp Einkaufspreis
     * @param totalqty Gesamtmenge
     * @param qty Lagernde Menge
     * @param minqty Minimale Menge
     */
    Resource(int id, const QString &eqno, QDate adate, const QString &costloc, const QString &ivs, int make,
                    const QString &type, int category, int room, const QString &descr, const QString &serialnr, double pp, int totalqty, int qty, int minqty)
        : DatabaseObject("resources",id), m_eqno(eqno), m_adate(adate), m_costloc(costloc), m_ivs(ivs), m_make(make),
          m_type(type), m_category(category), m_room(room), m_descr(descr), m_serialnr(serialnr), m_pp(pp), m_totalqty(totalqty), m_qty(qty), m_minqty(minqty)
    { }


    static bool updateQuantity(const int resourceid, const int oldq, const int newq, int column, Resource::QuantityUpdateType type);
    /**
     * @brief Verschiebt eine Ressource von einem Raum in den anderen. Ruft dazu DatabaseObject::update() auf. Wird im ResourceTree benötigt.
     * @param id ID des Raumes, in den verschoben werden soll
     */
    void moveToRoom(int roomId) { m_update.insert("room",roomId); this->update(); m_update.clear(); }
    /**
     * @brief Verschiebt eine Ressource von einer Kategorie in eine andere. Ruft dazu DatabaseObject::update() auf. Wird im ResourceTree benötigt.
     * @param id ID des Kategorie, in die verschoben werden soll
     */
    void moveToCategory(int categoryId) { m_update.insert("category",categoryId); this->update(); m_update.clear(); }
    static QSqlQuery search(const QString & searchtext, const QList<SearchFieldDelegate> &fields);
    /**
     * @brief Holt alle Ressourcen die einer gewünschten Kategorie zugeordnet sind. Ist prinzipiell ein Makro das getBy callt
     * @param id ID der Kategorie
     * @return Ergebnis als QSqlQuery. Im Fehlerfall leeres QSqlQuery.
     */
    static QSqlQuery getByCategory(const int id) { return Resource().getBy("category",id); }
    /**
     * @brief Holt alle Ressourcen die einem gewünschten Raum zugeordnet sind. Ist prinzipiell ein Makro das getBy callt.
     * @param id ID des Raumes
     * @return Ergebnis als QSqlQuery. Im Fehlerfall leeres QSqlQuery.
     */
    static QSqlQuery getByRoom(const int id) { return Resource().getBy("room",id); }
};

#endif // RESOURCE_H
