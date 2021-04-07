#include "searchfield.h"
#include "stylingparams.h"
#include <QIcon>
#include <QKeyEvent>
#include <QDebug>

SearchField::SearchField(QWidget *parent, int search_width, int search_height) : QLineEdit(parent)
{
    setAutoFillBackground(true);
    setMinimumSize(search_width,search_height);
    //setMaximumSize(search_width,search_height);
    setPlaceholderText("search");
    addAction(QIcon::fromTheme("search"),QLineEdit::LeadingPosition);

    QPalette searchfield_palette;
    searchfield_palette.setColor(QPalette::ColorRole::Base,RocketStyle::WhiteColour);
    searchfield_palette.setColor(QPalette::ColorRole::Highlight,Qt::blue);
    searchfield_palette.setColor(QPalette::Text,Qt::black);
    setPalette(searchfield_palette);
}

void SearchField::keyPressEvent(QKeyEvent *event)
{
    std::vector<Qt::Key> navigationKeys = {Qt::Key::Key_Right,Qt::Key::Key_Left,Qt::Key::Key_Up,Qt::Key::Key_Down, Qt::Key::Key_Escape};
    if (std::find(navigationKeys.begin(),navigationKeys.end(),event->key())!=navigationKeys.end())
    {
        navigate(event->key());
        event->accept();
    }
    else {
        QLineEdit::keyPressEvent(event);
        event->accept();
    }
}

