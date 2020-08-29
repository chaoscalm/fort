QT = core testlib

CONFIG += c++11 console testcase
CONFIG -= app_bundle

TARGET = test
TEMPLATE = app

UI_PWD = $$PWD/../../ui
INCLUDEPATH += $$UI_PWD

INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/main.cpp \
    test.cpp

HEADERS += \
    $$PWD/commontest.h \
    test.h

# Driver integration
include($$UI_PWD/../driver/Driver.pri)

# Windows
LIBS += -lfwpuclnt -lkernel32 -luser32 -luuid -lws2_32
