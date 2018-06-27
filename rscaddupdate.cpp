#include "rscaddupdate.h"
#include "ui_rscaddupdate.h"

#include <QInputDialog>
#include <QModelIndexList>

rscaddupdate::rscaddupdate(QWidget *parent, int showBy, Resource resource) :
    QDialog(parent),
    ui(new Ui::rscaddupdate)
{
    ui->setupUi(this);
    this->setWindowTitle("Ressource bearbeiten");
    ui->buttonUpdateResource->setEnabled(true);
    this->populate();
    this->setupTreeViews();

    if(!dbcon::connecttodb())
        return;

    QSqlQuery query = resource.get();

    this->resourceToUpdate = query.value("id").toInt();
    ui->lineEditEquipmentnumber->setText(query.value("equipmentnr").toString());
    ui->dateEditActiveDate->setDate(query.value("activedate").toDate());
    ui->comboBoxCostLocation->setCurrentIndex(ui->comboBoxCostLocation->findText(query.value("costlocation").toString(), Qt::MatchExactly));
    ui->lineEditIVS->setText(query.value("ivs").toString());
    ui->comboBoxLocation->setCurrentIndex(ui->comboBoxLocation->findData(query.value("room").toInt()));
    ui->comboBoxCategory->setIndex(query.value("category").toInt());
    ui->comboBoxLocation->setIndex(query.value("room").toInt());
    ui->comboBoxMake->setCurrentIndex(ui->comboBoxMake->findData(query.value("make").toInt()));
    ui->lineEditType->setText(query.value("type").toString());
    ui->textEditDescription->setText(query.value("description").toString());
    ui->lineEditSerialnumber->setText(query.value("serialnr").toString());
    ui->doubleSpinBoxPurchasePrice->setValue(query.value("purchaseprice").toDouble());
    ui->spinBoxTotalQty->setValue(query.value("totalquantity").toInt());
    ui->spinBoxQtyInStock->setValue(query.value("quantity").toInt());
    ui->spinBoxMinQty->setValue(query.value("minquantity").toInt());
    enableComboBoxes(showBy);
}

rscaddupdate::rscaddupdate(QWidget *parent, int showBy, Room room) :
    QDialog(parent),
    ui(new Ui::rscaddupdate)
{
    ui->setupUi(this);
    this->setWindowTitle("Ressource hinzufügen (Ort)");
    ui->buttonAddResource->setEnabled(true);
    ui->comboBoxLocation->setEnabled(false);
    this->populate();
    this->setupTreeViews();
    ui->comboBoxLocation->setIndex(room.id());
    enableComboBoxes(showBy);
}

rscaddupdate::rscaddupdate(QWidget *parent, int showBy, Category category) :
    QDialog(parent),
    ui(new Ui::rscaddupdate)
{
    ui->setupUi(this);
    this->setWindowTitle("Ressouce hinzufügen (Kategorie)");
    ui->buttonAddResource->setEnabled(true);
    ui->comboBoxCategory->setEnabled(false);
    this->populate();
    this->setupTreeViews();
    ui->comboBoxCategory->setIndex(category.id());
    enableComboBoxes(showBy);
}

void rscaddupdate::setupTreeViews()
{
    ui->comboBoxCategory->init("categories");
    ui->comboBoxLocation->init("rooms");
}

void rscaddupdate::enableComboBoxes(int showBy)
{
    //scheisse Kategorie
    if(showBy == 1000)
    {
        ui->comboBoxCategory->setEnabled(false);
        ui->comboBoxLocation->setEnabled(true);
    }
    // raum
    else if(showBy == 1001)
    {
        ui->comboBoxCategory->setEnabled(true);
        ui->comboBoxLocation->setEnabled(false);
    }
}

void rscaddupdate::populate()
{
    ui->comboBoxCostLocation->clear();
    ui->comboBoxMake->clear();

    if(!dbcon::connecttodb())
        return;

    QSqlQuery allCostLocs = Resource().getColumn("costlocation", true);
    while(allCostLocs.next())
        ui->comboBoxCostLocation->addItem(allCostLocs.value("costlocation").toString(), allCostLocs.value("costlocation").toString());

    ui->comboBoxMake->clear();
    QSqlQuery allMakes = Make().getAll();
    while(allMakes.next())
        ui->comboBoxMake->addItem(allMakes.value("name").toString(), allMakes.value("id"));
}

