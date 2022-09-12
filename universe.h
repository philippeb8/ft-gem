#ifndef UNIVERSE_H
#define UNIVERSE_H

#include <QWidget>

class Universe : public QWidget
{
    Q_OBJECT

public:
    Universe( QWidget *parent );
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
