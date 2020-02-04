#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdio.h>
#include <string>
#include <WinSock2.h>

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

        client_len = sizeof(*client);
        server_len = sizeof(*server);
    }
    bool send(string data) override {
        return sizeof(data) == sendto(sd, data.c_str(), sizeof(data), 0, server, server_len);
    }
    string receive() override {
        int n;
        if ((n = recvfrom(sd, buf, MAXLEN, 0, (struct sockaddr*)client, &client_len)) < 0) {
            printf("Received wrong output");
            exit(1);
        }
        string output(buf);
        return output;
    }
    Connection * initClientConnection() override {
        memset((char *)client, 0, sizeof(*client));
        client->sin_family = AF_INET;
        client->sin_port = htons(0);  // bind to any available port
        client->sin_addr.s_addr = htonl(INADDR_ANY);

        bind(sd, (struct sockaddr *)client, client_len);

        if (getsockname (sd, (struct sockaddr *)client, &client_len) < 0)
        {
            perror ("getsockname: \n");
            exit(1);
        }
        return nullptr;
    }
};

#endif // CONNECTION_H
