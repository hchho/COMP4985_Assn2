#include "SocketInfo.h"
#include "ErrorHandler.h"

void CALLBACK TCPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
                               LPWSAOVERLAPPED Overlapped, DWORD InFlags) {
    // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
    DWORD Flags;
    LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION) Overlapped;

    if (Error != 0)
    {
        perror("I/O operation failed with error");
    }

    if (BytesTransferred == 0)
    {
        perror("Closing socket");
    }

    if (Error != 0 || BytesTransferred == 0)
    {
        closesocket(SI->Socket);
        GlobalFree(SI);
        return;
    }

    Flags = 0;

    if (WSARecv(SI->Socket, &(SI->DataBuf), 1, &SI->BytesRECV, &Flags,
                &(SI->Overlapped), TCPWorkerRoutine) == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSA_IO_PENDING )
        {
            ErrorHandler::showMessage("WSARecv() failed with error %d\n");
            return;
        }
    }
    if (BytesTransferred > 0) {
        SI->packetCount++;
        SI->TotalBytesRecv += BytesTransferred;
    }
}

void CALLBACK UDPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
                               LPWSAOVERLAPPED Overlapped, DWORD InFlags) {
    // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
    DWORD Flags;
    int client_len;
    struct sockaddr_in *client;
    LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION) Overlapped;

    client = (struct sockaddr_in*)malloc(sizeof(*client));
    client_len = sizeof(*client);

    if (Error != 0)
    {
        perror("I/O operation failed with error");
    }

    if (BytesTransferred == 0)
    {
        perror("Closing socket");
    }

    if (Error != 0 || BytesTransferred == 0)
    {
        closesocket(SI->Socket);
        if (SI != nullptr) {
            GlobalFree(SI);
        }
        return;
    }

    Flags = 0;

    if (WSARecvFrom(SI->Socket, &(SI->DataBuf), 1, &SI->BytesRECV, &Flags,
                    (struct sockaddr*)client, &client_len,
                    &(SI->Overlapped), UDPWorkerRoutine) == SOCKET_ERROR)
    {
        int error = WSAGetLastError();
        if (error != WSA_IO_PENDING )
        {
            perror("WSARecv() failed with error %d\n");
            return;
        }
    }
    if (SI->BytesRECV > 0) {
        SI->packetCount++;
        SI->TotalBytesRecv += SI->BytesRECV;
    }
}
