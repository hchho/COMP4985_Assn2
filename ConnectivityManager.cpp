#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <ConnectivityManager.h>
#include <stdio.h>
#include <Winsock2.h>
#include <ErrorHandler.h>

/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: ConnectivityManager.cpp - This file contains the implementation of the ConnectivityManager class.
--
-- PROGRAM: Knekt
--
-- FUNCTIONS:
-- ConnectivityManager* instance(void)
-- Connection* initializeConnection(ConnectionType connectionType, ProtocolType protocol, int port, const char* host)
-- Connection* initializeClientConnection(ProtocolType protocol, SOCKET sd, struct	sockaddr_in *server, const char* host)
-- Connection* initializeServerConnection(ProtocolType protocol, SOCKET sd, struct	sockaddr_in *server)
-- void openConnection(void)
-- void closeConnection(Connection* connection)
--
-- DATE: Feb 17, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Henry Ho
--
-- PROGRAMMER: Henry Ho
--
-- NOTES:
-- This is a singleton class used to create connections and close existing connections.
------------------------------------------------------------------------------------------------------------------*/

ConnectivityManager * ConnectivityManager::s_instance = 0;


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: instance
--
-- DATE: Feb 17, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Henry Ho
--
-- PROGRAMMER: Henry Ho
--
-- INTERFACE: ConnectivityManager* instance(void)
--
-- RETURNS: ConnectivityManager*
--
-- NOTES:
-- Call this function to retrieve the singleton ConnectivityManager object.
------------------------------------------------------------------------------------------------------------------*/
ConnectivityManager* ConnectivityManager::instance() {
    if (!s_instance) {
        s_instance = new ConnectivityManager;
    }
    return s_instance;
}


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: initializeConnection
--
-- DATE: Feb 17, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Henry Ho
--
-- PROGRAMMER: Henry Ho
--
-- INTERFACE: Connection* initializeConnection(ConnectionType connectionType, ProtocolType protocol,
--                                             int port, const char* host)
--              ConnectionType connectionType - an enum of either a client or server connection
--              ProtocolType protocolType - an enum of either a TCP or a UDP connection
--              int port - the port to connect or to open
--              const char* host - the host's address to connect to
--
-- RETURNS: Connection*
--
-- NOTES:
-- Call this function to create any type of connection.
------------------------------------------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: initializeClientConnection
--
-- DATE: Feb 17, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Henry Ho
--
-- PROGRAMMER: Henry Ho
--
-- INTERFACE: Connection* initializeClientConnection(ProtocolType protocol, SOCKET sd,
--                                                   struct	sockaddr_in *server, const char* host)
--              ProtocolType protocolType - an enum of either a TCP or a UDP connection
--              SOCKET sd - the socket descriptor of the opened socket
--              struct	sockaddr_in *server - pointer to a socket address struct
--              const char* host - the host's address to connect to
--
-- RETURNS: Connection*
--
-- NOTES:
-- Call this function to create any client connection.
------------------------------------------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: initializeServerConnection
--
-- DATE: Feb 17, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Henry Ho
--
-- PROGRAMMER: Henry Ho
--
-- INTERFACE: Connection* initializeServerConnection(ProtocolType protocol, SOCKET sd, struct	sockaddr_in *server)
--              ProtocolType protocolType - an enum of either a TCP or a UDP connection
--              SOCKET sd - the socket descriptor of the opened socket
--              struct	sockaddr_in *server - pointer to a socket address struct
--
-- RETURNS: Connection*
--
-- NOTES:
-- Call this function to create any host connection.
------------------------------------------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: openConnection
--
-- DATE: Feb 17, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Henry Ho
--
-- PROGRAMMER: Henry Ho
--
-- INTERFACE: void openConnection(void)
--
-- RETURNS: void
--
-- NOTES:
-- Call this function to open sockets in Windows.
------------------------------------------------------------------------------------------------------------------*/
void ConnectivityManager::openConnection() {
    WSADATA stWSAData;
    WORD wVersionRequested = MAKEWORD (2,2);
    WSAStartup(wVersionRequested, &stWSAData );
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: closeConnection
--
-- DATE: Feb 17, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Henry Ho
--
-- PROGRAMMER: Henry Ho
--
-- INTERFACE: void closeConnection(Connection* connection)
--              Connection* connection - the connection to terminate
--
-- RETURNS: void
--
-- NOTES:
-- Call this function to terminate a Connection class.
------------------------------------------------------------------------------------------------------------------*/
void ConnectivityManager::closeConnection(Connection* connection) {
    connection->stopConnection();
}
