#include "dbinterface\resource.h"
#include "dbinterface\dbcon.h"


#define LOAN_POS_QTY_COL 2
#define LOAN_POS_RETQTY_COL 3

/**
 * @brief Implementiert von DatabaseObject. Gibt alle in der Datenbank zu aktualisierenden Daten einer Ressource zurück
 * @return DataHash mit den zu aktualisierenden Daten
 */
DataHash Resource::updateData() const
{
    DataHash data;
    if (m_update.isEmpty())
    {
        data.insert("equipmentnr", m_eqno);
        data.insert("activedate", m_adate);
        data.insert("costlocation", m_costloc);
        data.insert("ivs", m_ivs);
        data.insert("make", m_make);
        data.insert("type", m_type);
        data.insert("category", m_category);
        data.insert("room", m_room);
        data.insert("description", m_descr);
        data.insert("serialnr", m_serialnr);
        data.insert("purchaseprice", m_pp);
        data.insert("totalquantity", m_totalqty);
        data.insert("quantity", m_qty);
        data.insert("minquantity", m_minqty);
    }
    else
        data = m_update;

    return data;
}

/**
 * @brief Implementiert von DatabaseObject. Gibt alle Daten einer Ressource zurück, die zum Hinzufügen in die Datenbank verlangt werden.
 * Da eine neue Ressource immer gleich einem Raum oder einer Kategorie zugeordnet werden kann, wird nur eins von diesen beiden angegeben
 * Alle anderen Werte sind default Werte (bei Strings "" und bei numerics 0)
 * @return DataHash mit den einzufügenden Daten
 */
DataHash Resource::insertData() const
{
    DataHash data;
    if (m_room != -1)
        data.insert("room",m_room);
    if (m_category != -1)
        data.insert("category",m_category);

    data.insert("equipmentnr", m_eqno);
    data.insert("activedate", m_adate);
    data.insert("costlocation", m_costloc);
    data.insert("ivs", m_ivs);
    data.insert("make", m_make);
    data.insert("type", m_type);
    data.insert("description", m_descr);
    data.insert("serialnr", m_serialnr);
    data.insert("purchaseprice", m_pp);
    data.insert("totalquantity", m_totalqty);
    data.insert("quantity", m_qty);
    data.insert("minquantity", m_minqty);

    return data;
}

/**
 * @brief Aktualisiert die lagernde Menge einer Ressource
 * @param resourceid Id der Ressource
 * @param oldq Alte Menge
 * @param newq Neue Menge
 * @param column Spalte. Wird benötigt um zu unterscheiden ob die zu verleihende Menge oder die Retourmenge einer Ausleihe verändert wurde
 * @param type Art der aktualisierung
 * @return Im Erfolgsfall true, ansonsten false
 */
bool Resource::updateQuantity(const int resourceid, const int oldq, const int newq, int column, Resource::QuantityUpdateType type)
{
    bool result = false;
    QSqlQuery query(QSqlDatabase::database());
    QString updateStringTotal = "UPDATE resources SET quantity = ";

    if(type == Resource::ReturnOrIncrease)
    {
        if(column == LOAN_POS_QTY_COL)
            updateStringTotal = updateStringTotal + QString(" quantity + (%1)").arg(oldq-newq);

        if(column == LOAN_POS_RETQTY_COL)        
            updateStringTotal = updateStringTotal + QString(" quantity + (%1)").arg(newq-oldq);
    }
    if(type == Resource::ReturnAll)
        updateStringTotal = updateStringTotal + QString("quantity + %1").arg(oldq-newq);

    if(type == Resource::OnLoanPositionInserted)
        updateStringTotal = updateStringTotal + QString("quantity - %1").arg(newq);

    updateStringTotal = updateStringTotal + QString(" WHERE id = %1").arg(resourceid);
    query.prepare(updateStringTotal);
    if(query.exec())
        result = true;
    return result;
}

/**
 * @brief Sucht Ressourcen nach dem gegebenen Text in den gegebenen Feldern
 * @param searchtext Suchttext als QString
 * @param fields Felder könnnen sein: Hersteller, Typ, Beschreibung
 * @return Ressourcen als QSqlquery. Im Fehlerfall wird ein leerse QSqlQuery zurück gegeben
 */
QSqlQuery Resource::search(const QString &searchtext, const QList<SearchFieldDelegate> &fields)
{
    QString newSearchString = "";
    for (int i = 0; i < fields.count(); i++)
        newSearchString.append((i != 0 ? " OR " : "") + fields[i].toString(searchtext) );

    QSqlQuery query(QSqlDatabase::database());
    if (fields.isEmpty())
        query.prepare(QString("SELECT * FROM resources WHERE " MANUFACTURER_COLUMN " LIKE '%" + searchtext + "%' OR " RESOURCETYPE_COLUMN " LIKE '%" + searchtext+ "%' OR " DESCRIPTION_COLUMN " LIKE '%" + searchtext + "%'"));
    else
        query.prepare("SELECT * FROM resources WHERE " + newSearchString);

    qDebug() << query.executedQuery();

    query.exec();
    return query;
}
