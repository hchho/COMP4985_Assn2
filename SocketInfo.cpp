#include "SocketInfo.h"
#include "Helpers.h"

/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: SocketInfo.cpp - This file contains the implementation of the SocketInfo header file functions.
--
-- PROGRAM: Knekt
--
-- FUNCTIONS:
-- void CALLBACK TCPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
--                                LPWSAOVERLAPPED Overlapped, DWORD InFlags)
-- void CALLBACK UDPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
--                                LPWSAOVERLAPPED Overlapped, DWORD InFlags)
--
-- DATE: Feb 17, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Henry Ho
--
-- PROGRAMMER: Henry Ho
--
-- NOTES:
-- (N/A)
------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: TCPWorkerRoutine
--
-- DATE: January 21, 2008
--
-- REVISIONS: N/A
--
-- DESIGNER: Henry Ho
--
-- PROGRAMMER: Henry Ho
--
-- INTERFACE: void CALLBACK TCPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
--                                           LPWSAOVERLAPPED Overlapped, DWORD InFlags)
--              DWORD Error - error code encountered when initiating callback sequence
--              DWORD BytesTransferred - bytes transferred when reading from the socket
--              LPWSAOVERLAPPED Overlapped - overlapped structure associated with the asynchronous socket I/O
--              DWORD InFlags - flags set for the socket
--
-- RETURNS: void
--
-- NOTES:
-- Call this function to read socket data asynchronously
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

    if (SI->saveInputToFile) {
        writeToFile(SI->DataBuf.buf); // Write buffer to file
    }
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

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: UDPWorkerRoutine
--
-- DATE: January 21, 2008
--
-- REVISIONS: N/A
--
-- DESIGNER: Henry Ho
--
-- PROGRAMMER: Henry Ho
--
-- INTERFACE: void CALLBACK UDPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
--                                           LPWSAOVERLAPPED Overlapped, DWORD InFlags)
--              DWORD Error - error code encountered when initiating callback sequence
--              DWORD BytesTransferred - bytes transferred when reading from the socket
--              LPWSAOVERLAPPED Overlapped - overlapped structure associated with the asynchronous socket I/O
--              DWORD InFlags - flags set for the socket
--
-- RETURNS: void
--
-- NOTES:
-- Call this function to read socket data asynchronously
------------------------------------------------------------------------------------------------------------------*/
void CALLBACK UDPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
                               LPWSAOVERLAPPED Overlapped, DWORD InFlags) {
    int client_len, res, err;
    struct sockaddr_in *client;
    LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION) Overlapped;

    client = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    client_len = sizeof(struct sockaddr_in);

    if (Error != 0)
    {
        perror("I/O operation failed with error");
        closesocket(SI->Socket);
        CloseHandle(SI->Overlapped.hEvent);
        SetEvent(SI->EndEvent);
        return;
    }

    // This must be called in the scenario the program has not read the data in time
    res = WSAGetOverlappedResult(SI->Socket, &SI->Overlapped, &SI->BytesRECV, FALSE, &SI->Flags);
    WSAResetEvent(SI->Overlapped.hEvent);

    if (res == FALSE) {
        err = WSAGetLastError();
        if (err != WSA_IO_INCOMPLETE) {
            closesocket(SI->Socket);
            CloseHandle(SI->Overlapped.hEvent);
            SetEvent(SI->EndEvent);
            return;
        } else {
            // Wait for the read operation to complete
            res = WSAWaitForMultipleEvents(1, &SI->Overlapped.hEvent, TRUE, UDP_WAIT_TIME_MS, TRUE);
            WSAResetEvent(SI->Overlapped.hEvent);
            if (res == WAIT_FAILED) {
                err = WSAGetLastError();
                closesocket(SI->Socket);
                CloseHandle(SI->Overlapped.hEvent);
                SetEvent(SI->EndEvent);
                return;
            }
        }
    }
    if (SI->saveInputToFile) {
        writeToFile(SI->DataBuf.buf); // Write buffer to file
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
