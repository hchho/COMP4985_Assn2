#include "Connection.h"

int TCPConnection::sendToServer(std::string data) {
    return send(sd, data.c_str(), BUFSIZE, 0);
}

void TCPConnection::startRoutine() {
    listen(sd, 1);

    if ((AcceptEvent = WSACreateEvent()) == WSA_INVALID_EVENT)
    {
        ErrorHandler::showMessage("Can't accept client");
        exit(1);
    }

    if ((ServerThreadHandle = CreateThread(NULL, 0, WorkerThread, (LPVOID) this, 0, &ServerThreadId)) == NULL)
    {
        ErrorHandler::showMessage("Error creating thread");
        exit(1);
    }
    return;
}

void TCPConnection::stopRoutine() {
    CloseHandle(ServerThreadHandle);
}

DWORD WINAPI TCPConnection::WorkerThread(LPVOID lpParameter) {
    DWORD Flags;
//    LPSOCKET_INFORMATION SocketInfo;
    WSAEVENT EventArray[1];
    DWORD Index;
    DWORD RecvBytes;
    SOCKET accept_socket;
    // Save the accept event in the event array.
    TCPConnection *connection = (TCPConnection*) lpParameter;

    if ((accept_socket = accept (connection->getListenSocket(), NULL, NULL)) == -1) {
        perror("Can't accept client");
        return FALSE;
    }

    connection->setAcceptSocket(accept_socket);

    if (WSASetEvent(connection->getAcceptEvent()) == FALSE)
    {
        perror("WSASetEvent failed");
        return FALSE;
    }

    EventArray[0] = connection->getAcceptEvent();

    while(TRUE)
    {
        // Wait for accept() to signal an event and also process WorkerRoutine() returns.

        while(TRUE)
        {
            Index = WSAWaitForMultipleEvents(1, EventArray, FALSE, WSA_INFINITE, TRUE);

            if (Index == WSA_WAIT_FAILED)
            {
                perror("Waiting for multiple events failed");
                return FALSE;
            }

            if (Index != WAIT_IO_COMPLETION)
            {
                // An accept() call event is ready - break the wait loop
                break;
            }
        }

        WSAResetEvent(EventArray[Index - WSA_WAIT_EVENT_0]);

        // Create a socket information structure to associate with the accepted socket.

//        if ((SocketInfo = (LPSOCKET_INFORMATION) GlobalAlloc(GPTR,
//                                                             sizeof(SOCKET_INFORMATION))) == NULL)
//        {
//            ErrorHandler::showMessage("GlobalAlloc() failed");
//            return FALSE;
//        }

        // Fill in the details of our accepted socket.

        connection->getSocketInfo()->Socket = connection->getAcceptSocket();
        ZeroMemory(&(connection->getSocketInfo()->Overlapped), sizeof(WSAOVERLAPPED));
        connection->getSocketInfo()->BytesSEND = 0;
        connection->getSocketInfo()->BytesRECV = 0;
        connection->getSocketInfo()->DataBuf.len = DATA_BUFSIZE;
        connection->getSocketInfo()->DataBuf.buf = connection->getSocketInfo()->Buffer;

        Flags = 0;
        if (WSARecv(connection->getSocketInfo()->Socket, &(connection->getSocketInfo()->DataBuf), 1, &RecvBytes, &Flags,
                    &(connection->getSocketInfo()->Overlapped), WorkerRoutine) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                perror("I/O operation failed with error");
                                          return FALSE;
            }
        }
    }

    return TRUE;
};

void TCPConnection::initClientConnection() {
    if (connect (sd, (struct sockaddr *)server, server_len) == -1)
    {
        ErrorHandler::showMessage("Error creating event");
        exit(1);
    }
}

int UDPConnection::sendToServer(std::string data) {
    const char *output = data.c_str();
    return sendto(sd, output, sizeof(output), 0, (struct sockaddr*)server, server_len);
}

void UDPConnection::startRoutine() {
    int n;
    if ((n = recvfrom(sd, buf, MAXLEN, 0, (struct sockaddr*)client, &client_len)) < 0) {
        ErrorHandler::showMessage("Received wrong output. Exiting...");
        exit(1);
    }
    std::string output(buf);
    return;
}

void UDPConnection::initClientConnection() {
    if (setsockopt( sd, SOL_SOCKET, SO_SNDBUF, buf, sizeof(buf)) != 0) {
        ErrorHandler::showMessage("Error setting socket");
        exit(1);
    }
}
