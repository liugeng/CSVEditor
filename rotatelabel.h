#ifndef ROTATELABEL_H
#define ROTATELABEL_H

#include <QLabel>

class RotateLabel : public QLabel
{
public:
    RotateLabel();

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // ROTATELABEL_H
