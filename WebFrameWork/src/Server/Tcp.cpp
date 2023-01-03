#include <Server/Tcp.h>

namespace Net
{
#if (defined _WIN64) || (defined _WIN32)

    IOContext::IOContext()
    {
        memset(this, 0, sizeof(IOContext));
        ioSocket = INVALID_SOCKET;
    }

    void IOContext::SetBuffer(CHAR* buffer, size_t size)
    {
        m_buffer = buffer;
        reserved_buffer = buffer + size;
    }

    void IOContext::reset(std::any&& uobj)
    {
        UserObject = uobj;
    }

    TcpServerBase::TcpServerBase(const char* address, const unsigned short port, ULONG maxConn) :  \
        serverfd(0), server_addr(), able(false), Hiocp(0), HiocpRet(0), Contexts(nullptr),         \
        ServerBuffer(nullptr)
    {
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;
        left_connection = maxConn;
        wVersionRequested = MAKEWORD(2, 2);
        err = WSAStartup(wVersionRequested, &wsaData);
        if (err != 0)
        {
            //DEBUG_KEYI(("\n\n-----TCP WSAStartup failed------\n\n"));
            return;
        }
        serverfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverfd == SOCKET_ERROR)
        {
            return;
        }
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        inet_pton(AF_INET, address, &server_addr.sin_addr);
        server_addr.sin_port = htons(port);
        int nRet = bind(serverfd, (sockaddr*)&server_addr, sizeof(server_addr));
        if (nRet == SOCKET_ERROR)
        {
            return;
        }
        nRet = listen(serverfd, SOMAXCONN);
        if (nRet == SOCKET_ERROR)
        {
            return;
        }
        Hiocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
        HiocpRet = CreateIoCompletionPort((HANDLE)serverfd, Hiocp, NULL, 0);
        ServerBuffer = (CHAR*)malloc((maxConn) * kBufferSize * 2);
        Contexts = new IOContext[maxConn];
        for (ULONG var = 0; var < maxConn; var++)
        {
            Contexts[var].SetBuffer(ServerBuffer + var * kBufferSize * 2, kBufferSize);
            objPool.push(Contexts + var);
        }
        able = true;
    }

    void IOContext::switch_buffer()
    {
        std::swap(reserved_buffer, m_buffer);
    }

    IOContext::~IOContext()
    {
    }

    bool TcpServerBase::Accept()
    {
        IOContext* clientContext = objPool.front();
        clientContext->reset();
        clientContext->ioSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (clientContext->ioSocket == SOCKET_ERROR) throw std::runtime_error("create client socket failed");
        HANDLE hRet = CreateIoCompletionPort(
            (HANDLE)clientContext->ioSocket,
            Hiocp,
            NULL,
            0);
        int err = WSAGetLastError();
        objPool.pop();
        clientsfd.insert(clientContext->ioSocket);
        return AcceptEx(serverfd, clientContext->ioSocket, clientContext->m_buffer, kBufferSize - (sizeof(sockaddr) + 16) * 2,\
            sizeof(sockaddr) + 16, sizeof(sockaddr) + 16, &clientContext->received_size, clientContext);
    }

    TcpServerBase::~TcpServerBase()
    {
        free(ServerBuffer);
        delete[] Contexts;
    }

    void TcpServerBase::Start()
    {
        LPOVERLAPPED_ENTRY entrys = new OVERLAPPED_ENTRY[left_connection];
        ULONG removed;
        if (!IsAble())
        {
            throw std::runtime_error("Error occured! Server not able to start");
        }
        Accept();
        SetLastError(0);
        while (true)
        {
            BOOL QRet = GetQueuedCompletionStatusEx(Hiocp, entrys, left_connection, &removed, INFINITE, false);
            left_connection -= removed;
            for (ULONG var = 0; var < removed; var++)
            {
                if (entrys[var].Internal == ERROR_SUCCESS)
                {
                    session_up_side((IOContext*)entrys[var].lpOverlapped);
                    presentation_up_side((IOContext*)entrys[var].lpOverlapped);
                    application((IOContext*)entrys[var].lpOverlapped);
                }
            }
        }
    }

    std::string_view&& TcpServerBase::Recv(IOContext* context)
    {
        return std::string_view();
    }

    void TcpServerBase::Send(std::string msg)
    {
    }

    void TcpServerBase::application(IOContext* context)
    {
        Send(context->m_buffer);
    };

    void TcpServerBase::SystemSend(IOContext* context,char * msg, size_t send_size)
    {

    }
    void TcpServerBase::SystemRecv(IOContext* context)
    {

    }
#else

#endif
    bool TcpServerBase::IsAble()
    {
        return able;
    }
}