#ifndef CONNECTION_H
#define CONNECTION_H

#include <errno.h>
#include <stdio.h>
#include <string>
#include <WinSock2.h>
#include "ErrorHandler.h"
#include "SocketInfo.h"

#define MAXLEN  60000

void CALLBACK WorkerRoutine(DWORD Error, DWORD BytesTransferred,
                            LPWSAOVERLAPPED Overlapped, DWORD InFlags);

class Connection {
protected:
    SOCKET sd;
    char *buf;
    struct sockaddr_in *client, *server;
    int client_len, server_len;
public:
    Connection() = default;
    Connection(SOCKET s, struct sockaddr_in* ss) : sd(s) {
        buf = new char[MAXLEN];
        client = (struct sockaddr_in*)malloc(sizeof(*client));
        server = (struct sockaddr_in*)malloc(sizeof(*server));

        client_len = sizeof(*client);
        server_len = sizeof(*server);

        memcpy(server, ss, sizeof(*server));
    }
    virtual ~Connection() {
        delete[] buf;
        delete client;
        delete server;
    }
    virtual void initClientConnection() = 0;
    virtual int sendToServer(std::string data) = 0;
    virtual std::string receive() {
        return "";
    };
    virtual void startRoutine() = 0;
    virtual void stop() {
        WSACleanup();
    }

};

class TCPConnection : public Connection {
private:
    static int constexpr BUFSIZE = 255;
    SOCKET new_sd;
    WSAEVENT AcceptEvent;
    HANDLE ThreadHandle;
    DWORD ThreadId;
public:
    TCPConnection() = default;
    TCPConnection(SOCKET s, struct	sockaddr_in *ss) : Connection(s, ss) {}
    int sendToServer(std::string data) override;
    std::string receive() override;
    void initClientConnection() override;
    void startRoutine() override;
    static DWORD WINAPI WorkerThread(LPVOID lpParameter);
};

class UDPConnection : public Connection {
public:
    UDPConnection() = default;
    UDPConnection(SOCKET s, struct	sockaddr_in *ss) : Connection(s, ss) {}
    int sendToServer(std::string data) override;
    void initClientConnection() override;
};

void CALLBACK WorkerRoutine(DWORD Error, DWORD BytesTransferred,
                            LPWSAOVERLAPPED Overlapped, DWORD InFlags) {
    // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
    DWORD RecvBytes;
    DWORD Flags;
    LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION) Overlapped;

    if (Error != 0)
    {
        ErrorHandler::showMessage("I/O operation failed with error");
    }

    if (BytesTransferred == 0)
    {
        ErrorHandler::showMessage("Closing socket");
    }

    if (Error != 0 || BytesTransferred == 0)
    {
        closesocket(SI->Socket);
        GlobalFree(SI);
        return;
    }

    SI->BytesRECV = 0;

    // Now that there are no more bytes to send post another WSARecv() request.

    Flags = 0;
    ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));

    SI->DataBuf.len = DATA_BUFSIZE;
    SI->DataBuf.buf = SI->Buffer;

    if (WSARecv(SI->Socket, &(SI->DataBuf), 1, &RecvBytes, &Flags,
                &(SI->Overlapped), WorkerRoutine) == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSA_IO_PENDING )
        {
            ErrorHandler::showMessage("WSARecv() failed with error %d\n");
            return;
        }
    }
}


#endif // CONNECTION_H
