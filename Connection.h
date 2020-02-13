#ifndef CONNECTION_H
#define CONNECTION_H

#include <errno.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <WinSock2.h>
#include "ErrorHandler.h"
#include "SocketInfo.h"

/*------------------------------------------------------------------------------------------------------------------
-- HEADER FILE: Connection.h - An abstract class that contains functions and properties of TCP and UDP connection.
--                             This file also has the concrete classes that inherit this class.
--
-- PROGRAM: Knekt
--
-- FUNCTIONS:
-- void initClientConnection(void)
-- int sendToServer(const char*)
-- int sendFileToServer(const char*)
-- void startRoutine(unsigned long)
-- void stopRoutine(void)
-- void stopConnection(void)
-- SOCKET getListenSocket(void) const
-- WSAEVENT getAcceptEvent(void) const
-- void setReceivedEvent(const WSAEVENT)
-- WSAEVENT getReceivedEvent(void) const
-- LPSOCKET_INFORMATION getSocketInfo()
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
-- This abstract class is the parent class for TCP and UDP connections. Use this class to send and to receive
-- packets.
------------------------------------------------------------------------------------------------------------------*/

class Connection {
protected:
    HANDLE ServerThreadHandle;
    DWORD ServerThreadId;
    WSAEVENT AcceptEvent;
    WSAEVENT ReceivedEvent;
    LPSOCKET_INFORMATION SocketInfo;
    SOCKET sd;
    char *buf;
    struct sockaddr_in *client, *server;
    int client_len, server_len;
public:
    Connection() = default;
    Connection(SOCKET s, struct sockaddr_in* ss) : sd(s) {
        buf = new char[DATA_BUFSIZE];
        server = (struct sockaddr_in*)malloc(sizeof(*server));

        server_len = sizeof(*server);

        memcpy(server, ss, sizeof(*server));

        if ((SocketInfo = (LPSOCKET_INFORMATION) GlobalAlloc(GPTR,
                                                             sizeof(SOCKET_INFORMATION))) == NULL)
        {
            ErrorHandler::showMessage("GlobalAlloc() failed");
        }
    }
    virtual ~Connection() {
        CloseHandle(AcceptEvent);
        CloseHandle(ReceivedEvent);
        CloseHandle(SocketInfo->EndEvent);
        delete[] buf;
        delete server;
    }
    virtual void initClientConnection() = 0;
    virtual int sendToServer(const char* data) = 0;
    virtual int sendFileToServer(const char* filePath);
    virtual void startRoutine(unsigned long) = 0;

    /*------------------------------------------------------------------------------------------------------------------
    -- FUNCTION: stopRoutine
    --
    -- DATE: Feb 12, 2020
    --
    -- REVISIONS: N/A
    --
    -- DESIGNER: Henry Ho
    --
    -- PROGRAMMER: Henry Ho
    --
    -- INTERFACE: void stopRoutine(void)
    --
    -- RETURNS: void
    --
    -- NOTES:
    -- Closes the server thread handle
    ------------------------------------------------------------------------------------------------------------------*/
    void stopRoutine() {
        CloseHandle(ServerThreadHandle);
    }

    /*------------------------------------------------------------------------------------------------------------------
    -- FUNCTION: stopConnection(void)
    --
    -- DATE: Feb 12, 2020
    --
    -- REVISIONS: N/A
    --
    -- DESIGNER: Henry Ho
    --
    -- PROGRAMMER: Henry Ho
    --
    -- INTERFACE: void stopConnection(void)
    --
    -- RETURNS: void
    --
    -- NOTES:
    -- Closes the socket descriptor and closes Windows sockets
    ------------------------------------------------------------------------------------------------------------------*/
    virtual void stopConnection() {
        closesocket(sd);
        WSACleanup();
    }

    /*------------------------------------------------------------------------------------------------------------------
    -- FUNCTION: getListenSocket(void)
    --
    -- DATE: Feb 12, 2020
    --
    -- REVISIONS: N/A
    --
    -- DESIGNER: Henry Ho
    --
    -- PROGRAMMER: Henry Ho
    --
    -- INTERFACE: SOCKET getListenSocket(void)
    --
    -- RETURNS: SOCKET
    --
    -- NOTES:
    -- Returns the listen socket descriptor
    ------------------------------------------------------------------------------------------------------------------*/
    SOCKET getListenSocket() const {
        return sd;
    }

    /*------------------------------------------------------------------------------------------------------------------
    -- FUNCTION: getAcceptEvent(void)
    --
    -- DATE: Feb 12, 2020
    --
    -- REVISIONS: N/A
    --
    -- DESIGNER: Henry Ho
    --
    -- PROGRAMMER: Henry Ho
    --
    -- INTERFACE: WSAEVENT getAcceptEvent(void)
    --
    -- RETURNS: WSAEVENT
    --
    -- NOTES:
    -- Returns the event handle for accepting a connection
    ------------------------------------------------------------------------------------------------------------------*/
    WSAEVENT getAcceptEvent() const {
        return AcceptEvent;
    }

    /*------------------------------------------------------------------------------------------------------------------
    -- FUNCTION: getSocketInfo(void)
    --
    -- DATE: Feb 12, 2020
    --
    -- REVISIONS: N/A
    --
    -- DESIGNER: Henry Ho
    --
    -- PROGRAMMER: Henry Ho
    --
    -- INTERFACE: LPSOCKET_INFORMATION getAcceptEvent(void)
    --
    -- RETURNS: LPSOCKET_INFORMATION
    --
    -- NOTES:
    -- Returns the socket information struct
    ------------------------------------------------------------------------------------------------------------------*/
    LPSOCKET_INFORMATION getSocketInfo() {
        return SocketInfo;
    }
};

