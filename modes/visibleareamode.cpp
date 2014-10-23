#include "../modes/visibleareamode.h"

VisibleAreaMode::VisibleAreaMode(QGraphicsScene *scene, QWidget *form) : AbstractMode(scene), form(form)
{
    QPen pen(Qt::NoPen);
    QBrush brush(Qt::black);

    // Black background (top, bottom, left, right)
    rectTop    = new QGraphicsRectItem;
    rectBottom = new QGraphicsRectItem;
    rectLeft   = new QGraphicsRectItem;
    rectRight  = new QGraphicsRectItem;

    rectTop->setOpacity(.5);
    rectBottom->setOpacity(.5);
    rectLeft->setOpacity(.5);
    rectRight->setOpacity(.5);

    rectTop->setPen(pen);
    rectBottom->setPen(pen);
    rectLeft->setPen(pen);
    rectRight->setPen(pen);

    rectTop->setBrush(brush);
    rectBottom->setBrush(brush);
    rectLeft->setBrush(brush);
    rectRight->setBrush(brush);

    // First rectangle fullscreen
    rectTop->setRect(0, 0, scene->width(), scene->height());

    scene->addItem(rectTop);
    scene->addItem(rectBottom);
    scene->addItem(rectLeft);
    scene->addItem(rectRight);
}

void VisibleAreaMode::init(int x, int y)
{
    form->hide();

    area.x = x;
    area.y = y;
    area.width = 0;
    area.height = 0;

    set(x, y, 0, 0);
}

void VisibleAreaMode::move(int x, int y)
{
    if (x > area.x && qAbs(area.x + area.width - x) <= qAbs(area.x - x)) {
        area.width = x - area.x;
    } else {
        area.width = area.width + area.x - x;
        area.x = x;
    }

    if (y > area.y && qAbs(area.y + area.height - y) <= qAbs(area.y - y)) {
        area.height = y - area.y;
    } else {
        area.height = area.height + area.y - y;
        area.y = y;
    }

    set(area.x, area.y, area.width, area.height);
}

void VisibleAreaMode::stop(int x, int y)
{
    form->setGeometry(area.x, area.y + area.height + 5, form->width(), form->height());
    form->show();
}

void VisibleAreaMode::set(int x, int y, int width, int height)
{
    rectTop->setRect(0, 0, scene->width(), y);
    rectBottom->setRect(0, y + height, scene->width(), scene->height() - y - height);
    rectLeft->setRect(0, y, x, height);
    rectRight->setRect(x + width, y, scene->width() - x - width, height);
}
