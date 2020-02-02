#include <ConnectivityManager.h>
#include <stdio.h>
#include <Winsock2.h>

Connection* ConnectivityManager::initializeConnection(ConnectionType connectionType, ProtocolType protocolType, int port) {
    switch(connectionType) {
    case ConnectionType::CLIENT:
        break;
    case ConnectionType::SERVER:
        return initializeServerConnection(protocolType, port);
    }
}

void ConnectivityManager::initializeClientConnection(ProtocolType protocol, int port) {

}

Connection* ConnectivityManager::initializeServerConnection(ProtocolType protocol, int port) {
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
        perror ("Can't create a socket");
        exit(1);
    }

    memset((char *)&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections from any client

    if (bind (sd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror ("Can't bind name to socket");
        exit(1);
    }

    switch(protocol) {
    case ProtocolType::TCP:
        return new TCPConnection(&sd);
    case ProtocolType::UDP:
        return new UDPConnection(&sd);
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
