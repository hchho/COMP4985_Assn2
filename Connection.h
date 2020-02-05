#ifndef CONNECTION_H
#define CONNECTION_H

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <errno.h>
#include <stdio.h>
#include <string>
#include <WinSock2.h>
#include "ErrorHandler.h"

#define MAXLEN 60000

using namespace std;

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
    virtual int send(string data) = 0;
    virtual string receive() = 0;
    virtual void stop() {
        WSACleanup();
    }
};

class TCPConnection : public Connection {
public:
    TCPConnection() : Connection() {}
    TCPConnection(SOCKET s) : Connection(s) {}
    int send(string data) override {
        return 0;
    }
    string receive() override {
        return "";
    }
    Connection * initClientConnection() override {
        return nullptr;
    }
};

class UDPConnection : public Connection {
private:
    int client_len, server_len;
    struct	sockaddr *server;
    struct sockaddr_in *client;
public:
    UDPConnection(): Connection() {}
    UDPConnection(SOCKET s, struct	sockaddr *ss) : Connection(s), server(ss) {
        client = (struct sockaddr_in*)malloc(sizeof(*client));
        server_len = sizeof(*server);
    }
    int send(string data) override {
        return sendto(sd, data.c_str(), sizeof(data), 0, server, server_len);
    }
    string receive() override {
        int n;
        if ((n = recvfrom(sd, buf, MAXLEN, 0, (struct sockaddr*)client, &client_len)) < 0) {
            ErrorHandler::showMessage("Received wrong output. Exiting...");
            exit(1);
        }
        string output(buf);
        return output;
    }
    Connection * initClientConnection() override {
        client_len = sizeof(*client);

        client->sin_family = AF_INET;
        client->sin_port = htons(0);  // bind to any available port
        client->sin_addr.s_addr = htonl(INADDR_ANY);

        bind(sd, (struct sockaddr *)client, client_len);

        getsockname (sd, (struct sockaddr *)client, &client_len);

        return this;
    }
};

#endif // CONNECTION_H
