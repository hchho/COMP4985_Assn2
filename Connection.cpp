
#include "Connection.h"
#include "Helpers.h"
#include <streambuf>

int Connection::sendFileToServer(const char *filePath) {
    std::string buffer;

    std::ifstream fileToSend(filePath);

    fileToSend.seekg(0, std::ios::end);
    buffer.reserve(fileToSend.tellg());
    fileToSend.seekg(0, std::ios::beg);

    buffer.assign((std::istreambuf_iterator<char>(fileToSend)), std::istreambuf_iterator<char>());

    writeToFile(buffer.c_str()); // Write buffer to file first
    if (!sendToServer(buffer.c_str())) {
        return FALSE;
    }
    return TRUE;
}

int TCPConnection::sendToServer(const char* data) {
    int res, err;
    SocketInfo->DataBuf.buf = const_cast<char *>(data);
    SocketInfo->DataBuf.len = strlen(data);
    SocketInfo->packetCount++;
    res = WSASend(sd, &SocketInfo->DataBuf, 1, &SocketInfo->BytesSEND, 0, &SocketInfo->Overlapped, NULL);

    if (res == SOCKET_ERROR && (WSA_IO_PENDING != WSAGetLastError())) {
        err = WSAGetLastError();
        return 0;
    }

    res = WSAWaitForMultipleEvents(1, &SocketInfo->Overlapped.hEvent, TRUE, INFINITE, TRUE);

    if (res == WAIT_FAILED) {
        err = WSAGetLastError();
        return 0;
    }

    res = WSAGetOverlappedResult(sd, &SocketInfo->Overlapped, &SocketInfo->BytesSEND, FALSE, &SocketInfo->Flags);

    if (res == FALSE) {
        err = WSAGetLastError();
        return 0;
    }

    WSAResetEvent(SocketInfo->Overlapped.hEvent);
    SocketInfo->TotalBytesSend += SocketInfo->BytesSEND;
    return 1;
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
    SI->TotalBytesRecv = 0;
    SI->EndEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("EndEvent"));

    if ((accept_socket = accept (connection->getListenSocket(), NULL, NULL)) == -1) {
        perror("Can't accept client");
        return FALSE;
    }

    connection->setAcceptSocket(accept_socket);

    if (WSASetEvent(connection->getAcceptEvent()) == FALSE)
    {
        perror("WSASetEvent failed");
        return 0;
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
                return 0;
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
        memset(SI->Buffer, 0, DATA_BUFSIZE);
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
                return 0;
            }
        }
    }
    return 0;
}

void TCPConnection::initClientConnection() {
    if (connect (sd, (struct sockaddr *)server, server_len) == -1)
    {
        ErrorHandler::showMessage("Error creating event");
        exit(1);
    }

    SecureZeroMemory((PVOID) &SocketInfo->Overlapped, sizeof(WSAOVERLAPPED));
    SocketInfo->Overlapped.hEvent = WSACreateEvent();

    if (SocketInfo->Overlapped.hEvent == WSA_INVALID_EVENT) {
        ErrorHandler::showMessage("Error setting event");
        exit(1);
    }
}

int UDPConnection::sendToServer(const char* data) {
    int res, err;
    SocketInfo->DataBuf.buf = const_cast<char *>(data);
    SocketInfo->DataBuf.len = strlen(data);
    SocketInfo->packetCount++;

    res = WSASendTo(sd, &SocketInfo->DataBuf, 1, &SocketInfo->BytesSEND, 0, (struct sockaddr*)server, server_len, &SocketInfo->Overlapped, NULL);

    if (res == SOCKET_ERROR && (WSA_IO_PENDING != WSAGetLastError())) {
        err = WSAGetLastError();
        return 0;
    }

    res = WSAWaitForMultipleEvents(1, &SocketInfo->Overlapped.hEvent, TRUE, INFINITE, TRUE);

    if (res == WAIT_FAILED) {
        err = WSAGetLastError();
        return 0;
    }

    res = WSAGetOverlappedResult(SocketInfo->Socket, &SocketInfo->Overlapped, &SocketInfo->BytesSEND, FALSE, &SocketInfo->Flags);

    if (res == FALSE) {
        err = WSAGetLastError();
        return 0;
    }
    SocketInfo->TotalBytesSend += SocketInfo->BytesSEND;
    return 1;
}


