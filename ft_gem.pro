QT += widgets multimedia multimediawidgets webkitwidgets

HEADERS       = analogclock.h \
  blackhole.h \
  calculus.h \
  canvas.h \
  dd.h \
  dispatch.h \
  elevator.h \
  floating.h \
  fraction.h \
  graphui.h \
  imageviewer.h \
  mainwindow.h \
  mutable_ptr.h \
  normed.h \
  point.h \
  portability.h \
  qd.h \
  tuple.h \
  type.h \
  unistd.h \
  vector.h \
  videoplayer.h
SOURCES       = analogclock.cpp \
                blackhole.cpp \
                canvas.cpp \
                elevator.cpp \
                graphui.cpp \
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
