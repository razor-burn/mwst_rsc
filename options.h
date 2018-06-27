#ifndef OPTIONS_H
#define OPTIONS_H

#include <QWidget>

namespace Ui {
class options;
}

class options : public QWidget
{
    Q_OBJECT

protected:
    void showEvent(QShowEvent * event);

public:
    explicit options(QWidget *parent = 0);
    ~options();
    void init();

private slots:
    void on_buttonDelAdminUser_clicked();
    void on_buttonAddAdmin_clicked();
    void on_buttonDelEditingroom_clicked();
    void on_pushButtonAddEditingroom_clicked();

    void on_buttonDelFaculty_clicked();

    void on_ButtonAddFaculty_clicked();

private:
    /// Zeiger auf die Oberfläche
    Ui::options *ui;
};

#endif // OPTIONS_H
