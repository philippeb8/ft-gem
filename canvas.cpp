/**
    Finite Theory Simulator
    Copyright (C) 2011 Phil Bouchard <philippeb8@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define EDITION "4.21"

#include "canvas.h"

//#include <unistd.h>
#include <stdlib.h>
//#include <sys/sysinfo.h>

#include <cmath>
#include <limits>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <typeinfo>

#include <QtWidgets/QApplication>
#include <qevent.h>
#include <qpainter.h>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QToolTip>
#include <qrect.h>
#include <qpoint.h>
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QFileDialog>
#include <qcursor.h>
#include <qimage.h>
#include <QStringList>
#include <QtWidgets/QMenu>
#include <QHash>
#include <QtWidgets/QLayout>
#include <qobject.h>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QStyleFactory>
//Added by qt3to4:
#include <QtWidgets/QHBoxLayout>
#include <QTimerEvent>
#include <QtWidgets/QLabel>
#include <QPixmap>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QtWidgets/QVBoxLayout>
#include <QPaintEvent>
#include <QVector>
#include <QtGui/QPainter>
#include <QtWidgets/QDesktopWidget>
#include <QtCore/QProcess>
#include <QDebug>

using namespace std;

const real scale[] = {8e8, 8e9L, 8e9L, 8e9L, 8e10L};
const real upper = 0.1L;

// FT time formula
// observer is infinitly far away
real Planet::FR1(real m, real d, real h)
{
    return (h) / (m / d + h);
}

// observer is in a null environment
real Planet::FR2(real m, real d, real h)
{
    return (h + 1.L) / (m / d + h);
}

// Newton time formula
real Planet::NW(real, real, real)
{
    return 1.L;
}


namespace
{
void bitBlt( QPaintDevice * dst, int x, int y, const QPixmap* src, int sx, int sy, int sw, int sh )
{
    QPainter p( dst );
    p.drawPixmap( x, y, *src, sx, sy, sw, sh );
}
}


/** 
	@brief			Calculates the next position of the planet or photon
	@param planet	Planets that will affect the movement of the planet that is moving (this)
	@param upper	Time interval
*/

inline void Planet::operator () (const vector<Planet> &planet, const real & upper)
{
	// net acceleration vector (with all planets)
    vector3 va(0.L, 0.L, 0.L);

    // iterate through all planets
	for (size_t i = 0; i < planet.size(); i ++)
	{
		// if same planet or photon then skip
		if (planet[i].m == m || planet[i].m == 0.0L)
			continue;
		
		// vector and norm between the moving planet and the other one
		const vector3 normal(p[0] - planet[i].p[0], p[1] - planet[i].p[1], p[2] - planet[i].p[2]);
		const real norm2 = pow(normal[0], 2) + pow(normal[1], 2) + pow(normal[2], 2);
		const real norm = sqrt(norm2);
		
		// a = Gm/r^2 decomposed in scalar
        va[0] += - G * planet[i].m / norm2 * normal[0] / norm;
        va[1] += - G * planet[i].m / norm2 * normal[1] / norm;
        va[2] += - G * planet[i].m / norm2 * normal[2] / norm;
	}

    // spherical coordinates
    if (! first)
    {
        ps[2] = ps[1];
        ps[1] = ps[0];
    }

    ps[0][0] = sqrt(pow(p[0], 2) + pow(p[1], 2) + pow(p[2], 2));
    ps[0][1] = atan2(p[1], p[0]);
    ps[0][2] = acos(p[2] / ps[0][0]);

    if (first)
    {
        ps[2] = ps[0];
        ps[1] = ps[0];
    }

    // save
	const vector3 q(p[0], p[1], p[2]);
	
	// sun - planet
	const real nnorm_p = p.norm();
	vector3 vnn = p / nnorm_p;

    // save old time value
    if (! first)
    {
        t[1] = t[0];
    }

    // Newton: t = upper
    // FT: t = upper / ((m / d + h) / h)
    t[0] = upper * f(planet[0].m, nnorm_p, planet[0].h);

    if (first)
    {
        t[1] = t[0];
    }

    // p = p + v*t + (a*t^2)/2
    p += v[0] * t[0] + va * t[0] * t[0] / 2.;

    // v = v + a*t
    v[0] += va * upper;
	
    switch (eType)
	{
    // perihelion precession
    case PP:
        if (first || ps[1][0] < ps[2][0] && ps[1][0] < ps[0][0])
        {
            if (! first)
            {
                ps[4] = ps[3];
            }

            ps[3] = ps[1];

            if (first)
            {
                ps[4] = ps[3];
            }
            else
            {
                updated = true;
            }
        }
        break;

	// gravitational light bending
	case LB:
		if (q[0] >= -200000000000.L && p[0] < -200000000000.L)
		{
			pp[1] = pp[0];
			
            updated = true;
		}
		break;

    // big bang & pioneer 10
    case BB:
    case V1:
        if (floor(q[0] / 1e10) != floor(p[0] / 1e10))
        {
            v[1] = v[0];

            updated = true;
        }
        break;
    }

    first = false;
}

