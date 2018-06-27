#include "rscmgmt.h"
#include "ui_rscmgmt.h"
#include "dbinterface\dbcon.h"
#include "dbinterface\category.h"
#include "dbinterface\resource.h"
#include "dbinterface\databaseobject.h"
#include "dbinterface\location.h"
#include <QStandardItemModel>
#include <QSqlQuery>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QInputDialog>
#include <QMessageBox>
#include <QShowEvent>
#include <QSqlError>

#define CATEGORY_FILTER 0
#define ROOM_FILTER 1

/**
 * @brief Überladen von QDialog::showevent.
 * Wird jedes Mal aufgerufen, wenn der Dialog anzeigt wird.
 * Ruft clear() und updateTree() auf
 * @param event
 */
void rscmgmt::showEvent(QShowEvent *event)
{
    this->clear();
    this->updateTree();
    QWidget::showEvent(event);
}

/**
 * @brief Konstruktor
 * @param parent
 */
rscmgmt::rscmgmt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::rscmgmt)
{
    ui->setupUi(this);
}

/**
 * @brief Destruktor
 */
rscmgmt::~rscmgmt()
{
    delete ui;
}

/**
 * @brief Überprüft, ob der Benutzer einen Suchbegriff eingegeben hat und ruft Entsprechend des Ergebnisses die Methode updateTree von resourcetree.cpp auf
 * @param type Typ des Eintrages, der zuletzt ausgewählt war
 * @param id Id des Eintrages, der zuletzt ausgewählt war
 */
void rscmgmt::updateTree(int type, int id)
{
    if (ui->lineEditSearch->text().isEmpty())
    {
        ui->treeWidget->updateTree(type, id, ui->comboBoxSetFilter->currentIndex() == 0 ? ResourceTreeEntry::Category : ResourceTreeEntry::Room);
    }
    else
    {
        QList<SearchFieldDelegate> tmpList;
        if ( ui->make->isChecked() )
            tmpList << TableSearchFieldDelegate("make", "make","id","name");
        if(ui->type->isChecked())
            tmpList << TextSearchFieldDelegate("type");
        if(ui->description->isChecked())
            tmpList << TextSearchFieldDelegate("description");
        /*
        QStringList tmpList;
        if(ui->make->isChecked())
            tmpList << ui->make->objectName();
        if(ui->type->isChecked())
            tmpList << ui->type->objectName();
        if(ui->description->isChecked())
            tmpList << ui->description->objectName();
        */
        ui->treeWidget->updateTree(ui->lineEditSearch->text(), tmpList, type, id, ui->comboBoxSetFilter->currentIndex() == 0 ? ResourceTreeEntry::Category : ResourceTreeEntry::Room);
    }
}


/**
 * @brief rscmgmt::clear
 * Setzt die Eingabezeile zum Suchen von Ressourcen zurück
 */
void rscmgmt::clear()
{
    ui->lineEditSearch->clear();
}


/**
 * @brief Wird aufgerufen, wenn im Baum eine Kategorie ausgewählt wurde. Zeigt die Infos dieser an.
 * @param cat Id der Kategorie
 */
void rscmgmt::on_treeWidget_categorySelected(int cat)
{
    ui->stackedWidget->setCurrentIndex(1);
}

/**
 * @brief Wird aufgerufen, wenn im Baum eine Ressource ausgewählt wurde. Zeigt die Infos dieser an.
 * @param item Id der Kategorie
 */
void rscmgmt::on_treeWidget_itemSelected(int item)
{
    if(!dbcon::connecttodb())
        return;

    ui->stackedWidget->setCurrentIndex(0);
    Resource res(item);
    QSqlQuery allInfos = res.get();

    ui->labelEquipmentnumber_out->setText(allInfos.value("equipmentnr").toString());

    if(allInfos.value("activedate").isNull() || !allInfos.value("activedate").toDate().isValid())
        ui->dateEditActiveDate->setDate(QDate());
    else
        ui->dateEditActiveDate->setDate(allInfos.value("activedate").toDate());

    ui->labelCostLocation_out->setText(allInfos.value("costlocation").toString());
    ui->labelIVS_out->setText(allInfos.value("ivs").toString());
    ui->labelRoom_out->setText(Room(allInfos.value("room").toInt()).get().value("name").toString());
    ui->labelCategory_out->setText(Category(allInfos.value("category").toInt()).get().value("name").toString());
    ui->labelMake_out->setText(Make(allInfos.value("make").toInt()).get().value("name").toString());
    ui->labelType_out->setText(allInfos.value("type").toString());
    ui->textEditDescription->setText(allInfos.value("description").toString());
    ui->labelSerialNumber_out->setText(allInfos.value("serialnr").toString());
    ui->labelPurchasePrice_out->setText(allInfos.value("purchaseprice").toString());
    ui->labelTotalQty_out->setText(allInfos.value("totalquantity").toString());
    ui->labelQtyInStock_out->setText(allInfos.value("quantity").toString());
    ui->labelMinQty_out->setText(allInfos.value("minquantity").toString());
}

/**
 * @brief Ruft updateTree() auf. Zeigt keine Infos an.
 */
void rscmgmt::on_pushButton_clicked()
{
    this->updateTree();
    //ui->stackedWidget->setCurrentIndex(3);
}

/**
 * @brief Ruft updateTree() auf
 * @param arg1: Nicht verwendet
 */
void rscmgmt::on_comboBoxSetFilter_currentTextChanged(const QString &/*arg1*/)
{
    this->updateTree();

}

/**
 * @brief Wird aufgerufen, wenn im Baum ein Raum ausgewählt wurde. Zeigt die Infos dieses an.
 * @param room Id des Raumes
 */
void rscmgmt::on_treeWidget_roomSelected(int room)
{
    ui->stackedWidget->setCurrentIndex(1);
}

/**
 * @brief Ruft setupView() auf, wenn kein Item ausgewählt wurde.
 */
void rscmgmt::on_treeWidget_allDeSelected()
{
    ui->stackedWidget->setCurrentIndex(1);
}


/**
 * @brief Ruft on_pushButton_clicked() auf
 */
void rscmgmt::on_lineEditSearch_returnPressed()
{
    this->on_pushButton_clicked();
}



