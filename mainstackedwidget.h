#ifndef MAINSTACKEDWIDGET_H
#define MAINSTACKEDWIDGET_H


#include <QStackedWidget>
#include "usermgmt.h"

class MainStackedWidget : public QStackedWidget
{
    Q_OBJECT

public:

    enum Mask { MASKMAIN = 0, MASKUSERS = 1, MASKRSCMGMT = 2, MASKADDLOAN = 3, MASKMNGLOANS = 4, MASKOPTIONS = 5 };
    explicit MainStackedWidget(QWidget *parent = 0);

public slots:
    void currentIndexChanged(int index);
};

#endif // MAINSTACKEDWIDGET_H
