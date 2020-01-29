#ifndef CONNECTIVITYMANAGER_H
#define CONNECTIVITYMANAGER_H

#include <ConnectionType.h>

class ConnectivityManager {
private:
    ConnectionType connectionType;
    static ConnectivityManager *s_instance;
    ConnectivityManager() = default;
public:
    static ConnectivityManager *instance() {
        if (!s_instance) {
            s_instance = new ConnectivityManager;
        }
        return s_instance;
    }
    void setConnectionType(ConnectionType);
    void initializeConnection();
};

#endif // CONNECTIVITYMANAGER_H
