#include "universe.h"

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

Universe::Universe( QWidget *parent)
    : QWidget( parent )
    , buffer( width(), height() )
{
    reset();

    startTimer(10);
}

Universe::~Universe()
{
}

void Universe::timerEvent(QTimerEvent *)
{
    if (! isVisible())
        return;

    {
        QRect r(pos[0], height()/2 - (pos[0] * 200 / width()) / 2, (pos[0] * 200 / width()), (pos[0] * 200 / width()));
        QPainter painter;
        painter.begin( &buffer );
        painter.setBrush(Qt::black);
        painter.drawEllipse(r);
        painter.end();
        bitBlt( this, r.x(), r.y(), &buffer, r.x(), r.y(), r.width(), r.height() );

        update(r);
    }

    if (pos[0] + 200 < width() )
        pos[0] += vel[0];

    {
        QRect r(pos[0], height()/2 - (pos[0] * 200 / width()) / 2, (pos[0] * 200 / width()), (pos[0] * 200 / width()));
        QPainter painter;
        painter.begin( &buffer );
        QColor c;
        c.setHsv(240 - pos[0] * 240 / width(), 255, 255);
        painter.setBrush(c);
        painter.drawEllipse(r);
        painter.end();
        bitBlt( this, r.x(), r.y(), &buffer, r.x(), r.y(), r.width(), r.height() );

        update(r);
    }

    repaint();
}

void Universe::reset()
{
    pos[0] = 0;
    vel[0] = 1;

    buffer.fill( Qt::black );
    repaint();
}

void Universe::resizeEvent( QResizeEvent *e )
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

void Universe::paintEvent( QPaintEvent *e )
{
    QWidget::paintEvent( e );

    QVector<QRect> rects = e->region().rects();
    for ( uint i = 0; i < rects.count(); ++ i )
    {
        QRect r = rects[(int)i];
        bitBlt( this, r.x(), r.y(), &buffer, r.x(), r.y(), r.width(), r.height() );
    }
}
