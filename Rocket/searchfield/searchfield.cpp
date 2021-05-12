#include <QIcon>
#include <QKeyEvent>
#include <QDebug>

#include "searchfield.h"

#include "../RocketLibrary/tools/rocketconfigmanager.h"

SearchField::SearchField(QWidget *parent) : QLineEdit(parent)
{

}

void SearchField::positioning()
{
    QRect g = parentWidget()->geometry();
    setGeometry((g.width()-width())*0.5,(g.height()*0.1-height())*0.5,g.width()*0.2,g.height()*0.05);
}

void SearchField::resizeEvent(QResizeEvent *event)
{
    if (parentWidget()->size().width()<17)
    {
        event->accept();
        return;
    }

    //it costs less time to set these properties here
    setAutoFillBackground(true);
    setPlaceholderText("search");
    addAction(QIcon::fromTheme("search"),QLineEdit::LeadingPosition);

    QFont fieldfont = font();
    fieldfont.setPointSize(ConfigManager.getFontSize1());
    setFont(fieldfont);
    positioning();
}

void SearchField::keyPressEvent(QKeyEvent *event)
{
    std::vector<Qt::Key> navigationKeys = {Qt::Key::Key_Right,Qt::Key::Key_Left,Qt::Key::Key_Up,Qt::Key::Key_Down, Qt::Key::Key_Escape, Qt::Key::Key_Tab};
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

