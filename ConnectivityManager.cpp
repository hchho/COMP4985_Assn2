#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <ConnectivityManager.h>
#include <stdio.h>
#include <Winsock2.h>
#include <ErrorHandler.h>

ConnectivityManager * ConnectivityManager::s_instance = 0;

ConnectivityManager* ConnectivityManager::instance() {
    if (!s_instance) {
        s_instance = new ConnectivityManager;
    }
    return s_instance;
}

Connection* ConnectivityManager::initializeConnection(ConnectionType connectionType, ProtocolType protocol, int port, const char* host) {
    SOCKET sd;
    struct	sockaddr_in server;

    openConnection();

    switch(protocol) {
    case ProtocolType::TCP:
        sd = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
        break;
    case ProtocolType::UDP:
        sd = WSASocket (PF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
        break;
    }

    if (sd == -1)
    {
        ErrorHandler::showMessage("Can't create a socket. Exiting...");
        exit(1);
    }

    memset(&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    switch(connectionType) {
    case ConnectionType::CLIENT:
        return initializeClientConnection(protocol, sd, &server, host);
    case ConnectionType::SERVER:
        return initializeServerConnection(protocol, sd, &server);
    default:
        return nullptr;
    }
}

Connection* ConnectivityManager::initializeClientConnection(ProtocolType protocol, SOCKET sd, struct	sockaddr_in *server, const char* host) {
    Connection* connection;
    struct hostent	*hp;

    if ((hp = gethostbyname(host)) == NULL)
    {
        ErrorHandler::showMessage("Unknown server address. Exiting...");
        exit(1);
    }

    memcpy((char *)&server->sin_addr, hp->h_addr, hp->h_length);
    switch(protocol) {
    case ProtocolType::TCP:
        connection = new TCPConnection(sd, server);
        break;
    case ProtocolType::UDP:
        connection = new UDPConnection(sd, server);
        break;
    }
    connection->initClientConnection();
    return connection;
}

Connection* ConnectivityManager::initializeServerConnection(ProtocolType protocol, SOCKET sd, struct	sockaddr_in *server) {
    server->sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections from any client

    if(bind (sd, (struct sockaddr *)server, sizeof(*server)) == SOCKET_ERROR) {
        ErrorHandler::showMessage("Error binding socket");
        exit(1);
    }

    switch(protocol) {
    case ProtocolType::TCP:
        return new TCPConnection(sd, server);
    case ProtocolType::UDP:
        return new UDPConnection(sd, server);
    }
}

void ConnectivityManager::openConnection() {
    WSADATA stWSAData;
    WORD wVersionRequested = MAKEWORD (2,2);
    WSAStartup(wVersionRequested, &stWSAData );
}

void ConnectivityManager::closeConnection(Connection* connection) {
    connection->stopConnection();
}
