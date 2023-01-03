<script type="text/javascript" src="http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML"></script>
# project WebFrameWork
## Serialize
>### XMLSerialize
>
>xml tree object will be store at this xml data structure.$$\Downarrow$$
>```cpp
>typedef struct XmlTreeNode
>{
>	void Serialize(std::stringbuf &result);
>	void SubSerialize(std::stringbuf& result);
>	void SetDataValue(std::shared_ptr<std::string>& value);
>	std::shared_ptr<struct XmlTreeNode> GetValueByObjectName(const std::string& objectName);
>	std::shared_ptr<std::string> GetValue();
>	std::shared_ptr<std::string> GetValue(const std::string& objectName);
>	std::shared_ptr<std::string> GetAttrByName(const std::string& attrName);
>	const size_t GetArraySize();
>	void SetDataValue(const std::string& elementName, std::shared_ptr<struct XmlTreeNode>& value);
>	void SetAttrbute(const std::string& name, std::shared_ptr<std::string>& value);
>	void SetAttrbute(const std::string& name,const std::string& value);
>	void SetName(std::shared_ptr<std::string>& m_name);
>	void SetDataValue(std::shared_ptr<struct XmlTreeNode>& value);
>	std::shared_ptr<std::string> GetName();
>	std::shared_ptr<struct XmlTreeNode> GetArrayByNo(size_t No);
>	void Clear();
>	std::shared_ptr<std::string> name;
>	std::shared_ptr<std::map<std::string, std::shared_ptr<std::string>>> attribute;
>	std::variant<std::monostate, std::shared_ptr<std::string>, std::map<std::string, std::shared_ptr<struct XmlTreeNode>>, std::vector<std::shared_ptr<struct XmlTreeNode>>> Data;
>	XmlTreeNode();
>	~XmlTreeNode();
>} XMLTREE;
>```
>this structure inclue set tree node attribute��name��value functons you can safty store your data by
>use these provided functions.<br>
>it can serlize base data STL container and user DIY structure which must be dervied from IXmlSerializable.
>
>IXmlSerializable as follows
>```cpp
>	class IXmlSerializable
>	{
>	public:
>		virtual void Serialize(std::shared_ptr<XMLTREE> XmlTree, bool serialize) = 0;
>	};
>```
>
>you must use these definition to mark the variable and these serlize type witch you need to serlize
>```cpp
>BEGIN_XML_SERIALIZER(className)
>END_XML_SERIALIZER()
>XML_ELEMENT(var)
>XML_ATTRIBUTE(element, var)
>```
>---
>
>>### Deserialize
>>use XMLReader can be used to read a text and this class will help you create a new target type instanse.
>>
>>you can use Deserialize function to pass an string pointer the result is target type instance.
>>
>>### Serlize
>>you can use `Write()` function to serlize a instance and use `Clear()` to reset XmlReader inner flag.
>>
>>`void Write(_Tp& object)` will return a string object if you want write to a stream you can use `void Write(_Tp& object, std::ostream* output)` and this overloading will return nothing.
>>
>>Write will return the string of this object instance.
>>
>>---
>>***Attention*** if you need to use the same XmlReader and XmlWriter to deserlize multiple dataset you must use `clear()` function to reset the XmlReader inner flag
>
>### Examples of XML Serialization
>here is xml document.
>```XML
><?xml version="1.0" encoding="utf - 8" standalone="no"?>
><sites name="test deserlize object">
>    <site>
>        <name>RUNOOB</name>
>        <url>www.runoob.com</url>
>    </site>
>    <site>
>        <name>Google</name>
>        <url>www.google.com</url>
>    </site>
>    <site>
>        <name>Facebook</name>
>        <url>www.facebook.com</url>
>    </site>
></sites>"
>```
>serializing an instance of a public class, you can serialize an instance of a DataSet, as shown in the following code example:
>```cpp
>#include <iostream>
>#include <Serializer/Xml.h>
>#include <vector>
>using namespace std;
>using namespace Serializer;
>
>class site : public IXmlSerializable
>{
>public:
>	site()
>	{
>
>	};
>	BEGIN_XML_SERIALIZER(site) // you need mark your class name and variables
>	XML_ELEMENT(name)
>	XML_ELEMENT(url)
>	END_XML_SERIALIZER()
>public:
>	string name;
>	string url;
>};
>
>class sites : public IXmlSerializable //dervide IXmlSerializable is a symble to XmlWriter and XmlReader that this class can be derlizable or serlizable
>{
>public:
>	std::vector<site> site;
>	std::string name;
>	BEGIN_XML_SERIALIZER(sites)
>	XML_ELEMENT(site)
>	XML_ATTRIBUTE(sites, name) // mark the name of the node to which the tag attribute belongs
>	END_XML_SERIALIZER()
>};
>
>int main()
>{
>	string str = "<?xml version=\"1.0\" encoding=\"utf - 8\" standalone=\"no\"?><sites name = \"test deserlize object\"><site><name>RUNOOB</name><url>www.runoob.com</url></site><site><name>Google</name><url>www.google.com</url></site><site><name>Facebook</name><url>www.facebook.com</url></site></sites>";
>	XmlReader<sites> reader;
>	sites* sitesObject = reader.Deserialize(&str);
>	cout << sitesObject->name << endl;
>	for (int var = 0; var < sitesObject->site.size(); var++)
>	{
>		cout << sitesObject->site[var].name << endl;
>		cout << sitesObject->site[var].url << endl;
>		cout << "\n";
>	}
>	XmlWriter<sites> writer;
>	string xmlDocument = writer.Write(*sitesObject);
>	writer.Clear();
>	reader.Clear(); // don't forget reset the flag of serlizer
>	cout << xmlDocument << endl;
>	return EXIT_SUCCESS;
>}
>```
>The results as follows.
>```
>test deserlize object
>RUNOOB
>www.runoob.com
>
>Google
>www.google.com
>
>Facebook
>www.facebook.com
>
><?xml version = "1.0" encoding = "UTF - 8"?><sites name = "test deserlize object"><site><name>RUNOOB</name><url>www.runoob.com</url></site><site><name>Google</name><url>www.google.com</url></site><site><name>Facebook</name><url>www.facebook.com</url></site></sites>
>```
>by format the serlize document we can read these text easier.
>```XML
><?xml version = "1.0" encoding = "UTF - 8"?>
><sites name="test deserlize object">
>    <site>
>        <name>RUNOOB</name>
>        <url>www.runoob.com</url>
>    </site>
>    <site>
>        <name>Google</name>
>        <url>www.google.com</url>
>    </site>
>    <site>
>        <name>Facebook</name>
>        <url>www.facebook.com</url>
>    </site>
></sites>
>```

