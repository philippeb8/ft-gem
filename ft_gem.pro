QT += widgets multimedia multimediawidgets

HEADERS       = analogclock.h \
  imageviewer.h \
  videoplayer.h
SOURCES       = analogclock.cpp \
                imageviewer.cpp \
                main.cpp \
                videoplayer.cpp

QMAKE_PROJECT_NAME = ft_gem

FORMS += \
    mainwindow.ui

DISTFILES += \
  png/ship.png

RESOURCES += \
  resource.qrc
