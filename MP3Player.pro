QT       += core gui multimedia sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cdatabaseworker.cpp \
    cplaylistcontainer.cpp \
    ctrack.cpp \
    dbconnect.cpp \
    dialogaddplaylist.cpp \
    dialogmanagement.cpp \
    dialogprogress.cpp \
    dialogsearch.cpp \
    dialogsettings.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    cdatabaseworker.h \
    cplaylistcontainer.h \
    ctrack.h \
    dbconnect.h \
    dialogaddplaylist.h \
    dialogmanagement.h \
    dialogprogress.h \
    dialogsearch.h \
    dialogsettings.h \
    mainwindow.h

FORMS += \
    dialogaddplaylist.ui \
    dialogmanagement.ui \
    dialogprogress.ui \
    dialogsearch.ui \
    dialogsettings.ui \
    mainwindow.ui

# to use the oled drivers
# Include path (relative to the .pro file)
INCLUDEPATH += OLEDDriver/

# Library path and libraries to link against
LIBS += -LOLEDDriver/build/Debug -lCOled -lOledFont8x16 -lOledFont8x12 -lOledFont8x8 -lOledGraphics -lOledI2C -lOledPixel -lOledHardware -lFileDescriptor

# for the use of the tag libraries
INCLUDEPATH += $$PWD/myTaglib
DEPENDPATH += $$PWD/myTaglib
LIBS += -L$$PWD/myTaglib -ltag

QMAKE_CXXFLAGS += -Wall -Wextra -std=c++20
# Set runtime library path (rpath)
QMAKE_LFLAGS += -Wl,-rpath,OLEDDriver/build/Debug

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    .gitignore

# for debugging
# QMAKE_LINK += -v
