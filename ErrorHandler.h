#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <QMessageBox>
#include <WinSock2.h>

/*------------------------------------------------------------------------------------------------------------------
-- HEADER FILE: ErrorHandler.h - Generic error handler struct.
--
-- PROGRAM: Knekt
--
-- FUNCTIONS:
-- static void showMessage(QString msg)
--
-- DATE: Feb 12, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Henry Ho
--
-- PROGRAMMER: Henry Ho
--
-- NOTES:
-- (N/A)
------------------------------------------------------------------------------------------------------------------*/
struct ErrorHandler {

    /*------------------------------------------------------------------------------------------------------------------
    -- FUNCTION: showMessage(QString msg)
    --
    -- DATE: Feb 12, 2020
    --
    -- REVISIONS: N/A
    --
    -- DESIGNER: Henry Ho
    --
    -- PROGRAMMER: Henry Ho
    --
    -- INTERFACE: static void showMessage(QString msg)
    --              QString msg - the string message to display
    --
    -- RETURNS: void
    --
    -- NOTES:
    -- Use this function in the main thread to display a an error dialog box.
    ------------------------------------------------------------------------------------------------------------------*/
    static void showMessage(QString msg) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error",msg);
        messageBox.setFixedSize(500,200);
    }
};
#endif // ERRORHANDLER_H
