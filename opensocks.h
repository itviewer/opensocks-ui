#ifndef OPENSOCKS_H
#define OPENSOCKS_H

#include <QIcon>
#include <QTimer>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class OpenSocks;
}
QT_END_NAMESPACE

class QSystemTrayIcon;
class QMenu;

class OpenSocks : public QWidget
{
    Q_OBJECT

public:
    OpenSocks(QWidget *parent = nullptr);
    ~OpenSocks();

signals:
    // void proxyStarted();
    void proxyStopped();

protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    Ui::OpenSocks *ui;
    bool proxyRunning, activeStop;
    QSystemTrayIcon *trayIcon = nullptr;
    QMenu *trayIconMenu;
    QAction *actionStart, *actionStop, *actionQuit, *actionConfig;
    QIcon icon = QIcon(":/assets/icon.png");
    QTimer timer;

    void center();

    void createTrayActions();
    void createTrayIcon();
    void initConfig();
    void afterShowOneTime();

    void stopProxy();
    void saveConfig();

private slots:
    void startStopProxy();
    void onProxyStopped();
    void on_buttonLog_clicked();
};
#endif // OPENSOCKS_H