Dual::Dual(Canvas * pParent, int id) : p(pParent), i(id)
{
	start();
}

void Dual::run()
{
    while (true)
	{
		// stop the processing until the tab becomes visible
		while (! p->isVisible())
			QThread::msleep(100);
		
		// move the same planet or photon according to Newton & FT
		for (size_t j = 0; j < p->planet.size(); j ++)
            p->planet[j][i](p->planet[j], upper);
	}
}

const bool no_writing = false;

Canvas::Canvas( QWidget *parent, Type eType, real scale )
    : QWidget( parent/*, name, Qt::WStaticContents*/ ),
      eType(eType), pen( Qt::red, 3 ), polyline(3), mousePressed( false ), buffer( width(), height() ), scale(scale)
{
//	setAttribute(Qt::WA_PaintOutsidePaintEvent, true);
	
    reset();
	
//    if ((qApp->argc() > 0) && !buffer.load(qApp->argv()[1]))
//        buffer.fill( palette().base().color() );
//    setBackgroundMode( Qt::PaletteBase );
#ifndef QT_NO_CURSOR
    setCursor( Qt::CrossCursor );
#endif

	startTimer(100);

	// launch a thread for each planet or photon
	for (size_t i = 1; i < planet[0].size(); i ++)
		new Dual(this, i);
}

Canvas::~Canvas()
{
}

