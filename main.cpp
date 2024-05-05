#include "common.h"
#include "configmanager.h"
#include "opensocks.h"

#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#if defined(Q_OS_LINUX) || defined(Q_OS_MACOS) || defined(Q_OS_WIN)
#include "singleapplication.h"
#endif

int main(int argc, char *argv[])
{
    qSetMessagePattern("%{type}:[%{file}:%{line}]  %{message}");
    //    qDebug() << QStyleFactory::keys();
    //    QApplication::setStyle("fusion");
    QApplication::setApplicationName(appName);
    configLocation = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configLocation);

    //    qDebug() << configLocation;
    tempLocation = QStandardPaths::writableLocation(QStandardPaths::TempLocation);

#if defined(Q_OS_LINUX) || defined(Q_OS_MACOS) || defined(Q_OS_WIN)
    SingleApplication app(argc, argv);
#else
    QApplication app(argc, argv);
#endif

    configManager = new ConfigManager();
    configManager->loadConfig(Json);

    OpenSocks w;
    w.show();

    QApplication::setQuitOnLastWindowClosed(false);
    QObject::connect(&app, &SingleApplication::instanceStarted, &w, &OpenSocks::showNormal);

    return app.exec();
}
