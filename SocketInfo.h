#ifndef SOCKETINFO_H
#define SOCKETINFO_H

#define DATA_BUFSIZE 8192

#include <WinSock2.h>

typedef struct _SOCKET_INFORMATION {
   OVERLAPPED Overlapped;
   SOCKET Socket;
   CHAR Buffer[DATA_BUFSIZE];
   WSABUF DataBuf;
   DWORD BytesSEND;
   DWORD BytesRECV;
} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;

#endif // SOCKETINFO_H
