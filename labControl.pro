QT       += core gui charts printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp\
    encoder.cpp\

HEADERS += \
    mainwindow.h\
    encoder.h\

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /home/pi/$${TARGET}
!isEmpty(target.path): INSTALLS += target
INCLUDEPATH += /opt/qt5pi/sysroot/usr/include

LIBS += -L"/opt/qt5pi/sysroot/usr/lib"
LIBS += -lwiringPi
