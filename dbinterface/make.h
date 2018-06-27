#ifndef MAKE_H
#define MAKE_H

#include "databaseobject.h"

class Make : public DatabaseObject
{
private:
    QString m_name;

public:
    Make(int id = -1) : DatabaseObject("make", id) {}
    Make(const QString & name, int id = -1) : DatabaseObject("make", id), m_name(name) {}
    QString name(void) const { return this->m_name; }
    bool isDuplicate();

protected:
    DataHash insertData() const;
};

#endif // MAKE_H
