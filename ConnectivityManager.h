#ifndef CONNECTIVITYMANAGER_H
#define CONNECTIVITYMANAGER_H

#include <ConnectionType.h>
#include <Connection.h>
#include <ProtocolType.h>

class ConnectivityManager {
private:
    static ConnectivityManager *s_instance;
    ConnectivityManager() = default;
    void initializeClientConnection(ProtocolType, SOCKET, int port, const char* host);
    Connection* initializeServerConnection(ProtocolType, SOCKET, int port);

    void openConnection();
public:
    static ConnectivityManager *instance() {
        if (!s_instance) {
            s_instance = new ConnectivityManager;
        }
        return s_instance;
    }
    Connection* initializeConnection(ConnectionType, ProtocolType, int port);
    void closeConnection(Connection*);
};

#endif // CONNECTIVITYMANAGER_H
