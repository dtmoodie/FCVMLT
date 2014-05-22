#ifndef QTCONTAINER_H
#define QTCONTAINER_H

#include <QObject>
#include <QTreeWidgetItem>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


class qtContainer :public QObject, public QTreeWidgetItem
{
    Q_OBJECT
public:
    explicit qtContainer(QTreeWidget *parent = 0);

signals:
    void imgUpdated();
public slots:

};

#endif // QTCONTAINER_H
