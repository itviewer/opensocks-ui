QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32|macx|linux:!android {
    include(3rdparty/SingleApplication/singleapplication.pri)
    # used by SingleApplication
    DEFINES += QAPPLICATION_CLASS=QApplication
}

macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 14.0

    HEADERS += macdockiconhandler.h
    SOURCES += macdockiconhandler.mm

    INCLUDEPATH += $$PWD
    LIBS += -L$$PWD -lopensocks
}

linux:!android {
    # 包含静态库头文件所在的目录
    INCLUDEPATH += $$PWD
    # 添加要链接的静态库文件
    LIBS += -L$$PWD -lopensocks
}

SOURCES += \
    common.cpp \
    configmanager.cpp \
    main.cpp \
    opensocks.cpp \
    textbrowser.cpp

HEADERS += \
    common.h \
    configmanager.h \
    opensocks.h \
    textbrowser.h

FORMS += \
    opensocks.ui \
    textbrowser.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc
