QT += widgets multimedia multimediawidgets webkitwidgets

HEADERS       = analogclock.h \
  canvas.h \
  imageviewer.h \
  videoplayer.h
SOURCES       = analogclock.cpp \
                canvas.cpp \
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
