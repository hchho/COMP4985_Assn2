#define _CRT_SECURE_NO_WARNINGS

#include <QFileDialog>
#include <QDir>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ErrorHandler.h"
#include "SocketInfo.h"
#include "Helpers.h"

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
    if (isConnected && connectionType == ConnectionType::CLIENT) {

        int numberOfBytesToSend = getPacketSize();
        int numberOfTimesToSend = getNumberOfTimesToSend();

        SendThreadHandle = CreateThread(NULL, 0, SendThread, (void *) this, 0, &SendThreadId);

        ui->sendPacketBtn->setText("Sending...");
        if(WaitForSingleObject(SendThreadHandle, INFINITE)) {
            ErrorHandler::showMessage("Error sending packets");
        }
        CloseHandle(SendThreadHandle);
        ui->sendPacketBtn->setText("Send Packet");
    }
}

void MainWindow::on_receiveBtn_clicked()
{
    isReceiving = !isReceiving;
    if (isReceiving) {
        int packetSize = getPacketSize();
        currConnection->startRoutine(packetSize);
        ui->receiveBtn->setText("Stop receiving");
        if ((UIThreadHandle = CreateThread(NULL, 0, UIThread, (LPVOID) this, 0, &UIThreadId)) == NULL)
        {
            ErrorHandler::showMessage("Error creating UI thread");
            exit(1);
        }
        if ((TimerThreadHandle = CreateThread(NULL, 0, TimerThread, (LPVOID) this, 0, &TimerThreadId)) == NULL)
        {
            ErrorHandler::showMessage("Error creating timer thread");
            exit(1);
        }
    } else {
        currConnection->stopRoutine();
        ui->receiveBtn->setText("Begin receiving");
        CloseHandle(UIThreadHandle);
        CloseHandle(TimerThreadHandle);
    }
}

void MainWindow::on_sendFileBtn_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open a file", "", "Text files (*.txt)");
    currConnection->sendFileToServer(filePath.toStdString().c_str());
    LPSOCKET_INFORMATION SI = currConnection->getSocketInfo();
    ui->bytesSentOutput->setText(QString::number(SI->TotalBytesSend));
    ui->packetsSentOutput->setText(QString::number(SI->packetCount++));
    return;
}

int MainWindow::getPacketSize() {
    QComboBox *packetSizeComboBox = ui->packetSizeOptions;
    QString packetSizeValue = packetSizeComboBox->itemText(packetSizeComboBox->currentIndex());
    int numberOfBytesToSend = packetSizeValue.toInt();
    return numberOfBytesToSend;
}

int MainWindow::getNumberOfTimesToSend() {
    QComboBox *packetCountComboBox = ui->numberOfTimesOptions;
    QString packetCountValue = packetCountComboBox->itemText(packetCountComboBox->currentIndex());
    int numberOfTimesToSend = packetCountValue.toInt();
    return numberOfTimesToSend;
}

bool MainWindow::getIsSavedInputBoxChecked() {
    return ui->saveInputBox->isChecked();
}

void MainWindow::setSentData(unsigned long bytesSent, unsigned int packets) {
    ui->bytesSentOutput->setText(QString::number(bytesSent));
    ui->packetsSentOutput->setText(QString::number(packets));
}
void MainWindow::setReceivedData(unsigned long bytesReceived, unsigned int packets) {
    ui->packetsReceivedOutput->setText(QString::number(packets));
    ui->bytesReceivedOutput->setText(QString::number(bytesReceived));
}

void MainWindow::setTimeElapsedOutput(int time) {
    ui->timeElapsedOutput->setText(QString::number(time));
};

DWORD WINAPI MainWindow::SendThread(void* param) {
    int res, err;
    MainWindow* window = (MainWindow*) param;
    Connection* connection = window->getConnection();
    LPSOCKET_INFORMATION SI = connection->getSocketInfo();

    int packetSize = window->getPacketSize();
    int count = window->getNumberOfTimesToSend();

    char* output = (char*)malloc(packetSize);
    memset(output, 'a', packetSize);

    SI->TotalBytesSend = 0;
    for(int i = 0; i < count; i++) {
        if (connection->sendToServer(output) == FALSE) {
            return FALSE;
        }
        SI->TotalBytesSend += SI->BytesSEND;
        window->setSentData(SI->TotalBytesSend, SI->packetCount);
    }
    return TRUE;
}

DWORD WINAPI MainWindow::UIThread(void* param) {
    MainWindow* window = (MainWindow*) param;
    Connection* connection = (Connection*)window->getConnection();
    LPSOCKET_INFORMATION socketInfo = connection->getSocketInfo();
    bool isSavedToFile = window->getIsSavedInputBoxChecked();
    int lastBytesReceived = 0;
    while(socketInfo->Error == 0) {
        long bytesReceived = socketInfo->TotalBytesRecv;
        unsigned int packetsReceived = socketInfo->packetCount;
        window->setReceivedData(bytesReceived, packetsReceived);
        if (bytesReceived > lastBytesReceived) {
            lastBytesReceived = bytesReceived;
            if (isSavedToFile)
                writeToFile(socketInfo->Buffer);
        }
    }
    return FALSE;
}

DWORD WINAPI MainWindow::TimerThread(void* param) {
    int res;
    MainWindow* window = (MainWindow*) param;
    Connection* connection = (Connection*)window->getConnection();
    LPSOCKET_INFORMATION socketInfo = connection->getSocketInfo();
    int counter = 0;
    window->setTimeElapsedOutput(counter);
    while(TRUE) {
        if ((res = WaitForSingleObject(window->getUIThreadHandle(), 1)) > 0) {
            if (res == WAIT_TIMEOUT) {
                if (socketInfo->TotalBytesRecv > 0)
                    window->setTimeElapsedOutput(++counter);
                continue;
            }
            else {
                perror("Error waiting");
                int error = GetLastError();
                return FALSE;
            }
        } else {
            break;
        }
    }
    return FALSE;
}
