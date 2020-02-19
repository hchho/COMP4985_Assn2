#ifndef SOCKETINFO_H
#define SOCKETINFO_H

#define DATA_BUFSIZE        100000
#define UDP_WAIT_TIME_MS    10000

#include <WinSock2.h>

/*------------------------------------------------------------------------------------------------------------------
-- HEADER FILE: SocketInfo.h - This file contains the global struct for socket information and the callback
--                             routines that use the struct.
--
-- PROGRAM: Knekt
--
-- FUNCTIONS:
-- void CALLBACK TCPWorkerRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags)
-- void CALLBACK UDPWorkerRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags)
-- void terminateCallbackRoutine(LPSOCKET_INFORMATION)
--
-- DATE: Feb 12, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Henry Ho
--
-- PROGRAMMER: Henry Ho
--
-- NOTES:
-- This file should only contain methods or object closely coupled with the socket information struct.
------------------------------------------------------------------------------------------------------------------*/

typedef struct _SOCKET_INFORMATION {
    OVERLAPPED Overlapped;
    SOCKET Socket;
    CHAR Buffer[DATA_BUFSIZE];
    WSABUF DataBuf;
    DWORD BytesSEND;
    DWORD BytesRECV;
    DWORD Flags;
    SYSTEMTIME stStartTime, stEndTime;
    HANDLE EndEvent;
    BOOL saveInputToFile;
    unsigned int packetCount = 0;
    unsigned long TotalBytesRecv = 0;
    unsigned long TotalBytesSend = 0;
} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;

void CALLBACK TCPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
                               LPWSAOVERLAPPED Overlapped, DWORD InFlags);

void CALLBACK UDPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
                               LPWSAOVERLAPPED Overlapped, DWORD InFlags);

void terminateCallbackRoutine(LPSOCKET_INFORMATION);

#endif // SOCKETINFO_H
