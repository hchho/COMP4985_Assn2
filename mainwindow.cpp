#include "mainwindow.h"
#include "ui_mainwindow.h"

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
}

void MainWindow::on_actionUDP_triggered()
{
    ui->actionTCP->setChecked(false);
}

void MainWindow::on_actionClient_triggered()
{
    ui->actionServer->setChecked(false);
    ui->ipAddressInput->setHidden(!ui->actionClient->isChecked());
    ui->ipAddressLabel->setHidden(!ui->actionClient->isChecked());
}

void MainWindow::on_actionServer_triggered()
{
    ui->actionClient->setChecked(false);
    ui->ipAddressInput->setHidden(ui->actionServer->isChecked());
    ui->ipAddressLabel->setHidden(ui->actionServer->isChecked());
}
