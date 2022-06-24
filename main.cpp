/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QApplication>
#include <QMainWindow>
#include <QDesktopWidget>
#include <QNetworkProxyFactory>

#include "analogclock.h"
#include "ui_mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QMainWindow win;
    Ui::MainWindow window;

    window.setupUi(& win);
    window.ship_1->loadFile(":/png/ship1.png");
    window.ship_2->loadFile(":/png/ship1.png");
    window.ship_3->loadFile(":/png/ship2.png");
    window.ship_4->loadFile(":/png/ship1.png");
    window.missile_3->loadFile(":/png/missile.png");
    window.video_1->setUrl(QUrl("qrc:/mp4/drive.mp4"));
    window.video_2->setUrl(QUrl("qrc:/mp4/lorentz.mp4"));
    window.tides_1->loadFile(":/png/tides.png");
    window.dipole_1->loadFile(":/png/dipoles.png");

    QRect rec = QApplication::desktop()->screenGeometry();
    win.resize(rec.width(), rec.height());
    win.show();

    window.clock_1->startAt(100);
    window.clock_2->startAt(1);
    window.ship_1->startAt(100, 2.0);
    window.ship_2->startAt(1, 2.0, 0.0, -1, -1, Qt::darkBlue);
    window.missile_3->startAt(100, -300.0, 40.0, rec.width(), 100);
    window.ship_4->startFor(1, rec.width() / 2);
    window.video_1->play();

    return app.exec();
}
