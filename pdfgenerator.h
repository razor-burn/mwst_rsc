#ifndef PDFGENERATOR_H
#define PDFGENERATOR_H

#include <QDate>
#include <QSqlQuery>

class QPagedPaintDevice;

/**
 * @brief Klasse zum Erstellen von Dokumente die entweder als PDF geschrieben oder als Druckjob an einen physikalischen Drucker gesendet werden können.
 * Verwendet html Formatierung
 */

class PdfGenerator
{

public:
    PdfGenerator(int loanid);
    int printToPdfFile();
    int printToPrinter();

    friend QPagedPaintDevice & operator << ( QPagedPaintDevice & device, const PdfGenerator & pdf );

private:
    /// Enthält Überschrift und Benutzername
    QString m_preambel;
    /// Enthält Start und Endzeit
    QString m_startAndEnd;
    /// Enthält alle enthaltenen Ressourcen
    QString m_resources;
    /// Mitarbeiter, der die Ausleihe herausgegeben hat
    QString m_employee;
    /// Aktuelles Datum
    QString m_currDate;
    /// Platzhalter für Unterschriften
    QString m_signatures;
    /// Wird verwendet um alle Informationen über eine Ausleihe zu holen
    QSqlQuery m_allInfo;
};

QPagedPaintDevice & operator << ( QPagedPaintDevice & device, const PdfGenerator & pdf );

#endif // PDFGENERATOR_H
