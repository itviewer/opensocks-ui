#include "opensocks.h"
#include "common.h"
#include "configmanager.h"
#include "libopensocks.h"
#include "textbrowser.h"
#include "ui_opensocks.h"

#include <QByteArray>
#include <QCloseEvent>
#include <QDebug>
#include <QJsonDocument>
#include <QThreadPool>
#include <QtWidgets>

OpenSocks::OpenSocks(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OpenSocks)
    , proxyRunning(false)
    , activeStop(false)
{
    ui->setupUi(this);

#ifndef Q_OS_MACOS
    setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
#else
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint
                   | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
#endif
    setWindowTitle(appName + " v" + appVersion);

    setFixedSize(geometry().width(), geometry().height());
    center();

    connect(ui->buttonStartStop, &QPushButton::clicked, this, &OpenSocks::startStopProxy);
}

OpenSocks::~OpenSocks()
{
    delete ui;
}

void OpenSocks::closeEvent(QCloseEvent *event)
{
    if (proxyRunning) {
        hide();
        event->accept();
        if (!trayIcon->isVisible()) {
            trayIcon->show();
        }
    } else {
        qApp->quit();
    }
}

void OpenSocks::showEvent(QShowEvent *event)
{
    if (trayIcon == nullptr) {
        QTimer::singleShot(50, this, [this]() { afterShowOneTime(); });
    }
    event->accept();
}

void OpenSocks::center()
{
    QRect screenGeometry = screen()->geometry();
    QRect windowGeometry = frameGeometry();
    QPoint centerPoint = screenGeometry.center() - windowGeometry.center();
    if (screen()->devicePixelRatio() > 1.0) {
        centerPoint -= QPoint(0, 120);
    }
    // 将窗口移动到居中位置
    move(centerPoint);
}

void OpenSocks::createTrayActions()
{
    actionStart = new QAction(tr("Start Proxy"), this);
    // not lambda must have this
    connect(actionStart, &QAction::triggered, this, &OpenSocks::startStopProxy);

    actionStop = new QAction(tr("Stop Proxy"), this);
    connect(actionStop, &QAction::triggered, this, &OpenSocks::startStopProxy);

    actionConfig = new QAction(tr("Show Panel"), this);
    connect(actionConfig, &QAction::triggered, this, [this]() {
        // 有最小化按钮并最小化时 show 不起作用
        showNormal();
    });

    actionQuit = new QAction(tr("Quit"), this);
    connect(actionQuit, &QAction::triggered, qApp, &QApplication::quit, Qt::QueuedConnection);
}

void OpenSocks::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(actionStart);
    trayIconMenu->addAction(actionStop);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(actionConfig);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(actionQuit);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(icon);

#if defined(Q_OS_MACOS)
    // Note: On macOS, the Dock icon is used to provide the tray's functionality.
    MacDockIconHandler *dockIconHandler = MacDockIconHandler::instance();
    connect(dockIconHandler, &MacDockIconHandler::dockIconClicked, this, [this]() { showNormal(); });
    trayIconMenu->setAsDockMenu();
#endif

#if defined(Q_OS_WIN)
    connect(trayIcon,
            &QSystemTrayIcon::activated,
            this,
            [this](QSystemTrayIcon::ActivationReason reason) {
                if (reason == QSystemTrayIcon::Trigger) {
                    showNormal();
                }
            });
#endif
}

