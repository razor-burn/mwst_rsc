#include "dbinterface\databaseobject.h"


/**
 * @brief Holt alle Informationen des Objektes aus der entsprechenden Tabelle in der Datenbank.
 * @return Alle Informationen als QSqlQuery
 */
QSqlQuery DatabaseObject::get()
{
    QSqlQuery query(QSqlDatabase::database());
    query.prepare(QString("SELECT * FROM %1 WHERE id = %2")
                  .arg(this->table())
                  .arg(this->id()));

    if (query.exec())
        query.next();

    return query;
}

QSqlQuery DatabaseObject::getMax(const QString &column)
{
    QSqlQuery query(QSqlDatabase::database());
    query.prepare(QString("SELECT MAX(%1) as max FROM %2")
                  .arg(column)
                  .arg(this->table()));
    qDebug() << query.executedQuery();

    if (query.exec())
        query.next();

    return query;
}


/**
 * @brief Fügt Daten in die Datenbank ein. Die Daten werden aus dem DataHash mittels insertData ausgelesen. Daraus wird das query gebaut.
 * Die Tabelle, in die die Daten einzufügen sind, wird durch m_table bestimmt
 * @return Die ID der zuletzt eingefügten Zeile. Wird an manchen Stellen benötigt
 */
int DatabaseObject::insert()
{
    DataHash data = this->insertData();

    if(data.isEmpty())
        return -1;

    QStringList values;
    QStringList keys = data.keys();

    foreach(QString key, keys)
        values << data.value(key).toString();

    QSqlQuery query(QSqlDatabase::database());
    query.prepare(QString("INSERT INTO %1 (%2) VALUES ('%3')")
                  .arg(this->table())
                  .arg(keys.join(','))
                  .arg(values.join("','")));

    qDebug() << query.executedQuery();

    if (!query.exec())
        return -1;

    query.prepare(QString("SELECT MAX(id) FROM %1").arg(this->table()));



    if (!query.exec() || !query.next())
    {
        qDebug() << "asd" << QSqlDatabase::database().lastError().text();
        return -1;
    }


    return query.value(0).toInt();
}


/**
 * @brief Aktualisiert einen Datensatz in der Datenbank. Die Daten werden aus dem DataHash mittels updateData ausgelesen. Daraus wird das query gebaut.
 * Tabelle und ID des zu aktualisierenden Datensatzes werden über m_table und m_id bestimmt.
 * @return Im Erfolgsfall true, andernfalls false
 */
bool DatabaseObject::update()
{
    DataHash data = this->updateData();

    if(data.isEmpty())
        return false;

    QString update_data;
    QStringList keys = data.keys();

    foreach(QString key, keys)
        update_data.append(QString("%1%2 = '%3'")
                           .arg(update_data.isEmpty() ? "" : ",")
                           .arg(key)
                           .arg(data.value(key).toString()));

    QSqlQuery query(QSqlDatabase::database());
    query.prepare(QString("UPDATE %1 SET %2 WHERE id = %3")
                  .arg(this->table())
                  .arg(update_data)
                  .arg(this->id()));
    qDebug() << query.executedQuery();

    if (!query.exec())
    {
        qDebug() << query.lastError().text();
        return false;
    }

    return true;
}


/**
 * @brief Holt alle Objekte der gewünschten Tabelle aus der Datenbank
 * @return Alle Tupel als QSqlQuery
 */
QSqlQuery DatabaseObject::getAll()
{
    QSqlQuery query(QSqlDatabase::database());
    query.prepare(QString("SELECT * FROM %1").arg(this->table()));
    query.exec();
    return query;
}

/**
 * @brief Entfernt ein entsprechendes Objekt aus der Datenbank
 * @return Überprüft, ob beim Löschen ein StatementError auftrat. Falls dies der Fall ist, versucht der Benutzer ein Objekt zu löschen, dem noch
 * noch ander Objekte zugeordnet sind (was durch Integritätsbestimmungen unterbunden wird). In diesem Fall oder wenn ein anderer Fehler auftrat: return false,
 * ansonsten: return true
 */
int DatabaseObject::del()
{
    int result = -1;
    QSqlQuery query(QSqlDatabase::database());
    query.prepare(QString("DELETE FROM %1 WHERE id = %2").arg(this->table()).arg(this->id()));
    qDebug() << query.executedQuery();
    if(query.exec())
        result = 0;
    else if(query.lastError().type() == QSqlError::StatementError)
        result = -2;
    return result;
}

/**
 * @brief Holt eine beliebige Zeile anhand eines beliebigen Identifikators aus der Datenbank
 * @param field Der Identifikator
 * @param value Der Wert, auf den Verglichen werden soll
 * @return Ergebnis als QSqlQuery
 */
QSqlQuery DatabaseObject::getBy(const QString & field, const QVariant &value)
{
    QSqlQuery query(QSqlDatabase::database());
    query.prepare(QString("SELECT * FROM %1 WHERE %2 = '%3'").arg(this->table()).arg(field).arg(value.toString()));
    query.exec();
    return query;
}

/**
 * @brief Holt den Inhalt einer beliebigen Spalte aus einer beliebigen Tabelle
 * @param column Die gewünschte Spalte
 * @param distinct Kann verwendet werden, um im query das Schlüsselwort DISTINCT zu verwenden
 * @return
 */
QSqlQuery DatabaseObject::getColumn(const QString &column, bool distinct)
{
    QSqlQuery query(QSqlDatabase::database());
    QString selQuery = "SELECT";

    if(distinct)
        selQuery.append(" DISTINCT");

    selQuery.append(QString(" %1 FROM %2").arg(column).arg(this->table()));
    query.prepare(selQuery);
    query.exec();
    return query;
}


