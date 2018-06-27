#include "mainwindow.h"
#include <QApplication>

/**
 * @brief main
 * @param argc Nicht benötigt
 * @param argv Nicht benötigt
 * @return int
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Medienwerkstatt Ressourcenmanagement");
    w.show();

    return a.exec();
}
