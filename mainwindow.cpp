#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Please choose connection type and action!");
        messageBox.setFixedSize(500,200);
        return;
    }

    QString rawPort = ui->portInput->text();
    if (rawPort.isEmpty()) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Please enter the port number");
        messageBox.setFixedSize(500,200);
        return;
    }

    int port = rawPort.toInt();
    isConnected = !isConnected;
    if (isConnected) {
        ui->connectBtn->setText("Stop connection");
        if (connectionType == ConnectionType::CLIENT && ui->ipAddressInput->text().isEmpty()) {
            QMessageBox messageBox;
            messageBox.critical(0,"Error","Please enter the IP address");
            messageBox.setFixedSize(500,200);
            return;
        }
        QString rawIp = ui->ipAddressInput->text();
        const char *ipAddress = rawIp.toLocal8Bit().data();
        currConnection = ConnectivityManager::instance()->initializeConnection(connectionType, protocolType, port, ipAddress);
    } else {
        ui->connectBtn->setText("Connect");
    }
}
