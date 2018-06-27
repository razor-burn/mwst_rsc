#include "dbinterface\user.h"
#include "dbinterface\dbcon.h"

#include <QVariant>

/**
 * @brief Implementiert von DatabaseObject. Gibt alle Daten zum Einfügen in die Datenbank zurück
 * @return Im Erfolgsfall wird true zurück gegeben, ansonsten false.
 */
DataHash User::insertData() const
{
    DataHash data;
    data.insert("name", this->m_name);
    data.insert("firstname", this->m_firstName);
    data.insert("email", this->m_email);
    data.insert("faculty", this->m_faculty);
    data.insert("telephone", this->m_telephone);
    data.insert("address", this->m_address);
    return data;
}

/**
 * @brief Holt einen Benutzernamen anhand der Id einer Ausleihe aus der Datenbank
 * @param id des Benutzers
 * @return Im Erfolgsfall Name des Benutzers als QString, ansonsten ein leerer QString
 */
QString User::getUsernameByLoanid(const int id)
{
    QString ret = "";
    QSqlQuery query(QSqlDatabase::database());
    query.prepare(QString("SELECT name, firstname FROM users U, loans L WHERE L.user = U.id AND L.id = %1").arg(id));
    if(query.exec() && query.next())
        ret = query.value("name").toString() + ", " + query.value("firstname").toString();
    return ret;
}

/**
 * @brief Implementiert von DatabaseObject. Gibt alle Daten eines Adminusers zurück, die in die Datenbank eingefügt werden sollen
 * @return DataHash mit den einzufügenden Daten
 */
DataHash AdminUser::insertData() const
{
    DataHash data;
    data.insert("username", this->m_userName);
    return data;
}

DataHash Lecturer::insertData() const
{
    DataHash data;
    data.insert("name", m_name);
    data.insert("firstname", m_firstname);
    data.insert("faculty", m_faculty);
    return data;
}
