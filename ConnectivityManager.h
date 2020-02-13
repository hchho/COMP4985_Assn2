#ifndef CONNECTIVITYMANAGER_H
#define CONNECTIVITYMANAGER_H

#include <ConnectionType.h>
#include <Connection.h>
#include <ProtocolType.h>

/*------------------------------------------------------------------------------------------------------------------
-- HEADER FILE: ConnectivityManager.h - A singleton that creates and closes TCP and UDP connections
--
-- PROGRAM: Knekt
--
-- FUNCTIONS:
-- Connection* initializeClientConnection(ProtocolType, SOCKET, struct sockaddr_in, const char* host)
-- Connection* initializeServerConnection(ProtocolType, SOCKET, struct sockaddr_in)
-- void openConnection(void)
-- static ConnectivityManager* instance(void)
-- Connection* initializeConnection(ConnectionType, ProtocolType, int port, const char* host)
-- void closeConnection(Connection*)
--
-- DATE: Feb 12, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Henry Ho
--
-- PROGRAMMER: Henry Ho
--
-- NOTES:
-- Use this singleton class to create connections and to close connections.
------------------------------------------------------------------------------------------------------------------*/
class ConnectivityManager {
private:
    static ConnectivityManager *s_instance;
    ConnectivityManager() = default;
    Connection* initializeClientConnection(ProtocolType, SOCKET, struct	sockaddr_in *server, const char* host);
    Connection* initializeServerConnection(ProtocolType, SOCKET, struct	sockaddr_in *server);

    void openConnection();
public:
    static ConnectivityManager *instance();
    Connection* initializeConnection(ConnectionType, ProtocolType, int port, const char* host);
    void closeConnection(Connection*);
};

#endif // CONNECTIVITYMANAGER_H
