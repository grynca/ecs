TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

SOURCES += ../test/main.cpp
HEADERS += ../include/ECS.h \
    ../include/ECS/CommonDefs.h \
    ../include/ECS/Component.h \
    ../include/ECS/EcsConfig.h \
    ../include/ECS/ECSManager.h \
    ../include/ECS/Entity.h \
    ../include/ECS/System.h \
    ../include/ECS/ComponentsRegister.h \
    ../include/ECS/EntitiesRegister.h \
    ../include/ECS/EntitiesPool.h \
    ../include/ECS/Masks.h \
    ../include/ECS/TypeTuple.h \
    ../include/ECS/EntityHeaderComponent.h \
    ../include/ECS/Offsets.h
INCLUDEPATH += ../include

CONFIG(debug,debug|release) {
  BUILD=debug
} else {
  BUILD=release
}

unix {
#  QMAKE_CXXFLAGS_RELEASE += -O3 -fopenmp -D_GLIBCXX_PARALLEL
#  QMAKE_LFLAGS_RELEASE += -fopenmp
}

win32 {
#  QMAKE_CXXFLAGS_RELEASE += /openmp
}
