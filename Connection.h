#ifndef CONNECTION_H
#define CONNECTION_H

#include <WinSock2.h>
#include <stdio.h>
#include <string>

#define MAXLEN 60000

using namespace std;

class Connection {
protected:
    SOCKET sd;
    struct sockaddr_in client;
    int client_len;
    char *buf;
public:
    Connection() = default;
    Connection(SOCKET s) : sd(s) {
        client_len = sizeof(client);
        buf = new char[MAXLEN];
    }
    virtual ~Connection() {
        delete[] buf;
    }
    virtual Connection* initClientConnection() = 0;
    virtual bool send(string data) = 0;
    virtual string receive() = 0;
    virtual void stop() {
        WSACleanup();
    }
};

class TCPConnection : public Connection {
public:
    TCPConnection() : Connection() {}
    TCPConnection(SOCKET s) : Connection(s) {}
    bool send(string data) override {
        return 0;
    }
    string receive() override {
        return "";
    }
};

class UDPConnection : public Connection {
public:
    UDPConnection(): Connection() {}
    UDPConnection(SOCKET s) : Connection(s) {}
    bool send(string data) override {
        return sizeof(data) == sendto(sd, data.c_str(), sizeof(data), 0, (struct sockaddr *) &client, client_len);
    }
    string receive() override {
        int n;
        if ((n = recvfrom(sd, buf, MAXLEN, 0, (struct sockaddr*)&client, &client_len)) < 0) {
            printf("Received wrong output");
            exit(1);
        }
        string output(buf);
        return output;
    }
    Connection * initClientConnection() override {

    }
};

#endif // CONNECTION_H
