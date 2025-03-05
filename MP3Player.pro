QT       += core gui multimedia sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    #../Oled/OLEDTest/COled.cpp \
    dialogaddplaylist.cpp \
    dialogmanagement.cpp \
    dialogprogress.cpp \
    dialogsearch.cpp \
    dialogsettings.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    #../Oled/OLEDTest/COled.h \
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

INCLUDEPATH += /home/bart/Nextcloud/CPlusPlusProjects/Abschlussprojekt/Oled/OLEDTest

LIBS += -L/home/bart/Nextcloud/CPlusPlusProjects/Abschlussprojekt/Oled/OLEDTest/build/Debug -lCOled -lOledFont8x16 -lOledFont8x12 -lOledFont8x8 -OledGraphics -lOledI2C -lOledPixel -lOledHardware -lFileDescriptor


QMAKE_CXXFLAGS += -Wall -Wextra -std=c++20
QMAKE_LFLAGS += -Wl,-rpath,/home/bart/Nextcloud/CPlusPlusProjects/Abschlussprojekt/Oled/OLEDTest/build/Debug

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    .gitignore
