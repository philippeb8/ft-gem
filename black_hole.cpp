#include "black_hole.h"

#include <QPainter>
#include <QPaintEvent>
#include <QDebug>


BlackHole::BlackHole( QWidget *parent )
    : GraphUI( parent )
{
    option.min.x = -100;
    option.max.x = 100;
    option.scl.x = 25;
    option.min.y = 0;
    option.max.y = 100;
    option.scl.y = 25;
    option.min.z = 0;
    option.max.z = 100;
    option.scl.z = 25;
    option.stat = GraphUI::eScientific;
    option.mode = GraphUI::e3d;

    // zoom
    real const zoom = 0;

    real const xdelta = option.max.x - option.min.x;
    real const xrange = xdelta - xdelta * pow(2.0L, zoom);
    fxy.min.x = option.min.x + xrange / 2;
    fxy.max.x = option.max.x - xrange / 2;
    fxy.scl.x = option.scl.x;

    real const ydelta = option.max.y - option.min.y;
    real const yrange = ydelta - ydelta * pow(2.0L, zoom);
    fxy.min.y = option.min.y + yrange / 2;
    fxy.max.y = option.max.y - yrange / 2;
    fxy.scl.y = option.scl.y;

    real const zdelta = option.max.z - option.min.z;
    real const zrange = zdelta - zdelta * pow(2.0L, zoom);
    fxy.min.z = option.min.z + zrange / 2;
    fxy.max.z = option.max.z - zrange / 2;
    fxy.scl.z = option.scl.z;

    reset();

    startTimer(100);
}

BlackHole::~BlackHole()
{
}

void BlackHole::timerEvent(QTimerEvent *)
{
    if (! isVisible())
        return;

    plot([&](real x, real y) -> real { return 50 * H / (M / sqrt((x - pos[0])*(x - pos[0]) + y*y) + H) + 50 * H / (M / sqrt((x - pos[1])*(x - pos[1]) + y*y) + H); });

    if (pos[0] < 0)
    {
        pos[0] += vel[0];
        pos[1] += vel[1];
    }
}

void BlackHole::reset()
{
    pos[0] = -100;
    vel[0] = 2;

    pos[1] = 100;
    vel[1] = -2;
}
