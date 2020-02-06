#ifndef CONNECTION_H
#define CONNECTION_H

#include <errno.h>
#include <stdio.h>
#include <string>
#include <WinSock2.h>
#include "ErrorHandler.h"

#define MAXLEN  60000
#define BUFSIZE 255

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
    SOCKET new_sd;
public:
    TCPConnection() : Connection() {}
    TCPConnection(SOCKET s, struct	sockaddr_in *ss) : Connection(s, ss) {}
    int sendToServer(std::string data) override {
        return send(sd, data.c_str(), BUFSIZE, 0);
    }
    std::string receive() override {
        int n;
        int bytes_to_read;
        char* bp;
        listen(sd, 1);
        if ((new_sd = accept (sd, (struct sockaddr *)client, &client_len)) == -1)
        {
            ErrorHandler::showMessage("Can't accept client");
            exit(1);
        }

        bp = buf;
        bytes_to_read = BUFSIZE;

        while ((n = recv (new_sd, bp, bytes_to_read, 0)) < BUFSIZE)
        {
            bp += n;
            bytes_to_read -= n;
            if (n == 0)
                break;
        }
        closesocket(new_sd);
        return bp;
    }
    void initClientConnection() override {
        if (connect (sd, (struct sockaddr *)server, server_len) == -1)
        {
            int error = WSAGetLastError();
            ErrorHandler::showMessage("Can't connect to sever");
            exit(1);
        }
    }
};

class UDPConnection : public Connection {
public:
    UDPConnection(): Connection() {}
    UDPConnection(SOCKET s, struct	sockaddr_in *ss) : Connection(s, ss) {}
    int sendToServer(std::string data) override;
    std::string receive() override;
    void initClientConnection() override;
};

#endif // CONNECTION_H
