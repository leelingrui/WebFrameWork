#include <iostream>
#include <Windows.h>
#include <string>
#include <Serializer/Xml.h>
#include <vector>
#include <variant>
#include <sstream>
#include <ThreadPool.h>
#include <Logger.h>
#include <MemoryPool.h>
#include <chrono>
#include <coroutine>
#include <stdio.h>
#include <type_traits>
using namespace std;
using namespace Serializer;
using namespace thread;
using namespace Logger;
using namespace memory;
class site : public IXmlSerializable
{
public:
	site()
	{

	};
	BEGIN_XML_SERIALIZER(site)
	XML_ELEMENT(name)
	XML_ELEMENT(url)
	END_XML_SERIALIZER()
public:
	string name;
	string url;
};

class sites : public IXmlSerializable
{
public:
	std::vector<site> site;
	std::string name;
	BEGIN_XML_SERIALIZER(sites)
	XML_ELEMENT(site)
	XML_ATTRIBUTE(sites, name)
	END_XML_SERIALIZER()
};


int testfunc()
{
	return 1;
}

int testfuncc(const int c)
{
	cout << c << endl;
	return c;
}


//struct HelloCoroutine {
//	struct HelloPromise {
//		std::string_view value_;
//		HelloCoroutine get_return_object() {
//			return std::coroutine_handle<HelloPromise>::from_promise(*this);
//		}
//		std::suspend_never initial_suspend() { return {}; }
//		// 在 final_suspend() 挂起了协程，所以要手动 destroy
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
	co_await Task<int>::suspend(&tp);
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

int main()
{
	
	//string test = "<?xml version = \"1.0\" encoding = \"UTF - 8\"?><sites name = \"test\"><site><name>RUNOOB</name><url>www.runoob.com</url></site><site><name>Google</name><url>www.google.com</url></site><site><name>Facebook</name><url>www.facebook.com</url></site></sites>";
	//XmlReader<sites> reader;
	//sites *result = reader.Deserialize(&test);
	//XmlWriter<sites> writer;
	//std::shared_ptr<std::stringbuf> sb = writer.Write(*result);
	//delete result;
	//test = sb->str();
	//cout << test << endl;
	//reader.Clear();
	//result = reader.Deserialize(&test);
	//string path("./test/testlog.log"), name("test");
	//ILoggerProvider logger(path);
	//logger.Log(LogLevel::Debug, EventId(5), std::logic_error("error"), "test");
	//threadPoolBase pool;

	//threadPoolBase pool(24);
	//for(int var = 0; var < 6000; var++)
	//	pool.submit(testfuncc, var);
	//std::this_thread::sleep_for(std::chrono::seconds(10));
	//Task task = hello();
	//while (!task.finished()) task.handle.resume();
	//task.handle.destroy();
	//task = hello();
	//while (!task.finished()) task.handle.resume();
	//task.handle.destroy();
	//int c = asm_getcf();

	auto fu = tp.submit(hello);
	//fu = tp.submit(test);
	//fu.GetResult();
	std::this_thread::sleep_for(1s);
	tp.resume<Task<int>>(fu);
	std::cout << fu.GetResult() << std::endl;
	std::this_thread::sleep_for(100s);
	return 0;
}