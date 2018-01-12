#ifndef SCREENSHOTGUN_ABSTRACTITEM_H
#define SCREENSHOTGUN_ABSTRACTITEM_H

#include <QVector>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

class Overlay;

class AbstractItem : public QObject {
    Q_OBJECT

public:
    explicit AbstractItem(Overlay&);

    virtual void setPen(const QPen& pen)=0;

    void mousePressEvent(QGraphicsSceneMouseEvent*);
    void mouseMoveEvent(QGraphicsSceneMouseEvent*);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*);

    void hoverEnterEvent(QGraphicsSceneHoverEvent*);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent*);

    void addGraphicItem(QGraphicsItem*);
    QVector<QGraphicsItem*>& graphicItems();

protected:
    Overlay& overlay_;
    QVector<QGraphicsItem*> graphicItems_;
};

#endif //SCREENSHOTGUN_ABSTRACTITEM_H