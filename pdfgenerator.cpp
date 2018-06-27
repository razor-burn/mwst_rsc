#include "pdfgenerator.h"
#include <QPdfWriter>
#include <QPrintDialog>
#include <QTextDocument>
#include <QTextCursor>
#include <QMessageBox>
#include <QFileDialog>
#include <QPrinter>
#include "dbinterface\dbcon.h"
#include "dbinterface\user.h"
#include "dbinterface\loan.h"
#include "dbinterface\resource.h"


/**
 * @brief Konstruktor. Holt alle Informationen zur betroffenen Ausleihe aus der Datenbank und schreibt deren Werte in die Member
 * @param Id der betroffenen Ausleihe
 */
PdfGenerator::PdfGenerator(int loanid)
{
    QSqlQuery allPosInfo;
    QSqlQuery itemInfo;

    ResourceLoan currentLoan(loanid);
    LoanPosition allPositions(loanid);

    this->m_allInfo = currentLoan.get();
    if (!m_allInfo.isValid())
        QMessageBox::critical(NULL, "Fehler beim lesen der Daten", m_allInfo.lastError().text());

    allPosInfo = allPositions.getByLoanId();

    this->m_preambel = QString("<html> <body> <h1>"
            "<u>Fachhochschule Frankfurt - FB4 - Abt. AV-Medien</u></h1> &nbsp;"
        "<p>"
            "<u>F&uuml;r den/die BenutzerIn: </u></p>"
        "<p>"
            "%1</p>").arg(User::getUsernameByLoanid(loanid));

    this->m_startAndEnd = QString("<p>"
            "<u>Wurde folgende Ausleihe registriert:</u></p>"
        "<br />"
        "<table width=\"50%\" align=\"center\" border=\"1\" cellpadding=\"1\" cellspacing=\"1\">"
            "<tbody><tr> <td> Beginn</td>"
                        "<td> Ende</td> </tr>"
                "<tr> <td> %1 </td>"
                "<td> %2 </td> </tr> </tbody> </table> <br>").arg(this->m_allInfo.value("start").toDateTime().toString("dd-MM-yyyy hh:mm"))
            .arg(this->m_allInfo.value("end").toDateTime().toString("dd-MM-yyyy hh:mm"));

    this->m_resources = QString("<table width=\"50%\" align=\"center\" border=\"1\" cellpadding=\"1\" cellspacing=\"1\">"
                                  "<tbody> <tr> <th>Hersteller</th> <th>Typ</th> <th>Menge</th>");

    if(this->m_allInfo.value("goesabroad").toInt() == 1)
        this->m_resources = this->m_resources + QString(" <th>Einkaufspreis</th></tr>");
    else
        this->m_resources = this->m_resources + QString(" </tr>");

    while(allPosInfo.next())
    {
        Resource resource(allPosInfo.value("resourceid").toInt());
        itemInfo = resource.get();

        if(this->m_allInfo.value("goesabroad").toInt() == 1)
        {
            this->m_resources = this->m_resources + QString("<tr> <td>%1</td> <td>%2</td> <td>%3</td> <td>%4</td> </tr>")
                    .arg(itemInfo.value("make").toString())
                    .arg(itemInfo.value("type").toString())
                    .arg(allPosInfo.value("quantity").toString())
                    .arg(itemInfo.value("purchaseprice").toString());
        }
        else
        {
            this->m_resources = this->m_resources + QString("<tr> <td>%1</td> <td>%2</td> <td>%3</td> </tr>")
                    .arg(itemInfo.value("make").toString())
                    .arg(itemInfo.value("type").toString())
                    .arg(allPosInfo.value("quantity").toString());
        }
    }

    this->m_resources = this->m_resources + QString(" </tbody> </table> <br>");

    if(this->m_allInfo.value("goesabroad").toInt() == 1)
        this->m_resources = this->m_resources + QString("<br> Die Ausleihende Person bestätigt hiermit, das die Ressourcen wenn diese mit ins Ausland genommen werden &nbsp;"
                                                    "Nichtmehr versichert werden können und daher bei Verlust oder Defekt ersetzt werden müssen &nbsp;"
                                                    "Deshalb wird in den Positionen auf den Einkaufspreis der jeweiligen Ressourcen hingewiesen. <br>");

    this->m_employee =  QString("<p>"
            "<u>Ausleihe erstellt von:</u></p>"
        "<p>"
            "%1</p>").arg(AdminUser(this->m_allInfo.value("adminuser").toInt()).get().value("username").toString());



    this->m_currDate = QString("<p>"
                "<u>Datum:</u></p>"
            "<p>"
                "%1</p> <br>").arg(this->m_allInfo.value("created").toDateTime().toString("dddd, dd MMMM yyyy"));

    this->m_signatures = QString("<div align=\"left\">"
                               "Unterschrift der ausleihenden Person <br> <br>"
                               "____________________</div><br>"
                               "<div align=\"right\">"
                               "Unterschrift des Mitarbeiters <br> <br>"
                               "____________________</div> </body> </html>");
}

/**
 * @brief Öffnen einen "Drucken" Dialog.
 * @return Wurde der Druckjob abgeschickt wird 0 zurück gegeben. Ansonsten -1
 */
int PdfGenerator::printToPrinter()
{
    int result = -1;
    QPrinter * printer = NULL;
    QPrintDialog printDiag(printer);

    if(printDiag.exec() == QDialog::Accepted)
    {
        printer = printDiag.printer();
        *printer << *this;
        result = 0;
    }
    return result;

}

/**
 * @brief Schreibt die Daten aus den Membern in ein PDF File und speichert dieses Auf Festplatte.
 * @return Im Erfolgsfall 0, ansonsten -1
 */
int PdfGenerator::printToPdfFile()
{
    int result = -1;
    QString prefix = QString("Ausleihe Nr %1 - %2")
            .arg(this->m_allInfo.value("id").toString())
            .arg(this->m_allInfo.value("created").toDateTime().toString("dd-MM-yyyy hh-mm"));

    QPdfWriter pdfWriter("C:/" + prefix + ".pdf");
    pdfWriter << *this;
    result = 0;
    return result;
}

/**
 * @brief Überladener Operator der in ein QPagedPaintDevice schreibt. Sowohl beim Ausdrucken als auch
 * beim Drucken in PDF wird ein QPagedPaintDevice benötigt.
 * @param device Das QPagedPaintDevice in das geschrieben werden soll.
 * @param pdf Instanz von pdfwriter, die bereits den gesamten Dokumententext enthält
 * @return Beschriebenes QPagedPaintDevice zum Absenden an Drucker oder Schreiben in PDF
 */
QPagedPaintDevice &operator <<(QPagedPaintDevice &device, const PdfGenerator &pdf)
{
    QTextDocument doc;
    QTextCursor cursor(&doc);
    cursor.insertHtml(pdf.m_preambel + pdf.m_startAndEnd + pdf.m_resources + pdf.m_employee + pdf.m_currDate + pdf.m_signatures);
    doc.print(&device);
    return device;
}
