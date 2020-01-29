#ifndef CONNECTIVITYMANAGER_H
#define CONNECTIVITYMANAGER_H

#include <ConnectionType.h>
#include <Connection.h>

class ConnectivityManager {
private:
    ConnectionType connectionType;
    static ConnectivityManager *s_instance;
    ConnectivityManager() = default;
    void initializeClientConnection();
    void initializeServerConnection();
public:
    static ConnectivityManager *instance() {
        if (!s_instance) {
            s_instance = new ConnectivityManager;
        }
        return s_instance;
    }
    void setConnectionType(ConnectionType);
    Connection* initializeConnection();
};

#endif // CONNECTIVITYMANAGER_H
