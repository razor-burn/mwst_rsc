#ifndef CATEGORY_H
#define CATEGORY_H
#include "databaseobject.h"
#include <QSqlQuery>

/**
 * @brief Klasse zur Repräsentation von Kategorien
 */

class Category : public DatabaseObject
{
private:
    QString m_name;
    int m_superCategory;
    /// DataHash zum festhalten der gewünschten Änderungen bzgl. Kategorie oder Raum
    DataHash m_update;

protected:
    DataHash insertData() const;
    DataHash updateData() const;

public:
    /**
     * @brief Konstruktor
     * @param id Standardwert ist 0 da die Id nicht immer benötigt wird.
     */
    Category(const int id = 0) : DatabaseObject("categories", id), m_name(QString()), m_superCategory(-1) {}

    /**
     * @brief Konstruktur zum Updaten/Hinzufügen einer Kategorie
     * @param name Name der Kategorie
     * @param Id der Kategorie
     */
    Category(const QString & name,const int id = -1, const int superCat = -1) : DatabaseObject("categories", id), m_name(name), m_superCategory(superCat) {}

    /**
     * @brief Konstruktor für Superkategorie
     * @param name
     * @param id
     * @param superCat
     */
    //Category(const QString & name, const int id, const int superCat = -1) : DatabaseObject("categories", id), m_categoryName(name), m_superCategory(superCat) {}

    /**
     * @brief Verschiebt eine Kategorie in eine andere. Ruft dazu DatabaseObject::update() auf. Wird im ResourceTree benötigt.
     * @param id ID des Kategorie, in die verschoben werden soll
     */
    void moveToCategory(int categoryId) { m_update.insert(CATEGORY_PARENT_COLUMN,categoryId); this->update(); m_update.clear(); }
    bool moveResources(const Category &other);
    static bool hasSubCategory(int cat);
    static int hasSuperCategory(int cat);
    bool isDuplicate();
    static QSqlQuery subCategory(int cat);

    QString name(void) const { return m_name; }
};


#endif // CATEGORY_H
