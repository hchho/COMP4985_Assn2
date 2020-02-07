#include "SocketInfo.h"
#include "ErrorHandler.h"

void CALLBACK WorkerRoutine(DWORD Error, DWORD BytesTransferred,
                            LPWSAOVERLAPPED Overlapped, DWORD InFlags) {
    // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
    DWORD RecvBytes;
    DWORD Flags;
    LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION) Overlapped;

    if (Error != 0)
    {
        ErrorHandler::showMessage("I/O operation failed with error");
    }

    if (BytesTransferred == 0)
    {
        ErrorHandler::showMessage("Closing socket");
    }

    if (Error != 0 || BytesTransferred == 0)
    {
        closesocket(SI->Socket);
        GlobalFree(SI);
        return;
    }

    SI->BytesRECV = 0;

    // Now that there are no more bytes to send post another WSARecv() request.

    Flags = 0;
    ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));

    SI->DataBuf.len = DATA_BUFSIZE;
    SI->DataBuf.buf = SI->Buffer;

    if (WSARecv(SI->Socket, &(SI->DataBuf), 1, &RecvBytes, &Flags,
                &(SI->Overlapped), WorkerRoutine) == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSA_IO_PENDING )
        {
            ErrorHandler::showMessage("WSARecv() failed with error %d\n");
            return;
        }
    }
}
