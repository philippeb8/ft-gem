#include "blackhole.h"

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

    setFramework(eGR);

    reset();

    startTimer(100);
}

void BlackHole::setFramework(Framework aFramework)
{
    eFramework = aFramework;

    switch (eFramework)
    {
    case eGR:
        f = [&](real x, real y) -> real
        {
            return sqrt(pow(50 * H / (M / sqrt((x - pos[0])*(x - pos[0]) + y*y) + H), 2) + pow(50 * H / (M / sqrt((x - pos[1])*(x - pos[1]) + y*y) + H), 2)); // for demo purposes only
        };
        break;

    case eFT:
        f = [&](real x, real y) -> real
        {
            real const R = 8;
            real const d[2] = {sqrt((x - pos[0])*(x - pos[0]) + y*y), sqrt((x - pos[1])*(x - pos[1]) + y*y)};

            return sqrt(pow((d[0] < R) ? (30 - 3 * M * (3 * R * R + d[0] * d[0]) / (2 * R * R * R)) : (50 * H / (M / d[0] + H)), 2) + pow((d[1] < R) ? (30 - 3 * M * (3 * R * R + d[1] * d[1]) / (2 * R * R * R)) : (50 * H / (M / d[1] + H)), 2)); // for demo purposes only
        };
        break;
    }
}

BlackHole::~BlackHole()
{
}

void BlackHole::timerEvent(QTimerEvent *)
{
    if (! isVisible())
        return;

    plot(f);

    switch (eFramework)
    {
    case eGR:
        if (pos[0] < 0)
        {
            pos[0] += vel[0];
            pos[1] += vel[1];
        }
        break;

    case eFT:
        if (pos[0] < -4)
        {
            pos[0] += vel[0];
            pos[1] += vel[1];
        }
        break;
    }
}

void BlackHole::reset()
{
    pos[0] = -100;
    vel[0] = 2;

    pos[1] = 100;
    vel[1] = -2;
}
