#ifndef ICONGRID_H
#define ICONGRID_H

#include <QObject>
#include <QWidget>
#include <QGridLayout>

#include "icongriditem.h"

#include "../RocketLibrary/tools/rocketconfigmanager.h"

class IconGrid : public QWidget
{
    Q_OBJECT
public:
    explicit IconGrid(QWidget * parent);
    //explicit IconGrid(QWidget * parent, int rows, int columns);
    ~IconGrid();
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    int getMaxNumberOfRows() {return m_rows;}
    int getMaxNumberOfColumns() {return m_cols;}
    int getNumberOfItems() {return m_items.size();}
    int getCurrentNumberOfRows(){
        return getNumberOfItems()/getMaxNumberOfColumns()+(getNumberOfItems()%getMaxNumberOfColumns()>0 ? 1 : 0);}
    int getCurrentNumberOfColumns() {
        return getNumberOfItems()>=getMaxNumberOfColumns() ? getMaxNumberOfColumns() : getNumberOfItems();}
    void setMaxNumberOfRows(int row) {m_rows = row;}
    void setMaxNumberOfColumns(int cols) {m_cols = cols;}
    void addItem(IconGridItem * item);

    QGridLayout * getLayout(){return m_layout;}
    int getActiveElement(){return m_active_element;}
    void setActiveElement(int element);
    void unhighlightAll();
    void highlight(int element);
    void resetHighlightAndActiveElement();

    std::vector<IconGridItem*> getItems(){return m_items;}
    void setItems(std::vector<IconGridItem*> items){m_items=items;}

signals:
    void goToPage(int deltaPage);

private:
    // Vector of all items
    std::vector<IconGridItem*> m_items;

    // Maximum Number of Rows and Columns
    int m_rows = ConfigManager.getRowNumber();
    int m_cols = ConfigManager.getColumnNumber();
    int m_active_element = -1;

    QGridLayout * m_layout;
};

#endif // ICONGRID_H
