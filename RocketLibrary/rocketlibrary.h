#ifndef ROCKETLIBRARY_H
#define ROCKETLIBRARY_H

#include <QString>
#include <QPalette>

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
    const int use_system_wallpaper_screen = 0;
    const bool inverted_scrolling_x = false;
    const bool inverted_scrolling_y = false;

    // Active Indicator
    const QBrush active_indicator_brush = QBrush(SecondaryColour,Qt::BrushStyle::SolidPattern);
}

namespace RocketConfig {
    namespace Color {
        const QString group = QString("color");
        const QString base = QString("base");
        const QString secondary = QString("secondary");
    }

    namespace Font {
        const QString group = QString("font");
        const QString size1 = QString("size1");
        const QString size2 = QString("size2");
    }

    namespace Dimensions {
        const QString group = QString("dimensions");
        const QString rows = QString("rows");
        const QString columns = QString("columns");
        const QString verticalorientation = QString("vertical_orientation");
    }

    namespace Background {
        const QString group = QString("background");
        const QString usesystemwallpaper = QString("use_system_wallpaper");
        const QString wallpaperofscreen = QString("wallpaper_of_screen");
    }

    namespace Settings {
        const QString group = QString("settings");
        const QString invertedscrollingx = QString("inverted_scrolling_x");
        const QString invertedscrollingy = QString("inverted_scrolling_y");
    }
}

#endif // ROCKETLIBRARY_H
