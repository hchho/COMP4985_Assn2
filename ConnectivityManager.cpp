#include <ConnectivityManager.h>

void ConnectivityManager::setConnectionType(ConnectionType type) {
    connectionType = type;
}

Connection* ConnectivityManager::initializeConnection() {
    switch(connectionType) {
    case ConnectionType::CLIENT:
        break;
    case ConnectionType::SERVER:
        break;
    }
}
