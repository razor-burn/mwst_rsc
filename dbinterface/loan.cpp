#include "dbinterface\loan.h"
#include "dbinterface\dbcon.h"
#include "dbinterface\resource.h"

#define LOAN_POS_QTY_COL 2
#define LOAN_POS_RETQTY_COL 3

/**
 * @brief Implementiert von DatabaseObject. Gibt alle Daten für eine einzufügende Raumreservierung
 * zurück
 * @return Einzufügende Daten als DataHash
 */
DataHash RoomLoan::insertData() const
{
    DataHash data;
    data.insert("room", this->m_roomid);
    data.insert("user", this->userid());
    data.insert("start", this->startTime().toString("yyyy-MM-dd hh:mm"));
    data.insert("end", this->endTime().toString("yyyy-MM-dd hh:mm"));
    data.insert("adminuser", this->adminuserid());
    return data;
}

/**
 * @brief Implementiert von DatabaseObject. Gibt alle benötigten Informationen über eine zu
 * ändernde Raumreservierung zurück. (Also Start und Endzeit)
 * @return Informationen als DataHash
 */
DataHash RoomLoan::updateData() const
{
    DataHash data;
    if (this->m_roomid != -1)
        data.insert("room", this->m_roomid);
    if (this->userid() != -1)
        data.insert("user", this->userid());
    if (this->startTime().isValid())
        data.insert("start", this->startTime().toString("yyyy-MM-dd hh:mm"));
    if (this->endTime().isValid())
        data.insert("end", this->endTime().toString("yyyy-MM-dd hh:mm"));
    if (this->adminuserid() != -1)
        data.insert("adminuser", this->adminuserid());
    return data;
}

/**
 * @brief Überprüft, ob ein gewünschter Schnittraum zu einem gewünschten Zeitraum bereits vergeben ist
 * @param start Startzeit
 * @param end Endzeit
 * @return Wurden Überschneidungen gefunden, wird eine QList<int> zurück gegeben, die die IDs der Kollidierenden Reservierungen enthält
 */
QList<int> RoomLoan::checkOverlaps(const QDateTime start, const QDateTime end, int room)
{
    QList<int> list;
    QSqlQuery query(QSqlDatabase::database());
    query.prepare(QString("SELECT id FROM %1 WHERE room = %2 AND"
                          "((start > '%3' AND start < '%4') OR"
                          "(start > '%3' AND end < '%4') OR"
                          "(start = '%3' OR end = '%4') OR"
                          "(start < '%3' AND end > '%4') OR"
                          "(end > '%3' AND end < '%4'))")
                  .arg(this->table())
                  .arg(room != -1 ? room : this->m_roomid)
                  .arg(start.toString("yyyy-MM-dd hh:mm")).arg(end.toString("yyyy-MM-dd hh:mm")));

    if(query.exec())
    {
        while(query.next())
            list << query.value("id").toInt();
    }
    return list;
}

/**
 * @brief Implementiert von DatabaseObject. Gibt alle Informationen über eine einzufügende
 * Ausleihe zurück
 * @return Informationen als DataHash
 */
DataHash ResourceLoan::insertData() const
{
    DataHash data;
    data.insert("user", this->userid());
    data.insert("start", this->startTime().toString("yyyy-MM-dd hh:mm"));
    data.insert("end", this->endTime().toString("yyyy-MM-dd hh:mm"));
    data.insert("created", this->m_created.toString("yyyy-MM-dd hh:mm"));
    data.insert("adminuser", this->adminuserid());
    data.insert("goesabroad", this->m_goesabroad);
    data.insert("isactive", 1);
    return data;
}


/**
 * @brief Verlängert eine Ressourcenausleihe bis zum gewünschten Zeitpunkt
 * @param Die neue Endzeit
 * @return Im Erfolgsfall wird true zurück gegeben, ansonsten false
 */
bool ResourceLoan::extend(QDateTime newEnd)
{
    bool result = false;
    QSqlQuery query(QSqlDatabase::database());
    query.prepare(QString("UPDATE %1 SET end = '%2' WHERE id = %3")
                  .arg(this->table())
                  .arg(newEnd.toString("yyyy-MM-dd hh:mm"))
                  .arg(this->id()));
    if(query.exec())
        result = true;
    return result;
}

/**
 * @brief Implementiert von DatabaseObject. Gibt alle Informationen über eine einzufügende
 * Ausleihposition zurück
 * @return Informationen als DataHash
 */
DataHash LoanPosition::insertData() const
{
    DataHash data;
    data.insert("loanid", m_loanid);
    data.insert("resourceid", m_resourceid);
    data.insert("quantity", m_quantity);
    data.insert("returnedquantity", 0);
    return data;
}


/**
 * @brief Holt alle Positionen einer Ausleihe aus der Datenbank
 * @return Alle Positionen als QSqlQuery
 */

QSqlQuery LoanPosition::getByLoanId()
{
    return this->getBy( "loanid", m_loanid );
}

/**
 * @brief Setter für die Endzeit einer Ausleihe
 * @param value Die neue Endzeit
 */
void Loan::setEnd(const QDateTime &value)
{
    m_end = value;
}

/**
 * @brief Sette für die Startzeit einer Ausleihe
 * @param value Die neue Startzeit
 */
void Loan::setStart(const QDateTime &value)
{
    m_start = value;
}
