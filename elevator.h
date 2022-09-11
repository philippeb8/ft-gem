#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <QWidget>

class Elevator : public QWidget
{
    Q_OBJECT

public:
    Elevator( QWidget *parent );
    ~Elevator();

protected slots:
    void reset();

protected:
    void resizeEvent( QResizeEvent *e );
    void paintEvent( QPaintEvent *e );
    void timerEvent( QTimerEvent *e );

    QPixmap buffer;

    int pos[2], vel[2];
};

#endif // ELEVATOR_H