void OpenSocks::initConfig()
{
    configManager->config["LogPath"] = tempLocation;

    // 即时保存配置好处是避免配置未更改时退出应用也重新保存配置
    ui->lineEditServerAddr->setText(configManager->config["ServerAddr"].toString());
    ui->lineEditKey->setText(configManager->config["Key"].toString());

    connect(ui->lineEditServerAddr, &QLineEdit::editingFinished, this, [this]() {
        configManager->config["ServerAddr"] = ui->lineEditServerAddr->text();
        saveConfig();
    });
    connect(ui->lineEditKey, &QLineEdit::editingFinished, this, [this]() {
        configManager->config["Key"] = ui->lineEditKey->text();
        saveConfig();
    });

    const QString protocol = configManager->config["Protocol"].toString();
    for (auto *button : ui->buttonGroupProtocol->buttons()) {
        if (button->text() == protocol) {
            button->setChecked(true);
            break;
        }
    }
    connect(ui->buttonGroupProtocol,
            &QButtonGroup::buttonToggled,
            this,
            [this](QAbstractButton *button, bool checked) {
                if (checked) {
                    configManager->config["Protocol"] = button->text();
                    saveConfig();
                }
            });

    ui->lineEditSocks5Addr->setText(configManager->config["LocalAddr"].toString());
    ui->lineEditHttpAddr->setText(configManager->config["LocalHttpProxyAddr"].toString());

    connect(ui->lineEditSocks5Addr, &QLineEdit::editingFinished, this, [this]() {
        configManager->config["LocalAddr"] = ui->lineEditSocks5Addr->text();
        saveConfig();
    });
    connect(ui->lineEditHttpAddr, &QLineEdit::editingFinished, this, [this]() {
        configManager->config["LocalHttpProxyAddr"] = ui->lineEditHttpAddr->text();
        saveConfig();
    });

    ui->checkBoxBypass->setChecked(configManager->config["Bypass"].toBool());
    ui->checkBoxCompression->setChecked(configManager->config["Compress"].toBool());
    ui->checkBoxObfuscation->setChecked(configManager->config["Obfs"].toBool());
    ui->checkBoxHttpProxy->setChecked(configManager->config["HttpProxy"].toBool());
    ui->checkBoxDebug->setChecked(configManager->config["Debug"].toBool());

    connect(ui->checkBoxBypass, &QCheckBox::toggled, this, [this](bool checked) {
        configManager->config["Bypass"] = checked;
        saveConfig();
    });
    connect(ui->checkBoxCompression, &QCheckBox::toggled, this, [this](bool checked) {
        configManager->config["Compress"] = checked;
        saveConfig();
    });
    connect(ui->checkBoxObfuscation, &QCheckBox::toggled, this, [this](bool checked) {
        configManager->config["Obfs"] = checked;
        saveConfig();
    });
    connect(ui->checkBoxHttpProxy, &QCheckBox::toggled, this, [this](bool checked) {
        configManager->config["HttpProxy"] = checked;
        saveConfig();
    });
    connect(ui->checkBoxDebug, &QCheckBox::toggled, this, [this](bool checked) {
        configManager->config["Debug"] = checked;
        saveConfig();
    });
}

void OpenSocks::afterShowOneTime()
{
    createTrayActions();
    createTrayIcon();
    initConfig();

    connect(qApp, &QApplication::aboutToQuit, this, [this]() {
        if (proxyRunning) {
            stopProxy();
        }
    });

    connect(this, &OpenSocks::proxyStopped, this, &OpenSocks::onProxyStopped);

    connect(&timer, &QTimer::timeout, this, [this]() {
        ui->labelDownload->setText(apiGetDownloadByteSize());
        ui->labelUpload->setText(apiGetUploadByteSize());
    });
}

void OpenSocks::stopProxy()
{
    activeStop = true;
    apiStopClient();
}

void OpenSocks::saveConfig()
{
    configManager->saveConfig();
}

void OpenSocks::startStopProxy()
{
    if (proxyRunning) {
        stopProxy();
    } else {
        activeStop = false;
        QThreadPool::globalInstance()->start(
            [this]() {
                // 以下代码在新线程运行

                // 假设启动成功
                proxyRunning = true;

                auto config = QJsonDocument(configManager->config).toJson().toStdString();
                auto c_config = config.c_str();

                // qDebug() << c_config;

                GoString jsonConfig = {c_config, static_cast<ptrdiff_t>(strlen(c_config))};
                // 进入 golang 事件循环，OpenSocksStop 函数会退出事件循环
                apiStartClient(jsonConfig);

                // 退出 golang 事件循环

                // 信号在新线程，槽函数在父线程
                emit proxyStopped();
            },
            QThread::HighPriority);

        QTimer::singleShot(300, this, [this]() {
            if (proxyRunning) {
                ui->buttonStartStop->setText(tr("Stop"));
                actionStart->setEnabled(false);
                actionStop->setEnabled(true);
                close();

                timer.start(10 * 1000);
            }
        });
    }
}

void OpenSocks::onProxyStopped()
{
    if (proxyRunning) {
        proxyRunning = false;

        ui->buttonStartStop->setText(tr("Start"));
        actionStart->setEnabled(true);
        actionStop->setEnabled(false);
        ui->labelDownload->setText(apiGetDownloadByteSize());
        ui->labelUpload->setText(apiGetUploadByteSize());

        timer.stop();
    }
    if (!activeStop) {
        showNormal();
        error(tr("Something went wrong, please check the log!"), this);
    }
}

void OpenSocks::on_buttonLog_clicked()
{
    QString filePath = tempLocation + "/opensocks.log";
    QFile loadFile(filePath);
    if (!loadFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        error(tr("Couldn't open log file"), this);
        return;
    }
    TextBrowser textBrowser(tr("Log Viewer"), this);

    QString data = loadFile.readAll();
    textBrowser.setText(data);
    loadFile.close();

    // 创建文件系统监视器
    QFileSystemWatcher watcher;
    watcher.addPath(filePath);

    // 监视文件变化的信号槽连接
    QObject::connect(&watcher, &QFileSystemWatcher::fileChanged, this, [&]() {
        QFile updatedFile(filePath);
        if (updatedFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            // 重新读取文件内容
            data = updatedFile.readAll();
            textBrowser.setText(data);
            updatedFile.close();
        }
    });

    textBrowser.exec();
}
