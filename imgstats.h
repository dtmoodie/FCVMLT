#ifndef IMGSTATS_H
#define IMGSTATS_H

#include <QWidget>

namespace Ui {
class imgStats;
}

class imgStats : public QWidget
{
    Q_OBJECT

public:
    explicit imgStats(QWidget *parent = 0);
    ~imgStats();

private:
    Ui::imgStats *ui;
};

#endif // IMGSTATS_H
