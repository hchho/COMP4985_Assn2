#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string>
#include <QMessageBox>
#include <WinSock2.h>

struct ErrorHandler {
    static void showMessage(QString msg) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error",msg);
        messageBox.setFixedSize(500,200);
    }
};
#endif // ERRORHANDLER_H
