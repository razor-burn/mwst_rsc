#include "dbinterface\module.h"



DataHash Module::insertData() const
{
    DataHash data;
    data.insert("name", m_name);
    data.insert("faculty", m_faculty);
    return data;
}
