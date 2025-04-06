QT       += core gui multimedia sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cdatabaseworker.cpp \
    cplaylistcontainer.cpp \
    crotaryencoderworker.cpp \
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
    crotaryencoderworker.h \
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
# INCLUDEPATH += RotaryEncoder/

# Library path and libraries to link against
LIBS += -L../../OLEDDriver/build/Debug -lCOled -lOledFont8x16 -lOledFont8x12 -lOledFont8x8 -lOledGraphics -lOledI2C -lOledPixel -lOledHardware -lFileDescriptor
# LIBS += -L/home/bart/Nextcloud/CPlusPlusProjects/Abschlussprojekt/MP3PlayerWifi/OLEDDriver/build/Debug -lCOled -lOledFont8x16 -lOledFont8x12 -lOledFont8x8 -lOledGraphics -lOledI2C -lOledPixel -lOledHardware -lFileDescriptor
# LIBS += -L../../RotaryEncoder/build/Debug -lcrtcdriver
LIBS += -lgpiod

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
QMAKE_LINK += -v
message("Resolved LIBDIR: $$QMAKE_LIBDIR")
message("Resolved LIBS: $$LIBS")

# # Default to dynamic linking
# CONFIG += dynamic

# # # Override for release builds
# # CONFIG(release, debug|release) {
# #     CONFIG += static
# #     LIBS += -static
# # }

# # Release configuration
# CONFIG(release, debug|release) {
#     CONFIG += static
#     # Prevent linking against .so files
#     QMAKE_LFLAGS += -static
#     # Explicitly link against static libraries
#     LIBS += -L$$[QT_INSTALL_LIBS] -lQt6Core -lQt6Gui -lQt6Widgets -lQt6Multimedia
#     # For taglib
#     LIBS += -L$$PWD/myTaglib -ltag
# }

RESOURCES += \
    icons.qrc
