#ifndef BLACKHOLE_H
#define BLACKHOLE_H

#include <functional>

#include <QWidget>

#include "graphui.h"

class BlackHole : public GraphUI
{
    Q_OBJECT

public:
    enum Framework {eGR, eFT} eFramework = eGR;

    BlackHole( QWidget *parent );

    void setFramework(Framework aFramework);

    ~BlackHole();

protected slots:
    void reset();

protected:
    static real constexpr c = 299792458.L;
    static real constexpr G = 100;//6.67428e-11L;
    static real constexpr H = 10;//C * C / G / 2e7; // demo purposes only
    static real constexpr M = 100;//1.98911E+30L;

    void timerEvent( QTimerEvent *e );

    std::function<real (real, real)> f;
    int pos[2], vel[2];
};

#endif // BlackHole_H
