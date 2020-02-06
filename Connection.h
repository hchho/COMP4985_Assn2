#ifndef CONNECTION_H
#define CONNECTION_H

#include <errno.h>
#include <stdio.h>
#include <string>
#include <WinSock2.h>
#include "ErrorHandler.h"

#define MAXLEN  60000

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
    virtual std::string receive() = 0;
    virtual void stop() {
        WSACleanup();
    }
};

class TCPConnection : public Connection {
private:
    static int constexpr BUFSIZE = 255;
    SOCKET new_sd;
public:
    TCPConnection() = default;
    TCPConnection(SOCKET s, struct	sockaddr_in *ss) : Connection(s, ss) {}
    int sendToServer(std::string data) override;
    std::string receive() override;
    void initClientConnection() override;
};

class UDPConnection : public Connection {
public:
    UDPConnection() = default;
    UDPConnection(SOCKET s, struct	sockaddr_in *ss) : Connection(s, ss) {}
    int sendToServer(std::string data) override;
    std::string receive() override;
    void initClientConnection() override;
};

#endif // CONNECTION_H
