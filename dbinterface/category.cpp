#include "dbinterface\category.h"
#include "dbinterface\dbcon.h"
#include "QVariant"

/**
 * @brief Implementiert von DatabaseObject. Gibt alle in die Datenbank einzufügenden Daten
 * einer Kategorie zurück.
 * @return Kategorie-Info (hier nur der Name) als DataHash
 */
DataHash Category::insertData() const
{
    DataHash data;
    data.insert("name", m_name);
    if(m_superCategory != -1)
        data.insert(CATEGORY_PARENT_COLUMN, m_superCategory);
    else
        data.insert(CATEGORY_PARENT_COLUMN, -1);
    return data;
}

/**
 * @brief Implementiert von DatabaseObject. Gibt alle in der Datenbank zu aktualisierenden Daten
 * einer Kategorie zurück
 * @return Zu manipulierende Daten (hier nur Name) als DataHash
 */
DataHash Category::updateData() const
{
    DataHash data;
    if ( m_update.isEmpty() )
        data.insert("name", m_name);
    else
        data = m_update;

    return data;
}

/**
 * @brief Ordnet Ressourcen einer anderen Kategorie zu
 * @param other Die neue Kategorie die gesetzt werden soll
 * @return Wurde das Datenbankquery korrekt ausgeführt, return true, sonst false
 */
bool Category::moveResources(const Category & other)
{
    bool result = false;
    QSqlQuery query(QSqlDatabase::database());
    query.prepare(QString("UPDATE resources SET category = %1 WHERE category = %2").arg(other.id()).arg(this->id()));
    if (query.exec())
        result = true;
    return result;
}

bool Category::hasSubCategory(int cat)
{
    bool result = false;
    QSqlQuery query(QSqlDatabase::database());
    query.prepare(QString("SELECT * FROM CATEGORIES WHERE %1 = %2").arg(CATEGORY_PARENT_COLUMN).arg(cat));
    if(!query.exec())
        return result;
    if(query.next())
        result = true;
    return result;
}

QSqlQuery Category::subCategory(int cat)
{
    QSqlQuery result(QSqlDatabase::database());
    result.prepare(QString("SELECT * FROM CATEGORIES WHERE %1 = %2").arg(CATEGORY_PARENT_COLUMN).arg(cat));
    if(result.exec())
        result.next();

    return result;
}

int Category::hasSuperCategory(int cat)
{
    QSqlQuery category = Category(cat).get();
    return category.value(CATEGORY_PARENT_COLUMN).toInt();
}

bool Category::isDuplicate()
{
    QSqlQuery allQry = Category().getAll();
    if(!allQry.exec())
        return false;

    while(allQry.next())
        if(QString::compare(this->m_name, allQry.value("name").toString(), Qt::CaseInsensitive) == 0)
            return true;
    return false;
}





