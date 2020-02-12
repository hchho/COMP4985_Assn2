#ifndef SOCKETINFO_H
#define SOCKETINFO_H

#define DATA_BUFSIZE 100000

#include <WinSock2.h>

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
    unsigned int packetCount = 0;
    unsigned long TotalBytesRecv = 0;
    unsigned long TotalBytesSend = 0;
} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;

void CALLBACK TCPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
                               LPWSAOVERLAPPED Overlapped, DWORD InFlags);

void CALLBACK UDPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
                               LPWSAOVERLAPPED Overlapped, DWORD InFlags);

#endif // SOCKETINFO_H
