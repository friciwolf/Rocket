#ifndef ICONGRID_H
#define ICONGRID_H

#include <QObject>
#include <QWidget>
#include <QGridLayout>

#include "icongriditem.h"
#include "stylingparams.h"

class IconGrid : public QWidget
{
    Q_OBJECT
public:
    explicit IconGrid(QWidget * parent);
    //explicit IconGrid(QWidget * parent, int rows, int columns);
    ~IconGrid();
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

    int getMaxNumberOfRows() {return m_rows;}
    int getMaxNumberOfColumns() {return m_cols;}
    int getNumberOfItems() {return m_items.size();}
    int getCurrentNumberOfRows(){
        return (getNumberOfItems()<=getMaxNumberOfColumns() ? 1 : (int)(getNumberOfItems()/getMaxNumberOfColumns())+1);}
    int getCurrentNumberOfColumns() {
        return getNumberOfItems()>=getMaxNumberOfColumns() ? getMaxNumberOfColumns() : getNumberOfItems();}
    void setMaxNumberOfRows(int row) {m_rows = row;}
    void setMaxNumberOfColumns(int cols) {m_cols = cols;}
    void addItem(IconGridItem * item);

    int getActiveElement(){return m_active_element;}
    void setActiveElement(int element);
    void unhighlightAll();
    void highlight(int element);
    void resetHighlightAndActiveElement();

    std::vector<IconGridItem*> getItems(){return m_items;}

signals:
    void goToPage(int deltaPage);

private:
    // Vector of all items
    std::vector<IconGridItem*> m_items;

    // Maximum Number of Rows and Columns
    int m_rows = RocketStyle::m_rows;
    int m_cols = RocketStyle::m_cols;
    int m_active_element = -1;

    QGridLayout * m_layout;
};

#endif // ICONGRID_H
