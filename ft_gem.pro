QT += widgets multimedia multimediawidgets webkitwidgets

HEADERS       = analogclock.h \
  canvas.h \
  elevator.h \
  imageviewer.h \
  mainwindow.h \
  videoplayer.h
SOURCES       = analogclock.cpp \
                canvas.cpp \
                elevator.cpp \
                imageviewer.cpp \
                main.cpp \
                mainwindow.cpp \
                videoplayer.cpp

QMAKE_PROJECT_NAME = ft_gem

FORMS += \
    mainwindow.ui

DISTFILES += \
  png/ship.png

RESOURCES += \
  resource.qrc
