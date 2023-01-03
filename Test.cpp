#include <iostream>
#include <Server/TCP.h>
#include <string>
#include <Serializer/Xml.h>
#include <vector>
#include <variant>
#include <sstream>
#include <ThreadPool.h>
#include <Logger.h>
#include <MemoryPool.h>
#include <chrono>
#include <spin_lock.h>
#include <ConcurrentAlloc.h>
#include <coroutine>
#include <type_traits>
using namespace std;
using namespace Serializer;
using namespace thread;
using namespace logger;
using namespace memory;
using namespace lock;

//class site : public IXmlSerializable
//{
//public:
//	site()
//	{
//
//	};
//	BEGIN_XML_SERIALIZER(site) // you need mark your class name and variables
//	XML_ELEMENT(name)
//	XML_ELEMENT(url)
//	END_XML_SERIALIZER()
//public:
//	string name;
//	string url;
//};
//
//class sites : public IXmlSerializable
//{
//public:
//	std::vector<site> site;
//	std::string name;
//	BEGIN_XML_SERIALIZER(sites)
//	XML_ELEMENT(site)
//	XML_ATTRIBUTE(sites, name) // mark the name of the node to which the tag attribute belongs
//	END_XML_SERIALIZER()
//};

//int main()
//{
//	string str = "<?xml version=\"1.0\" encoding=\"utf - 8\" standalone=\"no\"?><sites name = \"test deserlize object\"><site><name>RUNOOB</name><url>www.runoob.com</url></site><site><name>Google</name><url>www.google.com</url></site><site><name>Facebook</name><url>www.facebook.com</url></site></sites>";
//	XmlReader<sites> reader;
//	sites* sitesObject = reader.Deserialize(&str);
//	cout << sitesObject->name << endl;
//	for (int var = 0; var < sitesObject->site.size(); var++)
//	{
//		cout << sitesObject->site[var].name << endl;
//		cout << sitesObject->site[var].url << endl;
//		cout << "\n";
//	}
//	XmlWriter<sites> writer;
//	std::fstream of;
//	of.open("test.xml", 2);
//	writer.Write(*sitesObject, &of);
//	return EXIT_SUCCESS;
//}



//int testfunc()
//{
//	return 1;
//}
//
//int testfuncc(const int c)
//{
//	cout << c << endl;
//	return c;
//}


//struct HelloCoroutine {
//	struct HelloPromise {
//		std::string_view value_;
//		HelloCoroutine get_return_object() {
//			return std::coroutine_handle<HelloPromise>::from_promise(*this);
//		}
//		std::suspend_never initial_suspend() { return {}; }
//		// �� final_suspend() ������Э�̣�����Ҫ�ֶ� destroy
//		std::suspend_always final_suspend() noexcept { return {}; }
//		std::suspend_always yield_value(std::string_view value) {
//			value_ = value;
//			std::cout << value_ << std::endl;
//			return {};
//		}
//		void return_void() {}
//		void unhandled_exception() {}
//	};
//
//	using promise_type = HelloPromise;
//	HelloCoroutine(std::coroutine_handle<HelloPromise> h) : handle(h) {}
//
//	std::coroutine_handle<HelloPromise> handle;
//};
//

threadPoolBase tp(12);

Task<int> hello() {
	std::cout << "Hello " << std::endl;
	std::cout << co_await suspend_task<int>(&tp) << endl;
	std::cout << "world!" << std::endl;
	co_return 5;
}

int test() {
	cout << "hi";
	return 0;
}

class C
{
public:
	int print()
	{
		cout << str << endl;
		return 0;
	}
	string str;
};

class TcpServer : public Net::TcpServerBase
{
public:
	TcpServer(const char* address, const unsigned short port, ULONG maxConn) : TcpServerBase(address, port, maxConn) {};
	void session_down_side(Net::IOContext* context) {};
	void session_up_side(Net::IOContext* context) {};
	void presentation_up_side(Net::IOContext* context) {};
	void presentation_down_side(Net::IOContext* context) {};
protected:
	void application(Net::IOContext* context)
	{
		string_view received = Recv(context);
	};
private:
};

int main()
{
	TcpServer server("127.0.0.1", 12345, 50);
	char* s;
	s = (char*)malloc(13);
	if (s == nullptr) return EXIT_SUCCESS;
	strcpy(s, "hello world!");
	string* str = new string(s);
	delete str;
	cout << s << endl;
	//int err = WSAGetLastError();
	//server.Start();
	//Sleep(100000000);
	return EXIT_SUCCESS;
}