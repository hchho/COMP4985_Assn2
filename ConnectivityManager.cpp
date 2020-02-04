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
        sd = socket(AF_INET, SOCK_STREAM, 0);
        break;
    case ProtocolType::UDP:
        sd = socket (PF_INET, SOCK_DGRAM, 0);
        break;
    }

    // Create a datagram socket
    if (sd == -1)
    {
        ErrorHandler::showMessage("Can't create a socket. Exiting...");
        exit(1);
    }

    memset((char *)&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections from any client

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
        connection = new TCPConnection(sd);
    case ProtocolType::UDP:
        connection = new UDPConnection(sd, (struct sockaddr*)&server);
    }
    connection->initClientConnection();
    return connection;
}

Connection* ConnectivityManager::initializeServerConnection(ProtocolType protocol, SOCKET sd, struct	sockaddr_in *server) {

    bind (sd, (struct sockaddr *)&server, sizeof(server));
//    if (bind (sd, (struct sockaddr *)&server, sizeof(server)) == -1)
//    {
//        ErrorHandler::showMessage("Can't bind name to socket. Exiting...");
//        exit(1);
//    }

    switch(protocol) {
    case ProtocolType::TCP:
        return new TCPConnection(sd);
    case ProtocolType::UDP:
        return new UDPConnection(sd, (struct sockaddr*)&server);
    }
}

void ConnectivityManager::openConnection() {
    WSADATA stWSAData;
    WORD wVersionRequested = MAKEWORD (2,2);
    WSAStartup(wVersionRequested, &stWSAData );
}

void ConnectivityManager::closeConnection(Connection* connection) {
    connection->stop();
}
