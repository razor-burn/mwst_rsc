#ifndef LOAN_H
#define LOAN_H
#include <QDateTime>
#include <QtCore>
#include <QSqlQuery>

#include "databaseobject.h"

/**
 * @brief Klasse zur Repräsentation von Ausleihen.
 * Basisklasse bildet Start und m_endzeit ab, da sich alle Ausleihen dies gemeinsam haben
 */
class Loan : public DatabaseObject
{

private:
    /// Beginn einer Ausleihe
    QDateTime m_start;
    /// m_ende einer Ausleihe
    QDateTime m_end;
    /// Ausleihender Benutzer
    int m_userid;
    /// Mitarbeiter
    int m_adminuserid;

public:
    /**
     * @brief Konstruktor
     * @param m_start m_startzeit
     * @param m_end m_endzeit
     */
    Loan(const QString & table, const QDateTime start = QDateTime(),
         const QDateTime end = QDateTime(), int userid = -1, int adminuserid = -1)
        : DatabaseObject(table), m_start(start), m_end(end),
         m_userid(userid), m_adminuserid(adminuserid) { }
    /**
     * @brief Konstruktor
     * @param table Tabellenname (in diesem Fall "loans")
     * @param id ID der Ausleihe
     * @param m_start m_startzeit
     * @param m_end m_endzeit
     */
    Loan(const QString & table, int id, const QDateTime start = QDateTime(),
         const QDateTime end = QDateTime(), int user = -1, int adminuser = -1)
        : DatabaseObject(table,id), m_start(start), m_end(end), m_userid(user), m_adminuserid(adminuser) { }

    const QDateTime & startTime() const { return m_start; }
    const QDateTime & endTime() const { return m_end; }
    const int userid() const { return m_userid; }
    const int adminuserid() const { return m_adminuserid; }
    void setStart(const QDateTime & value);
    void setEnd(const QDateTime & value);
};

/**
 * @brief Klasse zur Repräsentation von Raumausleihen.
 * Erbt von Loan
 */
class RoomLoan : public Loan
{

private:

    /// ID des Raumes, für den die Reservierung gilt
    int m_roomid;
   /* /// ID des Benutzers, für den die Reservierung gilt
    int m_userid;
    /// ID des Mitarbeiters, der die Ausleihe registriert hat
    int m_adminuserid;*/

protected:
    DataHash insertData() const;
    DataHash updateData() const;

public:

    /**
     * @brief Konstruktor. Wird benötigt um eine bestehm_ende Ausleihe zu manipulieren
     * @param id ID der bestehm_enden Ausleihe
     */
    RoomLoan(int id = -1) : Loan("editingroomloans",id), m_roomid(-1) /*m_userid(-1), m_adminuserid(-1)*/  { }
    /**
     * @brief Konstruktor. Wird benötigt um eine neue Ausleihe zu erfassen und in die Datenbank zu schreiben
     * @param roomid ID des Raumes
     * @param userid des Benutzers
     * @param start startzeit
     * @param end endzeit
     * @param adminuserid ID des Mitarbeiters
     */
    RoomLoan(int roomid, int userid, QDateTime start, QDateTime end, int adminuserid)
        : Loan("editingroomloans", start, end, userid, adminuserid), m_roomid(roomid)
        /*, m_userid(userid), m_adminuserid(adminuserid) */ { }
    QList<int> checkOverlaps(const QDateTime start, const QDateTime end, int room = -1);
};

/**
 * @brief Klasse zur Repräsentation von Ressourcenausleihen
 */

class ResourceLoan : public Loan
{
private:

    /// ID des Benutzers, für den die Reservierung gilt
   // int m_userid;
    /// Datum und Uhrzeit der Erstellung der Ausleihe
    QDateTime m_created;
    /// ID des Mitarbeiters, der die Ausleihe registriert hat
    //int m_adminuser;
    /// Flag das bestimmt, ob die Ressourcen einer Ausleihe ins Ausland gehen
    int m_goesabroad;
    /// Flag das bestimmt, ob die Ausleihe aktiv ist
    int m_isactive;

protected:
    DataHash insertData() const;

public:

    /**
     * @brief Konstruktor. Wird benötigt um eine bestehm_ende Ausleihe zu manipulieren
     * @param ID der bestehm_enden Ausleihe
     */
    ResourceLoan(int id = 0) : Loan("loans",id) { }
    /**
     * @brief Konstruktor. Wird benötigt um eine neue Ausleihe zu erfassen und in die Datenbank zu schreiben
     * @param userid ID des Benutzers
     * @param m_start m_startzeit
     * @param m_end m_endzeit
     * @param created Datum und Uhrzeit der Erstellung der Ausleihe
     * @param adminuserid ID des Mitarbeiters, der die Ausleihe registriert hat
     * @param goesabroad Flag das bestimmt, ob die Ressourcen einer Ausleihe ins Ausland gehen
     * @param isactive Flag das bestimmt, ob die Ausleihe aktiv ist
     */
    ResourceLoan(int userid, const QDateTime & start, const QDateTime & end, const QDateTime & created, int adminuserid
                 , int goesabroad, int isactive)
        : Loan("loans", start, end, userid, adminuserid)
        //, m_userid(userid)
        , m_created(created)
     //, m_adminuser(adminuserid)
        , m_goesabroad(goesabroad)
        , m_isactive(isactive) { }

    bool extend(QDateTime newEnd);
};

/**
 * @brief Klasse zur Repräsentation von Ausleihpositionen
 */

class LoanPosition : public DatabaseObject
{
private:

    /// ID der Ausleihe, die die referenzierte Position enthält
    int m_loanid;
    /// ID der Ressource, die in der Position enthalten ist
    int m_resourceid;
    /// Menge der Ressource der Position
    int m_quantity;
    /// Zurückgegebene Menge
    int m_returnedQuantity;

protected:
    DataHash insertData() const;

public:

    /**
     * @brief Konstruktor. Wird benötigt um eine bestehm_ende Position zu manipulieren.
     * @param loanId ID der Position
     */
    LoanPosition(int loanId) : DatabaseObject("loanposition"), m_loanid(loanId) {}
    /**
     * @brief Konstruktor. Wird Wird benötigt um eine neue Position zu erfassen und in die Datenbank zu schreiben
     * @param loanid ID der Ausleihe, die die referenzierte Position enthält
     * @param resourceid ID der Ressource, die in der Position enthalten ist
     * @param quantity Menge der Ressource der Position
     * @param returnedQuantity Zurückgegebene Menge
     */
    LoanPosition(const int loanid, const int resourceid, const int quantity, const int returnedQuantity)
        : DatabaseObject("loanposition"), m_loanid(loanid), m_resourceid(resourceid), m_quantity(quantity), m_returnedQuantity(returnedQuantity) {}

    QSqlQuery getByLoanId();

    int resourceId() const { return m_resourceid; }
    int quantity() const { return m_quantity; }
};

#endif // LOAN_H
