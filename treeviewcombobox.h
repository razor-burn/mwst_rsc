#ifndef TREEVIEWCOMBOBOX_H
#define TREEVIEWCOMBOBOX_H

#include <QComboBox>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>

class TreeViewComboBox : public QComboBox {
  Q_OBJECT

public:
  explicit TreeViewComboBox(QWidget *parent = 0);
  void init(const QString & table);
  void populate(QStandardItem * node);
  void setIndex(int index);
  int getCurrentData(void);
private:
  QStandardItemModel * model;
  QTreeView* _treeView;
  QString m_table;

};
#endif // TREEVIEWCOMBOBOX_H
