#include "Connection.h"

int UDPConnection::sendToServer(std::string data) {
    const char *output = data.c_str();
    return sendto(sd, output, sizeof(output), 0, (struct sockaddr*)server, server_len);
}

std::string UDPConnection::receive() {
    int n;
    if ((n = recvfrom(sd, buf, MAXLEN, 0, (struct sockaddr*)client, &client_len)) < 0) {
        ErrorHandler::showMessage("Received wrong output. Exiting...");
        exit(1);
    }
    std::string output(buf);
    return output;
}

void UDPConnection::initClientConnection() {
    if (setsockopt( sd, SOL_SOCKET, SO_SNDBUF, buf, sizeof(buf)) != 0) {
        ErrorHandler::showMessage("Error setting socket");
        exit(1);
    }
}
