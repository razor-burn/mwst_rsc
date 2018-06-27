#ifndef USER_H
#define USER_H

#include "databaseobject.h"

/**
 * @brief Klasse zur Repräsentation von Benutzern
 */

class User : public DatabaseObject
{
private:

    /// Nachname
    QString m_name;
    /// Vorname
    QString m_firstName;
    /// Email
    QString m_email;
    /// Fachbereich
    int m_faculty;
    /// Telefon
    QString m_telephone;
    /// Adresse
    QString m_address;

protected:
    DataHash insertData() const;

public:

    /**
     * @brief Konstruktor. Wird benötigt um bestehenden Benutzer zu verändern
     * @param id ID des Benutzers
     */
    User(int id) : DatabaseObject("users",id) { }
    /**
     * @brief Konstruktor. Wird benötigt um neuen Benutzer einzufügen
     * @param name Neuer Name
     * @param firstName Neuer Vorname
     * @param email Neue Email
     * @param faculty Neuer Fachbereich
     * @param telephone Neue Telefonnr
     */
    User(const QString name, const QString firstName, const QString  email, const int faculty, const QString  telephone, const QString address)
        : DatabaseObject("users"), m_name(name), m_firstName(firstName), m_email(email), m_faculty(faculty), m_telephone(telephone), m_address(address) { }
    static QString getUsernameByLoanid(const int id);

};

/**
 * @brief Klasse zur Repräsentation eines Mitarbeiters
 * @todo Noch zu erledigen
 * @warning Eine Warnung
 */

class AdminUser : public DatabaseObject
{
private:
    QString m_userName;

protected:
    DataHash insertData() const;

public:

    /**
     * @brief Konstruktor, wird benötigt um alle Admins zu holen oder um zu löschen
     * @param id ID des Mitarbeiters, default -1 (invalid)
     */
    AdminUser(int id = -1) : DatabaseObject("adminuser",id) {  }
    /**
     * @brief Konstruktor, wird benötigt um neuen Admin hinzuzufügen
     * @param name Name des neuen Admins
     * @param id
     */
    AdminUser(const QString &name,int id = -1) : DatabaseObject("adminuser",id), m_userName(name) {  }
};

class Lecturer : public DatabaseObject
{
private:
    QString m_name;
    QString m_firstname;
    int m_faculty;
protected:
    DataHash insertData(void) const;
public:
    Lecturer(const int id = -1) : DatabaseObject("lecturers", id) { }
    Lecturer(const QString &name, const QString &firstname, const int fac, int id = -1) : DatabaseObject("lecturers", id), m_name(name), m_firstname(firstname), m_faculty(fac) { }
};


#endif // USER_H
