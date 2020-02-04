#ifndef CONNECTIVITYMANAGER_H
#define CONNECTIVITYMANAGER_H

#include <ConnectionType.h>
#include <Connection.h>
#include <ProtocolType.h>

class ConnectivityManager {
private:
    static ConnectivityManager *s_instance;
    ConnectivityManager() = default;
    Connection* initializeClientConnection(ProtocolType, SOCKET, struct	sockaddr_in *server, const char* host);
    Connection* initializeServerConnection(ProtocolType, SOCKET, struct	sockaddr_in *server);

    void openConnection();
public:
    static ConnectivityManager *instance() {
        if (!s_instance) {
            s_instance = new ConnectivityManager;
        }
        return s_instance;
    }
    Connection* initializeConnection(ConnectionType, ProtocolType, int port, const char* host);
    void closeConnection(Connection*);
};

#endif // CONNECTIVITYMANAGER_H