void Canvas::slotPlanet(int i)
{
	++ i;
	
    switch (eType)
    {
    case PP:
    case LB:
        // precession
        for (size_t j = 0; j < planet.size(); ++ j)
            for (int x = 0; x < 3; ++ x)
            {
                ostringstream s;

                if (stats[i].mean[0].size() > 0)
                {
                    s.setf(ios::scientific, ios::floatfield);
                    s << std::setprecision(numeric_limits<real>::digits10);
                    s << stats[i].precession[j][x];
                }
            }

        // anomaly
        for (int x = 0; x < 3; ++ x)
        {
            ostringstream s;

            if (stats[i].mean[0].size() > 0)
            {
                s.setf(ios::scientific, ios::floatfield);
                s << std::setprecision(numeric_limits<real>::digits10);
                s << stats[i].precession[1][x] - stats[i].precession[0][x];
            }
        }

        // mean
        for (int x = 0; x < 3; ++ x)
        {
            ostringstream s[4];

            // median
            if (stats[i].mean[0].size() > 0)
            {
                s[0].setf(ios::scientific, ios::floatfield);
                s[0] << std::setprecision(numeric_limits<real>::digits10);

                set<real>::iterator k = stats[i].mean[x].begin();
                set<real>::reverse_iterator l = stats[i].mean[x].rbegin();
                advance(k, stats[i].mean[x].size() / 2);
                advance(l, stats[i].mean[x].size() / 2);

                const real median = (* k + * l) / 2;

                s[0] << median;

                // median absolute deviation
                if (stats[i].mean[0].size() > 1)
                {
                    s[1].setf(ios::scientific, ios::floatfield);
                    s[1] << std::setprecision(numeric_limits<real>::digits10);

                    set<real> dev;
                    for (set<real>::iterator m = stats[i].mean[x].begin(); m != stats[i].mean[x].end(); ++ m)
                        dev.insert(abs(* m - median));

                    set<real>::iterator m = dev.begin();
                    set<real>::reverse_iterator n = dev.rbegin();
                    advance(m, dev.size() / 2);
                    advance(n, dev.size() / 2);

                    const real mad = (* m + * n) / 2;

                    s[1] << mad;

                    if (stats[i].best[1][x] > mad)
                    {
                        stats[i].best[0][x] = median;
                        stats[i].best[1][x] = mad;
                    }

                    s[2].setf(ios::scientific, ios::floatfield);
                    s[2] << std::setprecision(numeric_limits<real>::digits10);

                    s[3].setf(ios::scientific, ios::floatfield);
                    s[3] << std::setprecision(numeric_limits<real>::digits10);

                    s[2] << stats[i].best[0][x];
                    s[3] << stats[i].best[1][x];
                }
            }
        }
        break;
    }
}

void Canvas::slotGalaxy(int i)
{
    ++ i;

    switch (eType)
    {
    case BB:
    case V1:
        for (size_t j = 0; j < planet.size(); ++ j)
            for (int x = 0; x < 3; ++ x)
            {
                ostringstream s;

                s.setf(ios::scientific, ios::floatfield);
                s << std::setprecision(numeric_limits<real>::digits10);
                s << planet[j][i].p[x];
            }

        for (size_t j = 0; j < planet.size(); ++ j)
            for (int x = 0; x < 3; ++ x)
            {
                ostringstream s;

                s.setf(ios::scientific, ios::floatfield);
                s << std::setprecision(numeric_limits<real>::digits10);
                s << planet[j][i].v[1][x];
            }

        for (int x = 0; x < 3; ++ x)
        {
            ostringstream s;

            s.setf(ios::scientific, ios::floatfield);
            s << std::setprecision(numeric_limits<real>::digits10);
            s << planet[1][i].v[1][x] - planet[0][i].v[1][x];
        }
        break;
    }
}

