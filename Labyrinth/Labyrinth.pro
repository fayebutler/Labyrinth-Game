TARGET=Labyrinth
OBJECTS_DIR=obj
# as I want to support 4.8 and 5 this will set a flag for some of the mac stuff
# mainly in the types.h file for the setMacVisual which is native in Qt5
isEqual(QT_MAJOR_VERSION, 5) {
		cache()
		DEFINES +=QT5BUILD
}

MOC_DIR=moc
CONFIG-=app_bundle
QT+=gui opengl core
SOURCES+= src/main.cpp \
    src/NGLDraw.cpp \
    src/PhysicsWorld.cpp \
    src/CollisionShape.cpp \
    src/Text.cpp

HEADERS+= \
    include/NGLDraw.h \
    include/PhysicsWorld.h \
    include/CollisionShape.h \
    include/Text.h
INCLUDEPATH +=./include

DESTDIR=./
OTHER_FILES+= \
    shaders/PhongFragment.glsl \
    shaders/PhongVertex.glsl \
    shaders/TextureFrag.glsl \
    shaders/TextureVert.glsl

CONFIG += console
CONFIG -= app_bundle
INCLUDEPATH+=/usr/local/include/bullet
INCLUDEPATH+=/usr/local/include
LIBS+= -L/usr/local/lib -lBulletDynamics  -lBulletCollision -lLinearMath
# use this to suppress some warning from boost
unix:QMAKE_CXXFLAGS_WARN_ON += "-Wno-unused-parameter"

QMAKE_CXXFLAGS+=$$system($$(HOME)/SDL2.0/bin/sdl2-config  --cflags)
message(output from sdl2-config --cflags added to CXXFLAGS= $$QMAKE_CXXFLAGS)

LIBS+=$$system($$(HOME)/SDL2.0/bin/sdl2-config  --libs)
message(output from sdl2-config --libs added to LIB=$$LIBS)
LIBS+=-L/usr/local/lib -lSDL2_ttf

unix:QMAKE_CXXFLAGS+= -msse -msse2 -msse3
macx:QMAKE_CXXFLAGS+= -arch x86_64
macx:INCLUDEPATH+=/usr/local/include/
linux-g++:QMAKE_CXXFLAGS +=  -march=native
linux-g++-64:QMAKE_CXXFLAGS +=  -march=native
# define the _DEBUG flag for the graphics lib
DEFINES +=NGL_DEBUG

unix:LIBS += -L/usr/local/lib
# add the ngl lib
unix:LIBS +=  -L/$(HOME)/NGL/lib -l NGL

# now if we are under unix and not on a Mac (i.e. linux) define GLEW
linux-*{
		linux-*:QMAKE_CXXFLAGS +=  -march=native
		linux-*:DEFINES+=GL42
		DEFINES += LINUX
}
DEPENDPATH+=include
# if we are on a mac define DARWIN
macx:DEFINES += DARWIN
# this is where to look for includes
INCLUDEPATH += $$(HOME)/NGL/include/

win32: {
				PRE_TARGETDEPS+=C:/NGL/lib/NGL.lib
				INCLUDEPATH+=-I c:/boost
				DEFINES+=GL42
				DEFINES += WIN32
				DEFINES+=_WIN32
				DEFINES+=_USE_MATH_DEFINES
				LIBS += -LC:/NGL/lib/ -lNGL
				DEFINES+=NO_DLL
}


