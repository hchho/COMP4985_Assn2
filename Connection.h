#ifndef CONNECTION_H
#define CONNECTION_H

#include <errno.h>
#include <stdio.h>
#include <string>
#include <WinSock2.h>
#include "ErrorHandler.h"
#include "SocketInfo.h"

#define MAXLEN  60000

class Connection {
protected:
    HANDLE ServerThreadHandle;
    DWORD ServerThreadId;
    WSAEVENT AcceptEvent;
    LPSOCKET_INFORMATION SocketInfo;
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

        if ((SocketInfo = (LPSOCKET_INFORMATION) GlobalAlloc(GPTR,
                                                             sizeof(SOCKET_INFORMATION))) == NULL)
        {
            ErrorHandler::showMessage("GlobalAlloc() failed");
        }
    }
    virtual ~Connection() {
        delete[] buf;
        delete client;
        delete server;
    }
    virtual void initClientConnection() = 0;
    virtual int sendToServer(const char* data) = 0;
    virtual void startRoutine() = 0;
    virtual void stopRoutine() {}
    virtual void stop() {
        WSACleanup();
    }
    SOCKET getListenSocket() const {
        return sd;
    }
    WSAEVENT getAcceptEvent() const {
        return AcceptEvent;
    }
    void setAcceptEvent(const WSAEVENT e) {
        AcceptEvent = e;
    }
    LPSOCKET_INFORMATION getSocketInfo() {
        return SocketInfo;
    }
};

class TCPConnection : public Connection {
private:
    static int constexpr BUFSIZE = MAXLEN;
    SOCKET AcceptSocket;
public:
    TCPConnection() = default;
    TCPConnection(SOCKET s, struct	sockaddr_in *ss) : Connection(s, ss) {}
    int sendToServer(const char* data) override;
    void initClientConnection() override;
    void startRoutine() override;
    void stopRoutine() override;
    SOCKET getAcceptSocket() const {
        return AcceptSocket;
    }
    void setAcceptSocket(const SOCKET s) {
        AcceptSocket = s;
    }
    static DWORD WINAPI WorkerThread(LPVOID lpParameter);
};

class UDPConnection : public Connection {
public:
    UDPConnection() = default;
    UDPConnection(SOCKET s, struct	sockaddr_in *ss) : Connection(s, ss) {}
    int sendToServer(const char* data) override;
    void startRoutine() override;
    void initClientConnection() override;
    static DWORD WINAPI WorkerThread(LPVOID lpParameter);
};

#endif // CONNECTION_H
