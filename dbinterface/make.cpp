#include "dbinterface\make.h"




bool Make::isDuplicate()
{
    QSqlQuery allQry = Make().getAll();
    if(!allQry.exec())
        return false;

    while(allQry.next())
        if(QString::compare(this->m_name, allQry.value("name").toString(), Qt::CaseInsensitive) == 0)
            return true;
    return false;
}


DataHash Make::insertData() const
{
    DataHash data;
    data.insert("name", this->m_name);
    return data;
}
