#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <ConnectivityManager.h>
#include <ConnectionType.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static DWORD WINAPI UIThread(void* param);
    Ui::MainWindow* getUI() const {
        return ui;
    }
    Connection* getConnection() const {
        return currConnection;
    }

private slots:
    void on_actionTCP_triggered();
    void on_actionUDP_triggered();

    void on_actionClient_triggered();
    void on_actionServer_triggered();

    void on_connectBtn_clicked();

    void on_sendPacketBtn_clicked();

    void on_receiveBtn_clicked();

private:
    Ui::MainWindow *ui;
    ConnectionType connectionType;
    ProtocolType protocolType;
    Connection* currConnection;
    bool isConnected = false;
    bool isReceiving = false;
    HANDLE UIThreadHandle;
    DWORD UIThreadId;
};
#endif // MAINWINDOW_H
