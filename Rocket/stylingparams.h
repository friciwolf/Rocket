#ifndef STYLINGPARAMS_H
#define STYLINGPARAMS_H

#include <QPalette>
#include <QFont>


namespace RocketStyle {
    /*
     * Reasonable Defaults.
     * White theme
    */
    const QColor BaseColour = QColor(255,255,255,200);
    const QColor SecondaryColour = QColor(0,0,0,100);
    const int fontsize1 = 10;
    const int fontsize2 = 20;

    // A click is recongnized only if the mouse moves 10 pixels
    const int click_tolerance = 100;

    // IconGrid -- minimum 2 columns, otherwise navigation gets crazy (=it crashes)
    const int m_rows = 4;
    const int m_cols = 9;

    // IconGridItem
    const std::vector<int> icongrid_ratio_rows({3,1});

    // Pager
    const int pager_swpipe_threshold = 200;
    const int pager_deadzone_threshold = 50;

    const bool pager_vertical_orientation = false;

    const bool use_system_wallpaper = true;
    const bool use_system_wallpaper_screen = 0;

    // Active Indicator
    const QBrush active_indicator_brush = QBrush(SecondaryColour,Qt::BrushStyle::SolidPattern);
}

#endif // STYLINGPARAMS_H
