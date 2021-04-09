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

    // IconGrid -- minimum 2 columns, otherwise navigation gets crazy (=it crashes)
    const int m_rows = 5;
    const int m_cols = 9;

    // IconGridItem
    const std::vector<int> icongrid_ratio_rows({3,1});

    // IconGridItemCanvas
    const int icongridcanvas_threshold = 100;

    // Pager
    const int pager_swpipe_threshold = 200;
    const int pager_deadzone_threshold = 50;

    // PagerItem
    const QSize pageritem_error_label_size = QSize(500,50);
    const QFont pageritem_error_label_font = QFont("5",16); // '5': default font QFont::AnyStyle.

    // Active Indicator
    const QBrush active_indicator_brush = QBrush(QColor(0,0,0,100),Qt::BrushStyle::SolidPattern);
}

#endif // STYLINGPARAMS_H
