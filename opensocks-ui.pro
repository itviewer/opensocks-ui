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

# Copies the given files to the destination directory
defineTest(copyToDestDir) {
    files = $$1
    DDIR = $$DESTDIR/../

    for(FILE, files) {
        FILE = $$PWD/$$FILE
        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g

        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

defineTest(copyIcon) {
    files = $$1
    DDIR = $$PWD/installer/config

    for(FILE, files) {
        FILE = $$PWD/$$FILE
        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g

        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

VERSION = 2.0.0

macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 14.0

    HEADERS += macdockiconhandler.h
    SOURCES += macdockiconhandler.mm

    INCLUDEPATH += $$PWD
    LIBS += -L$$PWD -lopensocks -framework Security

    QMAKE_TARGET_BUNDLE_PREFIX = pro.opensocks

    TARGET = OpenSocks
    # QMAKE_APPLE_DEVICE_ARCHS = x86_64
    ICON = resource/mac/icon.icns
    QMAKE_INFO_PLIST = resource/mac/Info.plist
    DESTDIR = $$PWD/out/bin

    copyIcon(resource/mac/icon.icns)
}

win32 {
    INCLUDEPATH += $$PWD
    # 添加要链接的静态库文件
    LIBS += -L$$PWD -lopensocks

    RC_ICONS = resource\windows\icon.ico
    QMAKE_TARGET_PRODUCT = "OpenSocks UI"
    QMAKE_TARGET_COMPANY = "https://anylink.pro"
    QMAKE_TARGET_DESCRIPTION = "OpenSocks GUI Client"
    QMAKE_TARGET_COPYRIGHT = "Copyright 2023-2024 https://anylink.pro. All rights reserved."

    DESTDIR = $$PWD/out/bin
    copyIcon(assets/icon.png resource/windows/icon.ico)
}

linux:!android {
    # 包含静态库头文件所在的目录
    INCLUDEPATH += $$PWD
    # 添加要链接的静态库文件
    LIBS += -L$$PWD -lopensocks

    DESTDIR = $$PWD/out/opt/opensocks/bin
    copyToDestDir(assets/icon.png resource/linux/opensocks-ui.desktop)
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


RESOURCES += \
    icons.qrc
