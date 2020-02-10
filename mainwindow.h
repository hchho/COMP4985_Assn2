void on_sendFileBtn_clicked();
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <ConnectivityManager.h>
#include <ConnectionType.h>
#include <fstream>
#include <mutex>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

static std::mutex mtx;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static DWORD WINAPI UIThread(void* param);
    static DWORD WINAPI SendThread(void* param);
    static DWORD WINAPI TimerThread(void* param);
    Connection* getConnection() const {
        return currConnection;
    }
    HANDLE getUIThreadHandle() const {
        return UIThreadHandle;
    }
    HANDLE getSendThreadHandle() const {
        return SendThreadHandle;
    }
    void setSentData(unsigned long bytesSent, unsigned int packets);
    void setReceivedData(unsigned long bytesReceived, unsigned int packets);
    void setTimeElapsedOutput(int);

    int getPacketSize();
    int getNumberOfTimesToSend();
    bool getIsSavedInputBoxChecked();

private slots:
    void on_actionTCP_triggered();
    void on_actionUDP_triggered();

    void on_actionClient_triggered();
    void on_actionServer_triggered();

    void on_connectBtn_clicked();

    void on_sendPacketBtn_clicked();

    void on_sendFileBtn_clicked();

    void on_receiveBtn_clicked();

private:
    Ui::MainWindow *ui;
    ConnectionType connectionType;
    ProtocolType protocolType;
    Connection* currConnection;
    bool isConnected = false;
    bool isReceiving = false;
    HANDLE SendThreadHandle;
    DWORD SendThreadId;
    HANDLE TimerThreadHandle;
    DWORD TimerThreadId;
    HANDLE UIThreadHandle;
    DWORD UIThreadId;
};

#endif // MAINWINDOW_H
