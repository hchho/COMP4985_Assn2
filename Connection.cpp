#include "Connection.h"

int TCPConnection::sendToServer(std::string data) {
    return send(sd, data.c_str(), BUFSIZE, 0);
}

std::string TCPConnection::receive() {
    int n;
    int bytes_to_read;
    char* bp;
    listen(sd, 1);
    if ((new_sd = accept (sd, (struct sockaddr *)client, &client_len)) == -1)
    {
        ErrorHandler::showMessage("Can't accept client");
        exit(1);
    }

    bp = buf;
    bytes_to_read = BUFSIZE;

    while ((n = recv (new_sd, bp, bytes_to_read, 0)) < BUFSIZE)
    {
        bp += n;
        bytes_to_read -= n;
        if (n == 0)
            break;
    }
    closesocket(new_sd);
    return bp;
}

void TCPConnection::initClientConnection() {
    if (connect (sd, (struct sockaddr *)server, server_len) == -1)
    {
        ErrorHandler::showMessage("Can't connect to server");
        exit(1);
    }
}

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
