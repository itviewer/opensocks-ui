#include "common.h"
#include "configmanager.h"
#include "opensocks.h"

#include <QApplication>
#include <QDir>
#include <QStandardPaths>

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

    QApplication a(argc, argv);

    configManager = new ConfigManager();
    configManager->loadConfig(Json);

    OpenSocks w;
    w.show();

    QApplication::setQuitOnLastWindowClosed(false);
    // QObject::connect(&app, &SingleApplication::instanceStarted, &w, &AnyLink::showNormal);

    return a.exec();
}
