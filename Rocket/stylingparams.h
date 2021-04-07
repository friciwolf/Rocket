#ifndef STYLINGPARAMS_H
#define STYLINGPARAMS_H

#include <QPalette>

class WhiteBck : public QPalette {
public:
    WhiteBck(QColor colour);
};

namespace RocketStyle {
    const QColor WhiteColour = QColor(255,255,255,150);
    const QPalette WhiteBackground = (QPalette)WhiteBck(WhiteColour);
    const int m_rows = 4;
    const int m_cols = 8;
}

#endif // STYLINGPARAMS_H