void Canvas::timerEvent(QTimerEvent *)
{
    if (! isVisible())
        return;

	{
        QRect r(planet[0][0].p[0] / scale - 40 + width()/2, planet[0][0].p[1] / scale - 40 + height()/2, 80, 80);
        QPainter painter;
		painter.begin( &buffer );
		painter.setBrush(Qt::yellow);
		painter.drawEllipse(r);
		painter.end();
		bitBlt( this, r.x(), r.y(), &buffer, r.x(), r.y(), r.width(), r.height() );

		update(r);
    }

    switch (eType)
    {
    case PP:
    case LB:
        for (size_t i = 1; i < planet[0].size(); ++ i)
        {
            for (size_t j = 0; j < planet.size(); ++ j)
                if (planet[j][i].updated)
                {
                    for (int x = 0; x < 3; ++ x)
                        stats[i].precession[j][x] = planet[j][i].ps[3][x] - planet[j][i].ps[4][x];
                }

            if (planet[0][i].updated && planet[1][i].updated)
            {
                for (int x = 0; x < 3; ++ x)
                    stats[i].mean[x].insert(stats[i].precession[1][x] - stats[i].precession[0][x]);
            }
        }
        break;
    }

    QRect b;

    for (size_t i = 1; i < planet[0].size(); ++ i)
    {
        for (size_t j = 0; j < planet.size(); ++ j)
		{
            QString t = QString(planet[j][i].f == & Planet::NW ? "Newton" : framework) + "_";
            QFont f = QPainter().font();
            f.setPointSize(30);
            QFontMetrics m(f);
            QRect s = m.boundingRect(t);

            b |= s;
        }
    }

    int y = 40;

    for (size_t i = 1; i < planet[0].size(); ++ i)
    {
        for (size_t j = 0; j < planet.size(); ++ j)
        {
            QRect e(planet[j][i].o[0] / scale - 2 + width()/2, planet[j][i].o[1] / scale - 2 + height()/2, 4+1, 4+1);

            planet[j][i].o[0] = planet[j][i].p[0];
            planet[j][i].o[1] = planet[j][i].p[1];
            planet[j][i].o[2] = planet[j][i].p[2];

            QRect r(planet[j][i].o[0] / scale - 2 + width()/2, planet[j][i].o[1] / scale - 2 + height()/2, 4, 4);
            QPainter painter;
            painter.begin( &buffer );
            painter.setPen(QPen(planet[j][i].c, 8));
            painter.drawLine(e.x(), e.y(), r.x(), r.y());

            QString t = QString(planet[j][i].f == & Planet::NW ? "Newton" : framework);
            QFont f = painter.font();
            f.setPointSize(30);
            QFontMetrics m(f);
            QRect s = m.boundingRect(t);
            painter.setFont(f);
            painter.setPen(QPen(planet[j][i].c, 2));
            painter.drawText(width() - b.width(), y, t);

            y += s.height();

            painter.end();
        }
    }

    repaint();
}

