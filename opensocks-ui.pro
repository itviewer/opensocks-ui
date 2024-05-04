QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

macx {
    HEADERS += macdockiconhandler.h
    SOURCES += macdockiconhandler.mm
}

linux:!android {
    # 包含静态库头文件所在的目录
    INCLUDEPATH += /media/itviewer/linux/GoProjects/opensocks
    # 添加要链接的静态库文件
    LIBS += -L/media/itviewer/linux/GoProjects/opensocks -lopensocks
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
