#-------------------------------------------------
#
# Project created by QtCreator 2014-05-15T16:20:14
#
#-------------------------------------------------

QT       += core gui sql printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mwst_rsc
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    resourcetree.cpp \
    mytablewidget.cpp \
    pdfgenerator.cpp \
    mainstackedwidget.cpp \
    usermgmt.cpp \
    rscmgmt.cpp \
    loanadd.cpp \
    loanmgmt.cpp \
    options.cpp \
    rscaddupdate.cpp \
    treeviewcombobox.cpp \
    delegates/loanpositiondelegate.cpp \
    delegates/spinboxdelegate.cpp \
    dbinterface/category.cpp \
    dbinterface/databaseobject.cpp \
    dbinterface/dbcon.cpp \
    dbinterface/faculty.cpp \
    dbinterface/loan.cpp \
    dbinterface/make.cpp \
    dbinterface/module.cpp \
    dbinterface/resource.cpp \
    dbinterface/user.cpp \
    dbinterface/location.cpp

HEADERS  += mainwindow.h \
    resourcetree.h \
    mytablewidget.h \
    pdfgenerator.h \
    mainstackedwidget.h \
    usermgmt.h \
    rscmgmt.h \
    loanadd.h \
    loanmgmt.h \
    options.h \
    rscaddupdate.h \
    treeviewcombobox.h \
    faculty.h \
    delegates/loanpositiondelegate.h \
    delegates/spinboxdelegate.h \
    dbinterface/category.h \
    dbinterface/databaseobject.h \
    dbinterface/dbcon.h \
    dbinterface/faculty.h \
    dbinterface/loan.h \
    dbinterface/make.h \
    dbinterface/module.h \
    dbinterface/resource.h \
    dbinterface/user.h \
    dbinterface/location.h

FORMS    += mainwindow.ui \
    usermgmt.ui \
    rscmgmt.ui \
    loanadd.ui \
    loanmgmt.ui \
    options.ui \
    rscaddupdate.ui

OTHER_FILES +=