## Logger
>### ILoggerProvider
>it's a logger base class all of logger must derived from this base class it inclue many base function of logger.
>
>construct function can set log file path an log file max size of single page.
>when current file size greater than max size of single pange logger will rename current file name with current time and create a new file to write new message.
>```cpp
>   ILoggerProvider(size_t MaxLogSizePerPage = 256);
>   ILoggerProvider(std::string &logFilePath, size_t MaxLogSizePerPage = 256);
>```
>you can use `void Log(const LogLevel level, const EventId &eventId, const std::exception &exception, const std::string &message)` to write event message to console and files.
>
>ILoggerProvider also support log rotate, if you want customize file rotate size you can use `void resetLoggerMaxSize(int _size)` to change max size of single file of log file. ***(the units are MB default if 256MB)***
>
>you can use `void setLogLevel(const LogLevel& level)` change log level of logger, Logger will only output greater than or equal to log level log message, message log level less than log level will be dispose.
>
>if you want to add more output stream you can use `void insertNewOutputStream(std::ostream &output)` function, the output stream will be add to output queue, next time you use `log()` function message will alse be write down to this stream.
>
>---
>
>### LogLevel
>you can chose logger log level by useing function `void setLogLevel(LogLevel level)` and LogLevel definition as following.
>```cpp
>enum class LogLevel
>	{
>		Diagnose,
>		Debug,
>		Information,
>		Waring,
>		Error,
>		Fatal, 
>		None
>	};
>```
>
>---
>
>### EventID
>the eventID class can be used to give every event unique identit and you can give them a brief description.
>
>to get event unique identit you can use `GetId()` function, to get event brief description you can use `GetEventName()` function.
>
>---
>
>