void Canvas::reset()
{
    // initial position of each planet and photon
    static const real pos[][3] =
    {
        {0.L, 0.L, 0.L},
        //{-57025548112.2453L, 3197006916.08582L, 5283916036.50742L},
        //{-57358990223.0187831L, 0.L, 0.L},
        {-45922743041.70308L, 0.L, 0.L},
        {26317771130.7392L, 105373484164.43L, 481049442.321637L},
        {-40584904469.4072L, -146162841483.741L, 582517208.913105L},
        {192608888576.284L, -72078449728.0548L, -5537406864.12226L},
        {-230068941192.889L, -766153804794.071L, 9039825087.87588L},
        {1359034179077.08L, -555461097003.149L, -48376702948.4567L},
        {1905563957085.85L, 2247912953966.77L, -16532490448.7952L},
        {1788083649521.39L, 4079380837677.57L, -125881827325.591L},
        {-4043923627184.17L, 3575690969311.01L, 795204553555.504L},

        {250000000000.L, -40000000000.L, 0.L},
        {250000000000.L, -40000000000.L, 0.L},

        {-50000000000.L, 50000000000.L, 0.L},
        {0.L, 50000000000.L, 0.L},
        {50000000000.L, 50000000000.L, 0.L},
        {-50000000000.L, 0.L, 0.L},
        {50000000000.L, 0.L, 0.L},
        {-50000000000.L, -50000000000.L, 0.L},
        {0.L, -50000000000.L, 0.L},
        {50000000000.L, -50000000000.L, 0.L},

        {-4000000000000.L, 0.L, 0.L},
        {-3800000000000.L, 0.L, 0.L},
        {-300000000000.L, 0.L, 0.L},
        {-100000000000.L, 0.L, 0.L},
        {100000000000.L, 0.L, 0.L},
        {300000000000.L, 0.L, 0.L},
        {3800000000000.L, 0.L, 0.L},
        {4000000000000.L, 0.L, 0.L},

        {17048116800000.L, 0.L, 0.L},
        {17048116800000.L, 0.L, 0.L},
    };

    // initial velocity of each planet and photon
    static const real vel[][3] =
    {
        {0.L, 0.L, 0.L},
        //{-13058.0445420602L, -46493.5791091285L, -2772.42900405547L},
        //{0.L, -48372.0145148178242L, 0.L},
        {0.L, -59148.05641434967L, 0.L},
        {-33720.199494784L, 8727.97495192353L, 2044.70922687897L},
        {28173.5639447033L, -8286.58463896112L, 13.3258392757908L},
        {9453.24519302534L, 24875.9047777036L, 333.149595901334L},
        {12310.4853583322L, -3126.10777330552L, -250.842129088533L},
        {3203.18660260855L, 8810.22721786771L, -260.876357307397L},
        {-5198.23543233994L, 4090.32678482699L, 78.6156634354517L},
        {-5005.88142339012L, 2215.0599004751L, 70.452880649377L},
        {-2122.7269723267L, -4538.25658137665L, 1101.51599904528L},

        {-300000.L, 0.L, 0.L},
        {-300000.L, 0.L, 0.L},

        {-50000.L, 50000.L, 0.L},
        {0.L, 50000.L, 0.L},
        {50000.L, 50000.L, 0.L},
        {-80000L, 0.L, 0.L},
        {80000L, 0.L, 0.L},
        {-50000.L, -50000.L, 0.L},
        {0.L, -50000.L, 0.L},
        {50000.L, -50000.L, 0.L},

        {0.L, 5.7749e-6L, 0.L},
        {0.L, 5.9249e-6L, 0.L},
        {0.L, 2.1087e-5L, 0.L},
        {0.L, 3.6523e-5L, 0.L},
        {0.L, -3.6523e-5L, 0.L},
        {0.L, -2.1087e-5L, 0.L},
        {0.L, -5.9249e-6L, 0.L},
        {0.L, -5.7749e-6L, 0.L},

        {11992.L, 0.L, 0.L},
        {11992.L, 0.L, 0.L},
    };

    // name, color, mass, position and velocity of each moving object
    static const Planet Sun 	  ("Sun", 		Qt::yellow, 1.98911E+30L, pos[0], vel[0]);
    static const Planet Mercury   ("Mercury", 	Qt::red, 3.302E+23L, pos[1], vel[1]);
    static const Planet Venus 	  ("Venus", 	Qt::cyan, 4.8685E+24L, pos[2], vel[2]);
    static const Planet Earth 	  ("Earth", 	Qt::blue, 5.9736E+24L, pos[3], vel[3]);
    static const Planet Mars 	  ("Mars", 		Qt::yellow, 6.41850000000001E+23L, pos[4], vel[4]);
    static const Planet Jupiter   ("Jupiter", 	Qt::magenta, 1.8986E+27L, pos[5], vel[5]);
    static const Planet Saturn 	  ("Saturn", 	Qt::darkRed, 5.6842928E+26L, pos[6], vel[6]);
    static const Planet Uranus 	  ("Uranus", 	Qt::green, 8.68320000000002E+25L, pos[7], vel[7]);
    static const Planet Neptune   ("Neptune", 	Qt::darkBlue, 1.0243E+26L, pos[8], vel[8]);
    static const Planet Pluto 	  ("Pluto", 	Qt::darkGray, 1.27E+22L, pos[9], vel[9]);

    static const Planet Photon1   ("Photon1", 	Qt::red, 0.0L, pos[10], vel[10], Planet::FR1, Planet::LB);
    static const Planet Photon2   ("Photon2", 	Qt::darkRed, 0.0L, pos[11], vel[11], Planet::NW, Planet::LB);

    static const Planet Pioneer1   ("Pioneer1", 	Qt::red, 258.8L, pos[28], vel[28], Planet::FR1, Planet::V1);
    static const Planet Pioneer2   ("Pioneer2", 	Qt::darkRed, 258.8L, pos[29], vel[29], Planet::NW, Planet::V1);

    static const Planet Core	  ("Core", 		Qt::black, 2E+11L, pos[0], vel[0], Planet::NW, Planet::BB);
    static const Planet Galaxy1   ("Galaxy1", 	Qt::red, 50000L, pos[12], vel[12], Planet::NW, Planet::BB);
    static const Planet Galaxy2   ("Galaxy2", 	Qt::cyan, 50000L, pos[13], vel[13], Planet::NW, Planet::BB);
    static const Planet Galaxy3   ("Galaxy3", 	Qt::blue, 50000L, pos[14], vel[14], Planet::NW, Planet::BB);
    static const Planet Galaxy4   ("Galaxy4", 	Qt::yellow, 50000L, pos[15], vel[15], Planet::NW, Planet::BB);
    static const Planet Galaxy5   ("Galaxy5", 	Qt::magenta, 50000L, pos[16], vel[16], Planet::NW, Planet::BB);
    static const Planet Galaxy6   ("Galaxy6", 	Qt::darkRed, 50000L, pos[17], vel[17], Planet::NW, Planet::BB);
    static const Planet Galaxy7   ("Galaxy7", 	Qt::green, 50000L, pos[18], vel[18], Planet::NW, Planet::BB);
    static const Planet Galaxy8   ("Galaxy8", 	Qt::darkBlue, 50000L, pos[19], vel[19], Planet::NW, Planet::BB);

    static const Planet Nucleus   ("Nucleus", 	Qt::black, 2E+12L, pos[0], vel[0], Planet::NW, Planet::GR);
    static const Planet Star1     ("Star1", 	Qt::red, 50000L, pos[20], vel[20], Planet::NW, Planet::GR);
    static const Planet Star2     ("Star2", 	Qt::red, 50000L, pos[21], vel[21], Planet::NW, Planet::GR);
    static const Planet Star3     ("Star3", 	Qt::red, 50000L, pos[22], vel[22], Planet::NW, Planet::GR);
    static const Planet Star4     ("Star4", 	Qt::red, 50000L, pos[23], vel[23], Planet::NW, Planet::GR);
    static const Planet Star5     ("Star5", 	Qt::red, 50000L, pos[24], vel[24], Planet::NW, Planet::GR);
    static const Planet Star6     ("Star6", 	Qt::red, 50000L, pos[25], vel[25], Planet::NW, Planet::GR);
    static const Planet Star7     ("Star7", 	Qt::red, 50000L, pos[26], vel[26], Planet::NW, Planet::GR);
    static const Planet Star8     ("Star8", 	Qt::red, 50000L, pos[27], vel[27], Planet::NW, Planet::GR);

    switch (eType)
    {
    // perihelion precession disparity
    case PP:
        planet.resize(2);

        // store each planet using the Newton time formula
        planet[0].resize(2);
        planet[0][0] = Sun;
        planet[0][1] = Mercury;
        //planet[0].push_back(Venus);
        //planet[0].push_back(Earth);
        //planet[0].push_back(Mars);
        //planet[0].push_back(Jupiter);
        //planet[0].push_back(Saturn);
        //planet[0].push_back(Uranus);
        //planet[0].push_back(Neptune);
        //planet[0].push_back(Pluto);

        // copy & change each planet for the FT time formula
        planet[1] = planet[0];
        for (size_t i = 0; i < planet[1].size(); i ++)
        {
            planet[1][i].f = Planet::FR1;
            planet[1][i].c = planet[1][i].c.dark();
        }

        stats.resize(planet[0].size());

        break;

    // gravitational light bending
    case LB:
        planet.resize(2);

        // store the Sun & the photon using the Newton time formula
        planet[0].resize(2);
        planet[0][0] = Sun;
        planet[0][1] = Photon1;

        // store the Sun & the photon using the FT time formula
        planet[1].resize(2);
        planet[1][0] = Sun;
        planet[1][1] = Photon2;

        stats.resize(planet[0].size());
        break;

    // big bang
    case BB:
        planet.resize(2);

        // store the Sun & the planets using FT time formula
        planet[0].resize(9);
        planet[0][0] = Core;
        planet[0][1] = Galaxy1;
        planet[0][2] = Galaxy2;
        planet[0][3] = Galaxy3;
        planet[0][4] = Galaxy4;
        planet[0][5] = Galaxy5;
        planet[0][6] = Galaxy6;
        planet[0][7] = Galaxy7;
        planet[0][8] = Galaxy8;

        // copy & change each planet for the FT time formula
        planet[1] = planet[0];
        for (size_t i = 0; i < planet[1].size(); i ++)
        {
            planet[1][i].f = Planet::Planet::FR2;
            planet[1][i].h = H[1];
            planet[1][i].c = planet[1][i].c.dark();
        }

        stats.resize(planet[0].size());

        break;

        // galactic rotation
    case GR:
        planet.resize(2);

        // store the Sun & the planets using FT time formula
        planet[0].resize(9);
        planet[0][0] = Nucleus;
        planet[0][1] = Star1;
        planet[0][2] = Star2;
        planet[0][3] = Star3;
        planet[0][4] = Star4;
        planet[0][5] = Star5;
        planet[0][6] = Star6;
        planet[0][7] = Star7;
        planet[0][8] = Star8;

        // copy & change each planet for the FT time formula
        planet[1] = planet[0];
        for (size_t i = 0; i < planet[1].size(); i ++)
        {
            planet[1][i].f = Planet::Planet::FR2;
            planet[1][i].h = H[1];
            planet[1][i].c = planet[1][i].c.dark();
        }

        stats.resize(planet[0].size());
        break;

    // pioneer 10
    case V1:
        planet.resize(2);

        // store the Sun & the photon using the Newton time formula
        planet[0].resize(2);
        planet[0][0] = Sun;
        planet[0][1] = Pioneer1;

        // store the Sun & the photon using the FT time formula
        planet[1].resize(2);
        planet[1][0] = Sun;
        planet[1][1] = Pioneer2;

        stats.resize(planet[0].size());
        break;
    }

    for (size_t i = 1; i < planet[0].size(); ++ i)
    {
        for (size_t j = 0; j < planet.size(); ++ j)
        {
            planet[j][i].o[0] = planet[j][i].p[0];
            planet[j][i].o[1] = planet[j][i].p[1];
            planet[j][i].o[2] = planet[j][i].p[2];
        }
    }

    buffer.fill( Qt::black );
    repaint();
}

