#ifndef UNIVERSE_H
#define UNIVERSE_H

#include <QWidget>

class Universe : public QWidget
{
    Q_OBJECT

public:
    enum Framework {eGR, eFT} eFramework = eGR;

    Universe( QWidget *parent );

    void setFramework(Framework aFramework);

    ~Universe();

protected slots:
    void reset();

protected:
    void resizeEvent( QResizeEvent *e );
    void paintEvent( QPaintEvent *e );
    void timerEvent( QTimerEvent *e );

    QPixmap buffer;

    int pos[1], vel[1];
};

#endif // Universe_H
