#ifndef FACULTY_H
#define FACULTY_H

#include "databaseobject.h"

class Faculty : public DatabaseObject
{
private:
    QString m_name;
    /// Fachbereiche und Abteilungen werden beide als Faculty

protected:
    DataHash insertData() const;

public:
    Faculty(const int id = 0) : DatabaseObject("faculties", id), m_name(QString())  {}
    Faculty(const int id, QString name) : DatabaseObject("faculties", id), m_name(name) {}
    QString name(void) const {return m_name;}
};

#endif // FACULTY_H