void Canvas::mousePressEvent( QMouseEvent *e )
{
    mousePressed = true;
    polyline[2] = polyline[1] = polyline[0] = e->pos();
}

void Canvas::mouseReleaseEvent( QMouseEvent * )
{
    mousePressed = false;
}

void Canvas::mouseMoveEvent( QMouseEvent *e )
{
    if ( mousePressed ) {
        QPainter painter;
        painter.begin( &buffer );
        painter.setPen( Qt::gray );
        polyline[2] = polyline[1];
        polyline[1] = polyline[0];
        polyline[0] = e->pos();
        painter.drawPolyline( polyline );
        painter.end();

        QRect r = polyline.boundingRect();
        r = r.normalized();
        r.setLeft( r.left() - pen.width() );
        r.setTop( r.top() - pen.width() );
        r.setRight( r.right() + pen.width() );
        r.setBottom( r.bottom() + pen.width() );

        bitBlt( this, r.x(), r.y(), &buffer, r.x(), r.y(), r.width(), r.height() );
    }
}

void Canvas::resizeEvent( QResizeEvent *e )
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

void Canvas::paintEvent( QPaintEvent *e )
{
    QWidget::paintEvent( e );

    QVector<QRect> rects = e->region().rects();
    for ( uint i = 0; i < rects.count(); ++ i ) 
	{
        QRect r = rects[(int)i];
        bitBlt( this, r.x(), r.y(), &buffer, r.x(), r.y(), r.width(), r.height() );
    }
}

void Canvas::setFramework(QString const & f)
{
    framework = f;
}
