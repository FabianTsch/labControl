QT       += core gui charts multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    charts.cpp \
    daqhat.cpp \
    daqhats_utils.cpp \
    main.cpp \
    mainwindow.cpp\
    encoder.cpp\
    BTS7960.cpp

HEADERS += \
    charts.h \
    daqhats_utils.h\
    daqhat.h \
    mainwindow.h\
    encoder.h\
    BTS7960.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /home/pi/$${TARGET}
!isEmpty(target.path): INSTALLS += target
INCLUDEPATH += /opt/qt5pi/sysroot/usr/include
INCLUDEPATH += /opt/qt5pi/sysroot/usr/local/include

LIBS += -L"/opt/qt5pi/sysroot/usr/lib"
LIBS += -lwiringPi
LIBS += -pthread
LIBS += -ldaqhats
LIBS += -lrt
LIBS += -lpigpiod_if2


unix:!macx: LIBS += -L$$PWD/../../../../opt/qt5pi/sysroot/opt/vc/lib/ -lbcm_host

INCLUDEPATH += $$PWD/../../../../opt/qt5pi/sysroot/opt/vc/include
DEPENDPATH += $$PWD/../../../../opt/qt5pi/sysroot/opt/vc/include

unix:!macx: LIBS += -L$$PWD/../../../../opt/qt5pi/sysroot/opt/vc/lib/ -lvcos

INCLUDEPATH += $$PWD/../../../../opt/qt5pi/sysroot/opt/vc/include
DEPENDPATH += $$PWD/../../../../opt/qt5pi/sysroot/opt/vc/include


unix:!macx: LIBS += -L$$PWD/../../../../opt/qt5pi/sysroot/opt/vc/lib/ -lvchiq_arm

INCLUDEPATH += $$PWD/../../../../opt/qt5pi/sysroot/opt/vc/include
DEPENDPATH += $$PWD/../../../../opt/qt5pi/sysroot/opt/vc/include
