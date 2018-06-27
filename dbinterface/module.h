#ifndef MODULE_H
#define MODULE_H

#include "databaseobject.h"

class Module : public DatabaseObject
{
private:
    QString m_name;
    int m_faculty;

public:

    Module(const int id = -1) : DatabaseObject("modules", id) {}
    Module(const QString & name, const int faculty, const int id = -1) : DatabaseObject("modules", id), m_name(name), m_faculty(faculty) {}

protected:
    DataHash insertData(void) const;
};

#endif // MODULE_H
