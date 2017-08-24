#include "rotatelabel.h"
#include <QPainter>

RotateLabel::RotateLabel()
{
    setAlignment(Qt::AlignCenter);
}

void RotateLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.translate(QPointF(geometry().width()*0.5, geometry().height()));
    p.rotate(-90);
    p.drawText(QPoint(0,0), text());
}
