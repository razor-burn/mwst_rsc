#ifndef RSCADDUPDATE_H
#define RSCADDUPDATE_H

#include "dbinterface\resource.h"
#include "dbinterface\location.h"
#include "dbinterface\category.h"
#include "dbinterface\make.h"

#include <QDialog>


namespace Ui {
class rscaddupdate;
}

class rscaddupdate : public QDialog
{
    Q_OBJECT

public:
    explicit rscaddupdate(QWidget *parent = 0, int showBy = 0, Resource resource = Resource(-1));
    explicit rscaddupdate(QWidget *parent = 0, int showBy = 0, Room room = Room(-1));
    explicit rscaddupdate(QWidget *parent = 0, int showBy = 0,Category category = Category(-1));
    void populate();
    void setupTreeViews();
    void enableComboBoxes(int showBy);
    ~rscaddupdate();

private slots:
    void on_buttonUpdateResource_clicked();
    void on_buttonAddResource_clicked();
    void on_buttonAddMake_clicked();
    void on_buttonDelMake_clicked();
    void on_spinBoxTotalQty_valueChanged(int arg1);

signals:
    void addResourceFinished(int id);
    void editResourceFinished(bool result, int id);

private:
    Ui::rscaddupdate *ui;
    int resourceToUpdate;
};

#endif // RSCADDUPDATE_H
