#include "Connection.h"

int TCPConnection::sendToServer(std::string data) {
    return send(sd, data.c_str(), BUFSIZE, 0);
}

void TCPConnection::startRoutine() {
    int n;
    int bytes_to_read;
    char* bp;
    listen(sd, 1);

    if ((AcceptEvent = WSACreateEvent()) == WSA_INVALID_EVENT)
    {
        ErrorHandler::showMessage("Can't accept client");
        exit(1);
    }

    if ((ThreadHandle = CreateThread(NULL, 0, WorkerThread, (LPVOID) AcceptEvent, 0, &ThreadId)) == NULL)
    {
        ErrorHandler::showMessage("Error creating thread");
        exit(1);
    }

    if ((new_sd = accept (sd, NULL, NULL)) == -1) {
        ErrorHandler::showMessage("Can't accept client");
        exit(1);
    }

    if (WSASetEvent(AcceptEvent) == FALSE)
    {
        ErrorHandler::showMessage("WSASetEvent failed");
        exit(1);
    }


    // THE SECTION BELOW DOES NOT IMPLEMENT OVERLAP STRUCTURE
    //    if ((new_sd = accept (sd, (struct sockaddr *)client, &client_len)) == -1)
    //    {
    //        ErrorHandler::showMessage("Can't accept client");
    //        exit(1);
    //    }

    //    bp = buf;
    //    bytes_to_read = BUFSIZE;

    //    while ((n = recv (new_sd, bp, bytes_to_read, 0)) < BUFSIZE)
    //    {
    //        bp += n;
    //        bytes_to_read -= n;
    //        if (n == 0)
    //            break;
    //    }
    //    closesocket(new_sd);
    return;
}

DWORD WINAPI TCPConnection::WorkerThread(LPVOID lpParameter) {
    DWORD Flags;
    LPSOCKET_INFORMATION SocketInfo;
    WSAEVENT EventArray[1];
    DWORD Index;
    DWORD RecvBytes;

    // Save the accept event in the event array.

    EventArray[0] = (WSAEVENT) lpParameter;

    while(TRUE)
    {
        // Wait for accept() to signal an event and also process WorkerRoutine() returns.

        while(TRUE)
        {
            Index = WSAWaitForMultipleEvents(1, EventArray, FALSE, WSA_INFINITE, TRUE);

            if (Index == WSA_WAIT_FAILED)
            {
                ErrorHandler::showMessage("Waiting for multiple events failed");
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

        if ((SocketInfo = (LPSOCKET_INFORMATION) GlobalAlloc(GPTR,
                                                             sizeof(SOCKET_INFORMATION))) == NULL)
        {
            ErrorHandler::showMessage("GlobalAlloc() failed");
            return FALSE;
        }

        // Fill in the details of our accepted socket.

        SocketInfo->Socket = AcceptSocket;
        ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
        SocketInfo->BytesSEND = 0;
        SocketInfo->BytesRECV = 0;
        SocketInfo->DataBuf.len = DATA_BUFSIZE;
        SocketInfo->DataBuf.buf = SocketInfo->Buffer;

        Flags = 0;
        if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags,
                    &(SocketInfo->Overlapped), WorkerRoutine) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                ErrorHandler::showMessage("I/O operation failed with error);
                                          return FALSE;
            }
        }
    }

    return 1;
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
