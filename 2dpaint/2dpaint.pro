QT += widgets opengl openglwidgets

HEADERS     = \
              box.hpp \
              cell.hpp \
              circle.hpp \
              geometryengine.hpp \
              glwidget.hpp \
              ishape.hpp \
              layer.hpp \
              layout.hpp \
              layoutManager.hpp \
              mainwindow.hpp \
              mainwindow.hpp \
              oasisFileManager.hpp \
              oasisIO.hpp \
              polygon.hpp \
              trapezoid.hpp

SOURCES     = \
              geometryengine.cpp \
              glwidget.cpp \
              ishape.cpp \
              layoutManager.cpp \
              main.cpp \
              mainwindow.cpp \
              oasisFileManager.cpp

# install
target.path = 2dpaint
INSTALLS += target

INCLUDEPATH += "C:/Program Files/boost/boost_1_85_0"

DISTFILES += \
  fshader.glsl \
  vshader.glsl



