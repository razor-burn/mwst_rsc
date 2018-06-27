#ifndef ADMINAREA_H
#define ADMINAREA_H

#include <QDialog>

namespace Ui {
class Adminarea;
}

/**
 * @brief Formularklasse zum Löschen und Hinzufügen von Schnitträumen und Adminbenutzern (also Mitarbeitern)
 */

class Adminarea : public QDialog
{
    Q_OBJECT

protected:

    void showEvent(QShowEvent * event);

public:
    explicit Adminarea(QWidget *parent = 0);
    ~Adminarea();
    void init();

private slots:
    void on_buttonDelAdminUser_clicked();
    void on_buttonAddAdmin_clicked();
    void on_buttonDelEditingroom_clicked();
    void on_pushButtonAddEditingroom_clicked();

private:
    /// Zeiger auf die Oberfläche
    Ui::Adminarea *ui;
};

#endif // ADMINAREA_H
