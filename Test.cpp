#include <iostream>
#include <Windows.h>
#include <string>
#include <Serializer/Xml.h>
#include <vector>

using namespace std;
using namespace Serializer;

class Test : public IXmlSerializable
{
public:
	Test()
	{

	};
	BEGIN_XML_SERIALIZER(Test)
	END_XML_SERIALIZER()
private:
	int CC;
	string str = "sb";
};


int main()
{
	string test = "<?xml version = \"1.0\" encoding = \"UTF - 8\"?><sites><site><name>RUNOOB</name><url>www.runoob.com</url></site><site><name>Google</name><url>www.google.com</url></site><site><name>Facebook</name><url>www.facebook.com</url></site></sites>";
	XmlReader<Test> reader;
	reader.Deserialize(&test);
	return 0;
}