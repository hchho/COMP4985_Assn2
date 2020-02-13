#include "SocketInfo.h"
#include "Helpers.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: convert
--
-- DATE: January 21, 2008
--
-- REVISIONS: N/A
--
-- DESIGNER: Henry Ho
--
-- PROGRAMMER: Henry Ho
--
-- INTERFACE: const char* convert (int args, char** input)
--                  int args - the number of arguments
--                  char ** input - the user input
--
-- RETURNS: const char*.
--
-- NOTES:
-- Converts a user-provided host name to an IP address
------------------------------------------------------------------------------------------------------------------*/
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
    memset(SI->Buffer, 0, DATA_BUFSIZE); // This is done to ensure the buffer is clear before it reads
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
    int client_len, res, err;
    struct sockaddr_in *client;
    LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION) Overlapped;

    client = (struct sockaddr_in*)malloc(sizeof(*client));
    client_len = sizeof(*client);

    if (Error != 0)
    {
        perror("I/O operation failed with error");
        closesocket(SI->Socket);
        SetEvent(SI->EndEvent);
        return;
    }

//    if (BytesTransferred == 0)
//    {
//        perror("Closing socket");
//    }

//    if (Error != 0 || BytesTransferred == 0)
//    {
//        closesocket(SI->Socket);
//        SetEvent(SI->EndEvent);
//        return;
//    }

    res = WSAGetOverlappedResult(SI->Socket, &SI->Overlapped, &SI->BytesRECV, FALSE, &SI->Flags);

    if (res == FALSE) {
        err = WSAGetLastError();
        if (err != WSA_IO_INCOMPLETE) {
            closesocket(SI->Socket);
            WSAResetEvent(SI->Overlapped.hEvent);
            SetEvent(SI->EndEvent);
            return;
        } else {
            res = WSAWaitForMultipleEvents(1, &SI->Overlapped.hEvent, TRUE, UDP_WAIT_TIME_MS, TRUE);

            if (res == WAIT_FAILED) {
                err = WSAGetLastError();
                return;
            }
        }
    }

    if (SI->TotalBytesRecv == 0) {
        GetSystemTime(&SI->stStartTime);
    }
    SI->packetCount++;
    SI->TotalBytesRecv += SI->BytesRECV;
    GetSystemTime(&SI->stEndTime);

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
}
