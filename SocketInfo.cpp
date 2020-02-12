#include "SocketInfo.h"
#include "Helpers.h"

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
        SetEvent(SI->EndEvent);
        return;
    }

    Flags = 0;

    writeToFile(SI->DataBuf.buf); // Write buffer to file first
    memset(SI->Buffer, 0, DATA_BUFSIZE);
    SI->DataBuf.buf = SI->Buffer;

    if (WSARecv(SI->Socket, &(SI->DataBuf), 1, &SI->BytesRECV, &Flags,
                &(SI->Overlapped), TCPWorkerRoutine) == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSA_IO_PENDING )
        {
            perror("WSARecv() failed with error %d\n");
            return;
        }
    }
    if (SI->TotalBytesRecv == 0) {
        GetSystemTime(&SI->stStartTime);
    }
    SI->packetCount++;
    SI->TotalBytesRecv += BytesTransferred;
    GetSystemTime(&SI->stEndTime);
}

void CALLBACK UDPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
                               LPWSAOVERLAPPED Overlapped, DWORD InFlags) {
    // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
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
        SetEvent(SI->EndEvent);
        return;
    }

    if (WSARecvFrom(SI->Socket, &(SI->DataBuf), 1, &SI->BytesRECV, &SI->Flags,
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
    if (SI->TotalBytesRecv == 0) {
        GetSystemTime(&SI->stStartTime);
    }
    SI->packetCount++;
    SI->TotalBytesRecv += SI->BytesRECV;
    GetSystemTime(&SI->stEndTime);
}
