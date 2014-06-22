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
    ../include/ECS/EntityTypePool.h \
    ../include/ECS/EntityHeaderComponent.h \
    ../include/ECS/TypeIdMask.h \
    ../include/ECS/Components.h \
    ../include/ECS/EntityTypeHeaderComponent.h \
    ../include/ECS/InternalComponents.h \
    ../include/ECS/StaticComponentsPool.h \
    ../include/ECS/DynamicComponentsPool.h

INCLUDEPATH += ../include

CONFIG(debug,debug|release) {
  BUILD=debug
} else {
  BUILD=release
}

#for profiling (very sleepy)
QMAKE_CXXFLAGS_RELEASE += /Zi
QMAKE_LFLAGS_RELEASE += /DEBUG

unix {
#  QMAKE_CXXFLAGS_RELEASE += -O3 -fopenmp -D_GLIBCXX_PARALLEL
#  QMAKE_LFLAGS_RELEASE += -fopenmp
}

win32 {
#  QMAKE_CXXFLAGS_RELEASE += /openmp
}
