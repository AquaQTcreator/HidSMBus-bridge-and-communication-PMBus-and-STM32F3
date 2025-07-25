QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.   Chislov Ivan
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cp2112.cpp \
    main.cpp \
    mainwindow.cpp \
    stmcomands.cpp

HEADERS += \
    Library/Windows/include/GPIO_Config.h \
    Library/Windows/include/SLABCP2112.h \
    SMBusConfig.h \
    cp2112.h \
    mainwindow.h \
    stmcomands.h \
    types.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += "Путь к вашему инклуду"
DEPENDPATH += "Путь к вашему инклуду"

LIBS += -L"Путь к библиотеке" -lSLABHIDtoSMBus
