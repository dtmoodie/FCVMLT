#ifndef MLDIALOG_H
#define MLDIALOG_H

#include <QDialog>
#include <container.h>
#include <QPlainTextEdit>
#include <QGridLayout>
namespace Ui {
class mlDialog;
}

class mlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit mlDialog(QWidget *parent = 0, QTreeWidget* tree_ = 0);
    ~mlDialog();

public slots:
    void handleSourceSelectionChange();


private:
    Ui::mlDialog *ui;
    QTreeWidget* selectedTree; // Pointer to the source select tree
    QTreeWidget* informationTree; // this tree displays more detailed information

};

#endif // MLDIALOG_H
