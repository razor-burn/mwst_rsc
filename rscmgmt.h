#ifndef RSCMGMT_H
#define RSCMGMT_H

#include <QWidget>
#include <QStandardItem>
#include <QModelIndex>
#include "resourcetree.h"
#include "dbinterface/make.h"

namespace Ui {
class rscmgmt;
}

class rscmgmt : public QWidget
{
    Q_OBJECT

protected:

    void showEvent(QShowEvent * event);

public:

    explicit rscmgmt(QWidget *parent = 0);
    ~rscmgmt();
    void clear(void);


private slots:
    void updateTree(int type = ResourceTreeEntry::InvalidType, int id = 0 );
    void on_treeWidget_categorySelected(int cat);
    void on_treeWidget_itemSelected(int item);
    void on_treeWidget_roomSelected(int room);
    void on_treeWidget_allDeSelected();
    void on_pushButton_clicked();
    void on_comboBoxSetFilter_currentTextChanged(const QString &arg1);
    void on_lineEditSearch_returnPressed();

private:
    Ui::rscmgmt *ui;
};

#endif // RSCMGMT_H
