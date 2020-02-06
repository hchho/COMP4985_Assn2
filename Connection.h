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
    UDPConnection(SOCKET s, struct	sockaddr_in *ss) : Connection(s) {
        client = (struct sockaddr_in*)malloc(sizeof(*client));

        server = (struct sockaddr_in*)malloc(sizeof(*server));

        memcpy(server, ss, sizeof(*server));

        client_len = sizeof(*client);
        server_len = sizeof(*server);
    }
    int send(std::string data) override {
        const char *output = data.c_str();
        return sendto(sd, output, sizeof(output), 0, (struct sockaddr*)server, server_len);
    }
    std::string receive() override {
        int n;
        if ((n = recvfrom(sd, buf, MAXLEN, 0, (struct sockaddr*)client, &client_len)) < 0) {
            ErrorHandler::showMessage("Received wrong output. Exiting...");
            exit(1);
        }
        std::string output(buf);
        return output;
    }
    Connection * initClientConnection() override {
        if (setsockopt( sd, SOL_SOCKET, SO_SNDBUF, buf, sizeof(buf)) != 0) {
            ErrorHandler::showMessage("Error setting socket");
            exit(1);
        }

        return this;
    }
};

#endif // CONNECTION_H
