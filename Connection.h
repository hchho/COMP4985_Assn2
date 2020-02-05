#ifndef CONNECTION_H
#define CONNECTION_H

#include <errno.h>
#include <stdio.h>
#include <string>
#include <WinSock2.h>
#include "ErrorHandler.h"

#define MAXLEN 60000

class Connection {
protected:
    SOCKET sd;
    char *buf;
public:
    Connection() = default;
    Connection(SOCKET s) : sd(s) {
        buf = new char[MAXLEN];
    }
    virtual ~Connection() {
        delete[] buf;
    }
    virtual Connection* initClientConnection() = 0;
    virtual int send(std::string data) = 0;
    virtual std::string receive() = 0;
    virtual void stop() {
        WSACleanup();
    }
};

class TCPConnection : public Connection {
public:
    TCPConnection() : Connection() {}
    TCPConnection(SOCKET s) : Connection(s) {}
    int send(std::string data) override {
        return 0;
    }
    std::string receive() override {
        return "";
    }
    Connection * initClientConnection() override {
        return nullptr;
    }
};

class UDPConnection : public Connection {
private:
    int client_len, server_len;
    struct sockaddr_in *client, *server;
public:
    UDPConnection(): Connection() {}
    UDPConnection(SOCKET s, struct	sockaddr_in *ss) : Connection(s), server(ss) {
        client = (struct sockaddr_in*)malloc(sizeof(*client));

        client_len = sizeof(*client);
        server_len = sizeof(*server);
    }
    int send(std::string data) override {
        return sendto(sd, data.c_str(), sizeof(data), 0, (struct sockaddr*)server, server_len);
    }
    std::string receive() override {
        int n;
        if ((n = recvfrom(sd, buf, MAXLEN, 0, (struct sockaddr*)client, &client_len)) < 0) {
            int err = WSAGetLastError();
            ErrorHandler::showMessage("Received wrong output. Exiting...");
            exit(1);
        }
        std::string output(buf);
        return output;
    }
    Connection * initClientConnection() override {
        client->sin_family = AF_INET;
        client->sin_port = htons(0);  // bind to any available port
        client->sin_addr.s_addr = htonl(INADDR_ANY);

        if(bind(sd, (struct sockaddr *)client, client_len) == SOCKET_ERROR) {
            ErrorHandler::showMessage("Error binding socket");
            exit(1);
        };

        if(getsockname (sd, (struct sockaddr *)client, &client_len) < 0) {
            int err = WSAGetLastError();
            ErrorHandler::showMessage("Error getting socket name");
            exit(1);
        };

        return this;
    }
};

#endif // CONNECTION_H
