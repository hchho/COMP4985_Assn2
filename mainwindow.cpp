#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ErrorHandler.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->receiveBtn->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionTCP_triggered()
{
    ui->actionUDP->setChecked(false);
    protocolType = ProtocolType::TCP;
}

void MainWindow::on_actionUDP_triggered()
{
    ui->actionTCP->setChecked(false);
    protocolType = ProtocolType::UDP;
}

void MainWindow::on_actionClient_triggered()
{
    ui->actionServer->setChecked(false);
    ui->ipAddressInput->setHidden(!ui->actionClient->isChecked());
    ui->ipAddressLabel->setHidden(!ui->actionClient->isChecked());
    connectionType = ConnectionType::CLIENT;
}

void MainWindow::on_actionServer_triggered()
{
    ui->actionClient->setChecked(false);
    ui->ipAddressInput->setHidden(ui->actionServer->isChecked());
    ui->ipAddressLabel->setHidden(ui->actionServer->isChecked());
    connectionType = ConnectionType::SERVER;
}

void MainWindow::on_connectBtn_clicked()
{
    if (!(ui->actionServer->isChecked() ||
          ui->actionClient->isChecked()) ||
            !(ui->actionTCP->isChecked() ||
              ui->actionUDP->isChecked())) {
        ErrorHandler::showMessage("Please choose connection type and action!");
        return;
    }

    QString rawPort = ui->portInput->text();
    if (rawPort.isEmpty()) {
        ErrorHandler::showMessage("Please enter port number");
        return;
    }

    int port = rawPort.toInt();
    isConnected = !isConnected;
    if (isConnected) {
        ui->connectBtn->setText("Stop connection");
        if (connectionType == ConnectionType::CLIENT && ui->ipAddressInput->text().isEmpty()) {
            ErrorHandler::showMessage("Please input IP address");
            return;
        }
        QString rawIp = ui->ipAddressInput->text();
        std::string stringIp = rawIp.toStdString();
        const char *ipAddress = stringIp.c_str();
        currConnection = ConnectivityManager::instance()->initializeConnection(connectionType, protocolType, port, ipAddress);

        ui->receiveBtn->setHidden(connectionType != ConnectionType::SERVER);
    } else {
        ui->connectBtn->setText("Connect");
        ui->receiveBtn->hide();
        ConnectivityManager::instance()->closeConnection(currConnection);
    }
}

void MainWindow::on_sendPacketBtn_clicked()
{
    if (isConnected && connectionType == ConnectionType::CLIENT && currConnection->sendToServer("Hello world") == -1) {
        ErrorHandler::showMessage("Error sending data");
    }
}

void MainWindow::on_receiveBtn_clicked()
{
    isReceiving = !isReceiving;
    if (isReceiving) {
        currConnection->startRoutine();
        ui->receiveBtn->setText("Stop receiving");
        if ((UIThreadHandle = CreateThread(NULL, 0, UIThread, (LPVOID) this, 0, &UIThreadId)) == NULL)
        {
            ErrorHandler::showMessage("Error creating thread");
            exit(1);
        }
    } else {
        currConnection->stopRoutine();
        ui->receiveBtn->setText("Begin receiving");
        CloseHandle(UIThreadHandle);
    }
}

DWORD WINAPI MainWindow::UIThread(void* param) {
    MainWindow* window = (MainWindow*) param;
    Ui::MainWindow *ui = window->getUI();
    Connection* connection = (Connection*)window->getConnection();
    LPSOCKET_INFORMATION socketInfo = connection->getSocketInfo();
    while(TRUE) {
        ui->packetsReceivedOutput->setText(socketInfo->Buffer);
    }
    return TRUE;
}
