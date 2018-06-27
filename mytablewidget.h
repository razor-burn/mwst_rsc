#ifndef MYTABLEWIDGET_H
#define MYTABLEWIDGET_H
#include "delegates\spinboxdelegate.h"
#include <QTableWidget>

/**
 * @brief TableWidget welches verwendet wird um alle gewünschten Positionen einer Ausleihe temporär aufzulisten, bevor diese in die Datenbank geschrieben werden
 */
class MyTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    explicit MyTableWidget(QWidget *parent = 0);

    int addRow(QList<QString> items);
    void appendToResourcesList(const int item);
    bool checkResourcesListForEntry(const int entry) const;
    void clearResourcesList();
    void removeFromResourcesList(const int item);

private:

    /// Liste, die die IDs aller in einer Ausleihe gewünschten Ressourcen (vor dem absenden!) festhält
    QList<int> m_currentResources;

};

#endif // MYTABLEWIDGET_H