void rscaddupdate::on_buttonUpdateResource_clicked()
{
    if(ui->lineEditType->text().isEmpty())
    {
        QMessageBox::StandardButton result;
        result = QMessageBox::question(this, "Kein Type angegeben", "Es wurde kein Typ angegeben. Fortfahren?", QMessageBox::Yes | QMessageBox::No);
        if(result == QMessageBox::No)
            return;
    }

    QMessageBox::StandardButton result2;
    result2 = QMessageBox::question(this, "Bestätigen", "Bestätigen", QMessageBox::Yes | QMessageBox::No);

    if(result2 != QMessageBox::Yes)
        return;

    Resource updateResource = Resource(this->resourceToUpdate,
                                       ui->lineEditEquipmentnumber->text(),
                                       ui->dateEditActiveDate->date(),
                                       ui->comboBoxCostLocation->currentText(),
                                       ui->lineEditIVS->text(),
                                       ui->comboBoxMake->currentData().toInt(),
                                       ui->lineEditType->text(),
                                       ui->comboBoxCategory->getCurrentData(),
                                       ui->comboBoxLocation->getCurrentData(),
                                       ui->textEditDescription->toPlainText(),
                                       ui->lineEditSerialnumber->text(),
                                       ui->doubleSpinBoxPurchasePrice->value(),
                                       ui->spinBoxTotalQty->value(),
                                       ui->spinBoxQtyInStock->value(),
                                       ui->spinBoxMinQty->value());

    bool result = updateResource.update();
    emit editResourceFinished(result, this->resourceToUpdate);
    this->close();
}

void rscaddupdate::on_buttonAddResource_clicked()
{
    if(ui->lineEditType->text().isEmpty())
    {
        QMessageBox::StandardButton result;
        result = QMessageBox::question(this, "Kein Type angegeben", "Es wurde kein Typ angegeben. Fortfahren?", QMessageBox::Yes | QMessageBox::No);
        if(result == QMessageBox::No)
            return;
    }

    Resource newResource = Resource(-1,
                                    ui->lineEditEquipmentnumber->text(),
                                    ui->dateEditActiveDate->date(),
                                    ui->comboBoxCostLocation->currentText(),
                                    ui->lineEditIVS->text(),
                                    ui->comboBoxMake->currentData().toInt(),
                                    ui->lineEditType->text(),
                                    ui->comboBoxCategory->getCurrentData(),
                                    ui->comboBoxLocation->getCurrentData(),
                                    ui->textEditDescription->toPlainText(),
                                    ui->lineEditSerialnumber->text(),
                                    ui->doubleSpinBoxPurchasePrice->value(),
                                    ui->spinBoxTotalQty->value(),
                                    ui->spinBoxQtyInStock->value(),
                                    ui->spinBoxMinQty->value());
    int result = newResource.insert();

    if(result == -1)
        QMessageBox::critical(this, "Fehler beim Hinzufügen", QSqlDatabase::database().lastError().text());

    emit addResourceFinished(result);
    this->close();
}

void rscaddupdate::on_buttonAddMake_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Neuer Hersteller", "Bitte Name des neuen Herstellers angeben:", QLineEdit::Normal,"", &ok);
    if(text.isEmpty())
    {
        QMessageBox::critical(this, "Ungültige Angabe", "Leere Namen werden nich akzeptiert");
        return;
    }
    if(Make(text).isDuplicate())
    {
        QMessageBox::critical(this, "Ungültige Angabe", "Namen müssen eindeutig sein!");
        return;
    }

    if(!dbcon::connecttodb())
        return;

    int result = Make(text).insert();
    if(result != -1)
    {
        QMessageBox::information(this, "Erfolgreich", "Hersteller hinzugefügt");
        this->populate();
        ui->comboBoxMake->setCurrentIndex(ui->comboBoxMake->findData(result));
    }
    else
        QMessageBox::critical(this, "Fehler beim Hinzufügen", QSqlDatabase::database().lastError().text());

}

void rscaddupdate::on_buttonDelMake_clicked()
{
    QMessageBox::StandardButton result;
    result = QMessageBox::question(this, "Bestätigen", "Diesen Hersteller löschen?", QMessageBox::Yes | QMessageBox::No);

    if(result == QMessageBox::Yes)
    {
        Make make(ui->comboBoxMake->currentData().toInt());
        if(make.id() == 0)
        {
            QMessageBox::critical(this, "Fehler", "Dieser Hersteller kann nicht gelöscht werden");
            return;
        }
        if(!dbcon::connecttodb())
            return;

        int result = make.del();
        if(result == 0)
        {
            QMessageBox::information(this,"Erfolgreich", "Hersteller wurde erfolgreich gelöscht");
            this->populate();
        }
        else if(result == -2)
            QMessageBox::critical(this, "Kann nicht gelöscht werden", "Dieser Hersteller kann nicht gelöscht werden, da diesem noch Resourcen zugeordnet sind");
        else
            QMessageBox::critical(this, "Fehler beim Löschen", QSqlDatabase::database().lastError().text());
    }
}

void rscaddupdate::on_spinBoxTotalQty_valueChanged(int arg1)
{
    ui->spinBoxQtyInStock->setValue(ui->spinBoxQtyInStock->value() + arg1 - ui->spinBoxQtyInStock->value());
}

rscaddupdate::~rscaddupdate()
{
    delete ui;
}

