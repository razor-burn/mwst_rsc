#include "dbinterface\faculty.h"
#include <QVariant>

DataHash Faculty::insertData() const
{
    DataHash data;
    data.insert("id", DatabaseObject::id());
    data.insert("name", m_name);
    return data;
}
