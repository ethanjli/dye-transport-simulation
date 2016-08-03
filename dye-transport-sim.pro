TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

SOURCES += \
    src/main.cpp \
    src/fluid-sim/math.cpp \
    src/fluid-sim/fluidsystem.cpp \
    src/graphics/shader.cpp \
    src/graphics/fluidtexture.cpp \
    src/graphics/resourcemanager.cpp \
    src/graphics/canvas.cpp \
    src/graphics/interface.cpp

HEADERS += \
    src/fluid-sim/math.h \
    src/fluid-sim/vectorfield.h \
    src/fluid-sim/vectorfield.tpp \
    src/fluid-sim/fluidsystem.h \
    src/fluid-sim/fluidsystem.tpp \
    src/graphics/shader.h \
    src/graphics/fluidtexture.h \
    src/graphics/canvas.h \
    src/graphics/resourcemanager.h \
    src/graphics/interface.h

DISTFILES += \
    src/graphics/canvas.frag \
    src/graphics/canvas.vert

shaders.path = $${OUT_PWD}/shaders
shaders.files += \
    src/graphics/canvas.frag \
    src/graphics/canvas.vert
INSTALLS += shaders

LIBS += -lGL -lGLEW -lglfw

INCLUDEPATH += ext/eigen3.3b2 ext/glm-0.9.7.6

win32: LIBS += -L$$PWD/ext/glew-2.0.0-win32/glew-2.0.0/lib/Release/x64/ -lglew32 \
    -L$$PWD/ext/glfw-3.2.bin.WIN64/lib-mingw-w64

win32: INCLUDEPATH += $$PWD/ext/glew-2.0.0-win32/glew-2.0.0/include \
    $$PWD/ext/glfw-3.2.bin.WIN64/include
win32: DEPENDPATH += $$PWD/ext/glew-2.0.0-win32/glew-2.0.0/include \
    $$PWD/ext/glfw-3.2.bin.WIN64/include
