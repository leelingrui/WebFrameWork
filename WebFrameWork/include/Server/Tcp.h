#ifndef TCP_H
#define TCP_H
#include <set>
#include <string>
#if (defined _WIN32) || (defined _WIN64)
#include <ws2tcpip.h>
#include <winSock2.h>
#include <windows.h>
#include <ThreadPool.h>
#include <Mswsock.h>
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib,"ws2_32.lib")

#else 

#include <sys/socket.h>

#endif

namespace Net
{
#if (defined _WIN32) || (defined _WIN64)
	struct IOContext : public OVERLAPPED
	{
	public:
		SOCKET ioSocket;
		CHAR* m_buffer;
		CHAR* reserved_buffer;
		std::queue<std::stringbuf> mseeage_queue;
		std::any UserObject;
		DWORD received_size;
		IOContext();
		~IOContext();
		void SetBuffer(CHAR* buffer, size_t size);
		void reset(std::any&& uobj = std::any());
		void switch_buffer();
	protected:
	};
#else
#endif
	class TcpServerBase
	{
	protected:
#pragma pack(push)
#pragma pack(1)
		using pack_head = struct
		{
			unsigned short length;
			// FF means this fragment is the final fragment of this message.
			char FF : 1;
		};
		int test()
		{
			sizeof(pack_head);
		}
#pragma pack(pop)
	public:
		TcpServerBase(const char* address, const unsigned short port, ULONG maxConn);
		void Send(std::string msg);
		void Send(char* msg, size_t length);
		std::string_view&& Recv(IOContext* context);
		bool IsAble();
		bool Accept();
		void Start();
		virtual ~TcpServerBase();
	protected:
		virtual void SystemSend(IOContext* context, char* msg, size_t send_size);
		virtual void SystemRecv(IOContext* context);
		virtual void session_up_side(IOContext* context) = 0;
		virtual void presentation_up_side(IOContext* context) = 0;
		virtual void session_down_side(IOContext* context) = 0;
		virtual void presentation_down_side(IOContext* context) = 0;
		virtual void application(IOContext* context) = 0;
		const static size_t kBufferSize = 4096;
		bool able;
		CHAR* ServerBuffer;
		ULONG left_connection;
#if (defined _WIN32) || (defined _WIN64)
		SOCKET serverfd;
		HANDLE Hiocp, HiocpRet;
		sockaddr_in server_addr;
		std::set<SOCKET> clientsfd;
		std::queue<IOContext*> objPool;
		IOContext* Contexts;
#else
#endif

	};
}



#endif