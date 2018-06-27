#include "mainstackedwidget.h"
#include "rscmgmt.h"
#include <QMessageBox>
#include <QDebug>

MainStackedWidget::MainStackedWidget(QWidget *parent) :
    QStackedWidget(parent)
{
    //connect(this, SIGNAL(currentChanged(int)), this, SLOT(currentIndexChanged(int)));


}


void MainStackedWidget::currentIndexChanged(int index)
{

        //setCurrentIndex(index);
}
