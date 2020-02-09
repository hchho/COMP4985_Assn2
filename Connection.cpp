#include "Connection.h"

int TCPConnection::sendToServer(const char* data) {
    return send(sd, data, strlen(data), 0);
}

void TCPConnection::startRoutine(unsigned long packetSize) {
    listen(sd, 1);

    if ((AcceptEvent = WSACreateEvent()) == WSA_INVALID_EVENT)
    {
        ErrorHandler::showMessage("Can't accept client");
        exit(1);
    }

    SocketInfo->DataBuf.len = packetSize;
    if ((ServerThreadHandle = CreateThread(NULL, 0, WorkerThread, (LPVOID) this, 0, &ServerThreadId)) == NULL)
    {
        ErrorHandler::showMessage("Error creating thread");
        exit(1);
    }
    return;
}

DWORD WINAPI TCPConnection::WorkerThread(LPVOID lpParameter) {
    DWORD Flags;
    WSAEVENT EventArray[1];
    DWORD Index;
    SOCKET accept_socket;

    TCPConnection *connection = (TCPConnection*) lpParameter;
    LPSOCKET_INFORMATION SI = connection->SocketInfo;

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

    // Save the accept event in the event array.
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

        // Fill in the details of our accepted socket.

        SI->Socket = connection->getAcceptSocket();
        ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));
        SI->BytesSEND = 0;
        SI->BytesRECV = 0;
        SI->DataBuf.buf = SI->Buffer;

        Flags = 0;
        if (WSARecv(SI->Socket, &(SI->DataBuf), 1, &SI->BytesRECV, &Flags,
                    &(SI->Overlapped), TCPWorkerRoutine) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                perror("I/O operation failed with error");
                return FALSE;
            }
        }
    }
    return TRUE;
}

void TCPConnection::initClientConnection() {
    if (connect (sd, (struct sockaddr *)server, server_len) == -1)
    {
        ErrorHandler::showMessage("Error creating event");
        exit(1);
    }
}

int UDPConnection::sendToServer(const char* data) {
    return sendto(sd, data, std::strlen(data), 0, (struct sockaddr*)server, server_len);
}

void UDPConnection::startRoutine(unsigned long packetSize) {
    SocketInfo->DataBuf.len = packetSize;
    if ((ServerThreadHandle = CreateThread(NULL, 0, WorkerThread, (LPVOID) this, 0, &ServerThreadId)) == NULL)
    {
        ErrorHandler::showMessage("Error creating thread");
        exit(1);
    }
    return;
}

void UDPConnection::initClientConnection() {
    if (setsockopt( sd, SOL_SOCKET, SO_SNDBUF, buf, sizeof(buf)) != 0) {
        ErrorHandler::showMessage("Error setting socket");
        exit(1);
    }
}

DWORD WINAPI UDPConnection::WorkerThread(LPVOID lpParameter) {
    int client_len;
    struct sockaddr_in *client;
    DWORD Flags;
    WSAEVENT EventArray[1];
    DWORD Index;

    UDPConnection *connection = (UDPConnection*) lpParameter;
    LPSOCKET_INFORMATION SI = connection->SocketInfo;

    client = (struct sockaddr_in*)malloc(sizeof(*client));
    client_len = sizeof(*client);

    connection->setReceivedEvent(WSACreateEvent());

    if (connection->getAcceptEvent() == WSA_INVALID_EVENT) {
        perror("Error creating socket event");
        return FALSE;
    }

    if (WSAEventSelect(connection->getListenSocket(), connection->getReceivedEvent(), FD_CLOSE | FD_READ | FD_OOB) == SOCKET_ERROR) {
        perror("Error listening to socket event");
        return FALSE;
    }

    EventArray[0] = connection->getReceivedEvent();

    while(TRUE)
    {
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

        // Fill in the details of our accepted socket.

        SI->Socket = connection->getListenSocket();
        ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));
        SI->BytesSEND = 0;
        SI->BytesRECV = 0;
        SI->DataBuf.buf = SI->Buffer;

        Flags = 0;
        if (WSARecvFrom(SI->Socket, &(SI->DataBuf), 1, &SI->BytesRECV, &Flags,
                        (struct sockaddr*)client, &client_len, &(SI->Overlapped), UDPWorkerRoutine) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                perror("I/O operation failed with error");
                delete client;
                return FALSE;
            }
        }
        if (SI->BytesRECV > 0) {
            SI->packetCount++;
            SI->TotalBytesRecv += SI->BytesRECV;
        }
    }
    return TRUE;
}
