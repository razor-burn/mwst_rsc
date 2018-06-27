#include "TreeViewComboBox.h"

#include <QHeaderView>
#include "dbinterface\category.h"
#include "dbinterface\location.h"


TreeViewComboBox::TreeViewComboBox(QWidget *parent): QComboBox(parent),
    _treeView(NULL),
    model(NULL)
{
    model = new QStandardItemModel(this);
    _treeView = new QTreeView(this);
}

void TreeViewComboBox::init(const QString & table)
{
    m_table = table;
    this->populate(NULL);
    this->setModel(model);
    this->setView(_treeView);
    _treeView->header()->hide();
    _treeView->expandAll();
}

void TreeViewComboBox::populate(QStandardItem *node)
{
    QSqlQuery entries;
    if(m_table == "categories")
        entries = Category().getBy(CONTAINER_PARENT_COLUMN, node == NULL ? -1 : node->data().toInt());
    else if(m_table == "rooms")
        entries = Room().getBy(CONTAINER_PARENT_COLUMN, node == NULL ? -1 : node->data().toInt());
    while(entries.next())
    {
        QStandardItem * entry = new QStandardItem(entries.value("name").toString());
        entry->setData(entries.value("id"));

        QStandardItem * parent;
        if(node != NULL)
            parent = node;
        else
            parent = model->invisibleRootItem();

        parent->appendRow(entry);
        this->populate(entry);
    }
}


void TreeViewComboBox::setIndex(int index)
{
    QModelIndexList in = model->match(model->item(0,0)->index(), Qt::UserRole + 1, QVariant::fromValue(index), 1, Qt::MatchRecursive);
    if(in[0].parent().isValid())
    {
        this->setRootModelIndex(in[0].parent());
        this->setCurrentIndex(in[0].row());
        this->setRootModelIndex(model->invisibleRootItem()->index());
    }
    else
        this->setCurrentIndex(in[0].row());
}

int TreeViewComboBox::getCurrentData(void)
{
    return this->currentData(Qt::UserRole  + 1).toInt();
}