void UDPConnection::startRoutine(unsigned long packetSize) {

    // packetSize must be large enough to hold all the data coming in.
    SocketInfo->DataBuf.len = packetSize;
    if ((ServerThreadHandle = CreateThread(NULL, 0, WorkerThread, (LPVOID) this, 0, &ServerThreadId)) == NULL)
    {
        ErrorHandler::showMessage("Error creating thread");
        exit(1);
    }
    return;
}

void UDPConnection::initClientConnection() {
    SecureZeroMemory((PVOID) &SocketInfo->Overlapped, sizeof(WSAOVERLAPPED));
    SocketInfo->Overlapped.hEvent = WSACreateEvent();

    if (SocketInfo->Overlapped.hEvent == WSA_INVALID_EVENT) {
        ErrorHandler::showMessage("Error setting event");
        exit(1);
    }

    SocketInfo->Socket = sd;
    if (setsockopt( SocketInfo->Socket, SOL_SOCKET, SO_SNDBUF, buf, sizeof(buf)) != 0) {
        ErrorHandler::showMessage("Error setting socket");
        exit(1);
    }
}

DWORD WINAPI UDPConnection::WorkerThread(LPVOID lpParameter) {
    int client_len;
    struct sockaddr_in *client;
    WSAEVENT EventArray[1];
    DWORD Index;

    UDPConnection *connection = (UDPConnection*) lpParameter;
    LPSOCKET_INFORMATION SI = connection->SocketInfo;
    SI->TotalBytesRecv = 0;
    SI->EndEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("EndEvent"));

    client = (struct sockaddr_in*)malloc(sizeof(*client));
    client_len = sizeof(*client);

    connection->setReceivedEvent(WSACreateEvent());

    if (connection->getReceivedEvent() == WSA_INVALID_EVENT) {
        perror("Error creating socket event");
        return 0;
    }

    if (WSAEventSelect(connection->getListenSocket(), connection->getReceivedEvent(), FD_CLOSE | FD_READ) == SOCKET_ERROR) {
        perror("Error listening to socket event");
        return 0;
    }

    EventArray[0] = connection->getReceivedEvent();

    while(TRUE)
    {
        while(TRUE)
        {
            Index = WSAWaitForMultipleEvents(1, EventArray, FALSE, UDP_WAIT_TIME_MS, TRUE);

            if (Index == WSA_WAIT_FAILED)
            {
                perror("Waiting for multiple events failed");
                return 0;
            }

            if (Index == WSA_WAIT_TIMEOUT) {
                perror("Timed out");
                SetEvent(SI->EndEvent);
                return 0;
            }

            if (Index != WAIT_IO_COMPLETION)
            {
                break;
            }

        }

        WSAResetEvent(EventArray[Index - WSA_WAIT_EVENT_0]);

        // Fill in the details of our accepted socket.

        SI->Socket = connection->getListenSocket();
        ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));
        memset(SI->Buffer, 0, DATA_BUFSIZE);
        SI->BytesSEND = 0;
        SI->BytesRECV = 0;
        SI->DataBuf.buf = SI->Buffer;
        SI->Flags = 0;

        if (WSARecvFrom(SI->Socket, &(SI->DataBuf), 1, &SI->BytesRECV, &SI->Flags,
                        (struct sockaddr*)client, &client_len, &(SI->Overlapped), UDPWorkerRoutine) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                perror("I/O operation failed with error");
                delete client;
                return 0;
            }
        }
    }
    return 0;
}
