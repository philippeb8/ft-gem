#include "elevator.h"

#include <QPainter>
#include <QPaintEvent>


namespace
{
void bitBlt( QPaintDevice * dst, int x, int y, const QPixmap* src, int sx, int sy, int sw, int sh )
{
    QPainter p( dst );
    p.drawPixmap( x, y, *src, sx, sy, sw, sh );
}
}

Elevator::Elevator( QWidget *parent)
    : QWidget( parent )
    , buffer( width(), height() )
{
    reset();

    startTimer(100);
}

Elevator::~Elevator()
{
}

void Elevator::timerEvent(QTimerEvent *)
{
    if (! isVisible())
        return;

    {
        QRect r(width()/2 - 80, pos[0], 160, 180);
        QPainter painter;
        painter.begin( &buffer );
        painter.setBrush(Qt::black);
        painter.drawRect(r);
        painter.end();
        bitBlt( this, r.x(), r.y(), &buffer, r.x(), r.y(), r.width(), r.height() );

        update(r);
    }

    {
        QRect r(width()/2 - 10, pos[0] + pos[1] + 20, 20, 20);
        QPainter painter;
        painter.begin( &buffer );
        painter.setBrush(Qt::black);
        painter.drawRect(r);
        painter.end();
        bitBlt( this, r.x(), r.y(), &buffer, r.x(), r.y(), r.width(), r.height() );

        update(r);
    }

    if (pos[0] + 180 < height() )
        pos[0] += vel[0];

    if (pos[1] > 120 || pos[1] < 0)
        vel[1] = -vel[1];

    pos[1] += vel[1];

    {
        QRect r(width()/2 - 80, pos[0], 160, 180);
        QPainter painter;
        painter.begin( &buffer );
        painter.setBrush(Qt::white);
        painter.drawRect(r);
        painter.end();
        bitBlt( this, r.x(), r.y(), &buffer, r.x(), r.y(), r.width(), r.height() );

        update(r);
    }

    {
        QRect r(width()/2 - 10, pos[0] + pos[1] + 20, 20, 20);
        QPainter painter;
        painter.begin( &buffer );
        QColor c;
        c.setHsv((pos[0] + pos[1]) * 240 / height(), 255, 255);
        painter.setBrush(c);
        painter.drawRect(r);
        painter.end();
        bitBlt( this, r.x(), r.y(), &buffer, r.x(), r.y(), r.width(), r.height() );

        update(r);
    }

    repaint();
}

void Elevator::reset()
{
    pos[0] = 0;
    vel[0] = 2;

    pos[1] = 0;
    vel[1] = 8;

    buffer.fill( Qt::black );
    repaint();
}

void Elevator::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );

    int w = width() > buffer.width() ?
            width() : buffer.width();
    int h = height() > buffer.height() ?
            height() : buffer.height();

    buffer = QPixmap(w, h);

    buffer.fill( Qt::black );
//    bitBlt( &buffer, 0, 0, &tmp, 0, 0, tmp.width(), tmp.height() );
}

void Elevator::paintEvent( QPaintEvent *e )
{
    QWidget::paintEvent( e );

    QVector<QRect> rects = e->region().rects();
    for ( uint i = 0; i < rects.count(); ++ i )
    {
        QRect r = rects[(int)i];
        bitBlt( this, r.x(), r.y(), &buffer, r.x(), r.y(), r.width(), r.height() );
    }
}
