QT += widgets

HEADERS       = analogclock.h \
  imageviewer.h
SOURCES       = analogclock.cpp \
                imageviewer.cpp \
                main.cpp

QMAKE_PROJECT_NAME = ft_gem

FORMS += \
    mainwindow.ui

DISTFILES += \
  png/field.png \
  png/missile.png \
  png/ship.png \
  png/ship2.png
