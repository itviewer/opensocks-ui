#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "common.h"
#include <QObject>
#include <QJsonObject>

class ConfigManager : public QObject
{
    Q_OBJECT
public:
    explicit ConfigManager(QObject *parent = nullptr);

    QJsonObject config{{"Bypass", true},
                       {"Compress", false},
                       {"HttpProxy", true},
                       {"Key", ""},
                       {"LocalAddr", "127.0.0.1:1080"},
                       {"LocalHttpProxyAddr", "127.0.0.1:8008"},
                       {"Obfs", false},
                       {"Protocol", "tcp"},
                       {"ServerAddr", "server:1080"},
                       {"Debug", false},
                       {"LogPath", ""}};
    bool loadConfig(SaveFormat saveFormat);
    void saveConfig(SaveFormat saveFormat);
    void saveConfig();
};

#endif // CONFIGMANAGER_H
