#ifndef RSCMANAGEMENT_H
#define RSCMANAGEMENT_H

#include <QDialog>
#include <QStandardItem>
#include <QModelIndex>
#include "resourcetree.h"

namespace Ui {
class RscManagement;
}

class RscManagement : public QDialog
{
    Q_OBJECT

protected:

    void showEvent(QShowEvent * event);

public:
    /**
     * @brief Konstruktor
     * @param parent
     */
    explicit RscManagement(QWidget *parent = 0);
    ~RscManagement();
    void updateTree(int type = ResourceTreeEntry::InvalidType, int id = 0 );
    void clear(void);
    void setupView(void);

private slots:

    void on_treeWidget_categorySelected(int cat);
    void on_treeWidget_itemSelected(int item);
    void on_treeWidget_roomSelected(int room);
    void on_treeWidget_allDeSelected();
    void on_buttonAddCat_clicked();
    void on_buttonSubmitChangesCat_clicked();
    void on_buttonSubmitResourceChanges_clicked();
    void on_buttonAddResource_clicked();
    void on_buttonDelResource_clicked();
    void on_buttonDelCat_clicked();
    void on_pushButton_clicked();
    void on_comboBoxSetFilter_currentTextChanged(const QString &arg1);
    void on_buttonSubmitChangesRoom_clicked();
    void on_buttonAddRoom_clicked();
    void on_buttonDelRoom_clicked();
    void on_lineEditSearch_returnPressed();
    void on_spinBoxTotalQty_valueChanged(int arg1);

private:
    /// Zeiger auf die Oberfläche
    Ui::RscManagement *ui;
};

#endif // RSCMANAGEMENT_H
