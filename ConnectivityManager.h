#ifndef CONNECTIVITYMANAGER_H
#define CONNECTIVITYMANAGER_H

#include <ConnectionType.h>
#include <Connection.h>
#include <ProtocolType.h>

class ConnectivityManager {
private:
    static ConnectivityManager *s_instance;
    ConnectivityManager() = default;
    void initializeClientConnection(ProtocolType, int port);
    Connection* initializeServerConnection(ProtocolType, int port);

    void openConnection();
public:
    static ConnectivityManager *instance() {
        if (!s_instance) {
            s_instance = new ConnectivityManager;
        }
        return s_instance;
    }
    Connection* initializeConnection(ConnectionType, ProtocolType, int port);
    void closeConnection();
};

#endif // CONNECTIVITYMANAGER_H
