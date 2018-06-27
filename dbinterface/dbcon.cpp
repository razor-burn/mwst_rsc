#include "dbcon.h"
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

///Anstatt dbcon jedes Mal wenn es gebraucht wird neu zu instantiieren wird immer diese eine Instanz zurück gegeben
dbcon * dbcon::instance = NULL;

/**
 * @brief Überprüft, ob ein gültiger Pointer auf das Datenbankobjekt existiert
 * Falls nicht, wird ein neues objekt erzeugt
 *
 * Diese Methode garantiert das immer nur genau ein und dasselbe Datenbankobjekt vorhanden ist
 * Es können also keine frei hängenden Verbindungen entstehen
 */
dbcon *dbcon::getInstance()
{
    if (!instance)
        new dbcon();
    return instance;
}

/**
 * @brief Konstruktor wird am Anfang aufgerufen und dann, wenn getInstance feststellt, das kein valider
 * Pointer auf die Datenbankverbindung existiert
 */
dbcon::dbcon()
{
    if(instance)
        return;

    instance = this;
    instance->db = QSqlDatabase::addDatabase("QMYSQL");
    QString tmp;
    QFile file("database.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(0, "Ort der Datenbank konnte nicht gefunden werden", "Bitte stellen Sie sicher, dass sich die Datei"
                              "database.txt im Ordner des Programmes befindet. Diese darf nur die IP Adresse des Datenbankservers enthalten");
        return;
    }
    QTextStream in(&file);
    while(!in.atEnd())
    {
        QString line = in.readLine();
        if(line.startsWith("#"))
            continue;
        tmp = line;
    }
    file.close();
    instance->db.setHostName(tmp);
    instance->db.setDatabaseName("mwst_rsc");
    instance->db.setUserName("root");
    instance->db.setPassword("password");
    instance->db.setConnectOptions("MYSQL_OPT_RECONNECT=1");
}

/**
 * @brief Holt sich den Pointer des Datenbankobjektes und mithilfe dessen die Instanz der Datenbank.
 * getInstance überprüft zuvor, ob ein Pointer auf das DB-Objekt existiert und setzt diesen ggf.
 * Danach wird die open-Methode der Klasse QSqlDatabase aufgerufen.
 * Diese Methode wird an jeder Stelle im Code verwendet, an der eine Datenbankverbindung benötigt wird.
 */
bool dbcon::connecttodb()
{
    QSqlDatabase & db = dbcon::getInstance()->db;

    if (!db.open())
    {
        QMessageBox::critical(NULL, "Datenbankverbindung", db.lastError().text());
        return false;
    }
    return true;
}

QString dbcon::lastError()
{
    return dbcon::getInstance()->db.lastError().text();
}


/**
 * @brief Destruktor
 */
dbcon::~dbcon()
{
    if (db.isOpen())
        db.close();
    if (instance == this)
        instance = NULL;
}