/*------------------------------------------------------------------------------------------------------------------
-- CLASS: TCPConnection - A concrete class that implements Connection. It is used to handle TCP specific
--                        functions and properties.
--
-- PROGRAM: Knekt
--
-- FUNCTIONS:
-- void initClientConnection(void)
-- int sendToServer(const char*)
-- void startRoutine(unsigned long)
-- void stopRoutine(void)
-- void stopConnection(void)
-- SOCKET getAcceptSocket(void) const
-- void setAcceptSocket(const SOCKET)
-- static DWORD WINAPI WorkerThread(LPVOID)
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
-- This class represents a TCP connection. Use this class to send or to receive data across a TCP connection. This
-- class also has functions to end a TCP connection.
------------------------------------------------------------------------------------------------------------------*/
class TCPConnection : public Connection {
private:
    static int constexpr BUFSIZE = DATA_BUFSIZE;
    SOCKET AcceptSocket;
public:
    TCPConnection() = default;
    TCPConnection(SOCKET s, struct	sockaddr_in *ss) : Connection(s, ss) {}
    ~TCPConnection() {}
    int sendToServer(const char* data) override;
    void initClientConnection() override;
    void startRoutine(unsigned long) override;
    void stopConnection() override {
        closesocket(AcceptSocket);
        Connection::stopConnection();
    }

    /*------------------------------------------------------------------------------------------------------------------
    -- FUNCTION: getAcceptSocket(void)
    --
    -- DATE: Feb 12, 2020
    --
    -- REVISIONS: N/A
    --
    -- DESIGNER: Henry Ho
    --
    -- PROGRAMMER: Henry Ho
    --
    -- INTERFACE: void getAcceptSocket(void) const
    --
    -- RETURNS: void
    --
    -- NOTES:
    -- Use this function to get the socket descriptor that accepted the connection
    ------------------------------------------------------------------------------------------------------------------*/
    SOCKET getAcceptSocket() const {
        return AcceptSocket;
    }

    /*------------------------------------------------------------------------------------------------------------------
    -- FUNCTION: setAcceptSocket(const WSAEVENT)
    --
    -- DATE: Feb 12, 2020
    --
    -- REVISIONS: N/A
    --
    -- DESIGNER: Henry Ho
    --
    -- PROGRAMMER: Henry Ho
    --
    -- INTERFACE: void setAcceptSocket(const WSAEVENT e)
    --
    -- RETURNS: void
    --
    -- NOTES:
    -- Use this function to set the event for accepting a connection
    ------------------------------------------------------------------------------------------------------------------*/
    void setAcceptSocket(const SOCKET s) {
        AcceptSocket = s;
    }
    static DWORD WINAPI WorkerThread(LPVOID lpParameter);
};

/*------------------------------------------------------------------------------------------------------------------
-- CLASS: UDPConnection - A concrete class that implements Connection. It is used to handle UDP specific
--                        functions and properties.
--
-- PROGRAM: Knekt
--
-- FUNCTIONS:
-- void initClientConnection(void)
-- int sendToServer(const char*)
-- void startRoutine(unsigned long)
-- void stopRoutine(void)
-- void stopConnection(void)
-- void setReceivedEvent(const WSAEVENT)
-- WSAEVENT getReceivedEvent(void)
-- static DWORD WINAPI WorkerThread(LPVOID)
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
-- This class represents a UDP connection. Use this class to send or to receive data across a UDP connection. This
-- class also has functions to end a UDP connection.
------------------------------------------------------------------------------------------------------------------*/
class UDPConnection : public Connection {
public:
    UDPConnection() = default;
    UDPConnection(SOCKET s, struct	sockaddr_in *ss) : Connection(s, ss) {}
    ~UDPConnection() {}
    int sendToServer(const char* data) override;
    void startRoutine(unsigned long) override;
    void initClientConnection() override;

    /*------------------------------------------------------------------------------------------------------------------
    -- FUNCTION: setReceivedEvent(const WSAEVENT)
    --
    -- DATE: Feb 12, 2020
    --
    -- REVISIONS: N/A
    --
    -- DESIGNER: Henry Ho
    --
    -- PROGRAMMER: Henry Ho
    --
    -- INTERFACE: void setReceivedEvent(const WSAEVENT e)
    --                  const WSAEVENT e - the event handle to set the Received Event
    --
    -- RETURNS: void
    --
    -- NOTES:
    -- Use this function to set the event when receiving packet
    ------------------------------------------------------------------------------------------------------------------*/
    void setReceivedEvent(const WSAEVENT e) {
        ReceivedEvent = e;
    }

    /*------------------------------------------------------------------------------------------------------------------
    -- FUNCTION: getReceivedEvent(const WSAEVENT)
    --
    -- DATE: Feb 12, 2020
    --
    -- REVISIONS: N/A
    --
    -- DESIGNER: Henry Ho
    --
    -- PROGRAMMER: Henry Ho
    --
    -- INTERFACE: void getReceivedEvent(void)
    --
    -- RETURNS: void
    --
    -- NOTES:
    -- Use this function to get the event for receiving a packet
    ------------------------------------------------------------------------------------------------------------------*/
    WSAEVENT getReceivedEvent() const {
        return ReceivedEvent;
    }
    static DWORD WINAPI WorkerThread(LPVOID lpParameter);
};

#endif // CONNECTION_H
