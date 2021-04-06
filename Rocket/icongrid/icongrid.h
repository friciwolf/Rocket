#ifndef ICONGRID_H
#define ICONGRID_H

#include <QObject>
#include <QWidget>
#include <QGridLayout>

#include "icongriditem.h"

class IconGrid : public QWidget
{
    Q_OBJECT
public:
    explicit IconGrid(QWidget * parent);
    //explicit IconGrid(QWidget * parent, int rows, int columns);
    ~IconGrid();

    int getMaxNumberOfRows() {return m_rows;}
    int getMaxNumberOfColumns() {return m_cols;}
    int getCurrentNumberOfItems() {return m_items.size();}
    int getCurrentNumberOfRows() {return (int)(getCurrentNumberOfItems()/getMaxNumberOfColumns());}
    int getCurrentNumberOfColumns() {
        return getCurrentNumberOfItems()>=getMaxNumberOfColumns() ? getMaxNumberOfColumns() : getCurrentNumberOfItems();}
    void setMaxNumberOfRows(int row) {m_rows = row;}
    void setMaxNumberOfColumns(int cols) {m_cols = cols;}
    void addItem(IconGridItem * item);
    void addItems(std::vector<IconGridItem*> items);

private:
    // Vector of all items
    std::vector<IconGridItem*> m_items;

    // Maximum Number of Rows and Columns
    int m_rows = 4;
    int m_cols = 8;

    QGridLayout * m_layout;
};

#endif // ICONGRID_H
