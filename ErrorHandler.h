#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

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
