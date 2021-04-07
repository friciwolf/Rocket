#include "searchfield.h"
#include "stylingparams.h"
#include <QIcon>
#include <QKeyEvent>

SearchField::SearchField(QWidget *parent, int search_width, int search_height) : QLineEdit(parent)
{
    setAutoFillBackground(true);
    setMinimumSize(search_width,search_height);
    setMaximumSize(search_width,search_height);
    setPlaceholderText("search");
    addAction(QIcon::fromTheme("search"),QLineEdit::LeadingPosition);

    QPalette searchfield_palette;
    searchfield_palette.setColor(QPalette::ColorRole::Base,RocketStyle::WhiteColour);
    searchfield_palette.setColor(QPalette::ColorRole::Highlight,Qt::blue);
    searchfield_palette.setColor(QPalette::Text,Qt::black);
    setPalette(searchfield_palette);
}

