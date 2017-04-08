TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread -levent -levent_pthreads

SOURCES += main.cpp \
    httpServer.cpp \
    log.cpp \
    httpTask.cpp \
    thread.cpp \
    threadPool.cpp

HEADERS += \
    httpServer.h \
    log.h \
    thread.h \
    threadPool.h \
    httpTask.h \
    task.h

DISTFILES += \
    CMakeLists.txt
