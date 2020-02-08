#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ErrorHandler.h"
#include "SocketInfo.h"

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
        HANDLE sendThreadHandle;
        DWORD sendThreadId;

        SEND_INFO *sendInfo = (SEND_INFO*)malloc(sizeof(SEND_INFO));

        int numberOfBytesToSend = getPacketSize();
        int numberOfTimesToSend = getNumberOfTimesToSend();

        sendInfo->connection = currConnection;
        sendInfo->packetSize = numberOfBytesToSend;
        sendInfo->numberOfTimesToSend = numberOfTimesToSend;
        sendInfo->ui = ui;

        sendThreadHandle = CreateThread(NULL, 0, SendThread, (void *) sendInfo, 0, &sendThreadId);

        ui->sendPacketBtn->setText("Sending...");
        if(WaitForSingleObject(sendThreadHandle, INFINITE)) {
            ErrorHandler::showMessage("Error sending packets");
        }
        ui->sendPacketBtn->setText("Send Packet");
        delete sendInfo;
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

DWORD WINAPI MainWindow::SendThread(void* param) {
    SEND_INFO* sendInfo = (SEND_INFO*) param;

    Connection* connection = sendInfo->connection;
    Ui::MainWindow *ui = sendInfo->ui;
    int packetSize = sendInfo->packetSize;
    int count = sendInfo->numberOfTimesToSend;

    char* output = (char*)malloc(packetSize);
    memset(output, 'a', packetSize);

    for(int i = 0; i < count; i++) {
        if (connection->sendToServer(output) == SOCKET_ERROR) {
            perror("Error sending");
            return FALSE;
        }
        ui->bytesSentOutput->setText(QString::number(packetSize * (i+ 1)));
        ui->packetsSentOutput->setText(QString::number(i + 1));
    }
    return TRUE;
}

DWORD WINAPI MainWindow::UIThread(void* param) {
    MainWindow* window = (MainWindow*) param;
    Ui::MainWindow *ui = window->getUI();
    Connection* connection = (Connection*)window->getConnection();
    LPSOCKET_INFORMATION socketInfo = connection->getSocketInfo();

    char* buf = (char*)malloc(DATA_BUFSIZE);
    memset(buf, 0, DATA_BUFSIZE);
    int packetCount = 0;
    long prevSize = 0;
    int index;
    while(TRUE) {
        char c[10];
        WaitForSingleObject(connection->getAcceptEvent(), INFINITE);

        buf = socketInfo->Buffer;
        long sizeOfBuffer = std::strlen(buf);

        prevSize += sizeOfBuffer;

        itoa(prevSize, c, 10);
        std::string rawInput{c};
        QString input = QString::fromStdString(rawInput);

        ui->packetsReceivedOutput->setText(QString::number(++packetCount));
        ui->bytesReceivedOutput->setText(input);

        memset(buf, 0, DATA_BUFSIZE);
    }
    return TRUE;
}
