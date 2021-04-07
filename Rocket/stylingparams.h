#ifndef STYLINGPARAMS_H
#define STYLINGPARAMS_H

#include <QPalette>
#include <QFont>

class WhiteBck : public QPalette {
public:
    WhiteBck(QColor colour);
};


namespace RocketStyle {
    /*
     * Reasonable Defaults for a resolution 1366x768
     * White theme
    */
    const QColor WhiteColour = QColor(255,255,255,200);
    const QPalette WhiteBackground = (QPalette)WhiteBck(WhiteColour);

    // MainWindow
    const int searchfield_width = 300;
    const int searchfield_height = 30;

    // IconGrid
    const int m_rows = 4;
    const int m_cols = 8;

    // IconGridItem
    const int icongrid_item_size = 150;
    const int icongrid_icon_size = 80;

    // IconGridItemCanvas
    const int icongridcanvas_threshold = 100;

    // Pager
    const int pager_swpipe_threshold = 200;
    const int pager_deadzone_threshold = 50;

    // PagerItem
    const QSize pageritem_error_label_size = QSize(500,50);
    const QFont pageritem_error_label_font = QFont("5",16); // '5': default font QFont::AnyStyle.

    // Indicator Preferences
    const int indicator_height = 30;
    const int indicator_spacing = 5;
    const int indicator_radius = 10;

    // Active Indicator
    const QBrush active_indicator_brush = QBrush(QColor(0,0,0,100),Qt::BrushStyle::SolidPattern);
}

#endif // STYLINGPARAMS_H
