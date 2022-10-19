#ifndef XML_CPP
#define XML_CPP
#include <Serializer/Xml.h>
namespace Serializer
{
	XmlSerializer::XmlSerializer()
	{
	}


	Serializer::XmlSerializer::~XmlSerializer()
	{
	}


	void XmlSerializer::Clear()
	{
	}


	XmlTreeNode::data::~data()
	{
		switch (Type)
		{
		case XmlNodeType::m_element:
			delete element;
			break;
		case XmlNodeType::m_object:
			delete object;
			break;
		default:
			break;
		}
	}


	std::string* XmlSerializer::Serialize()
	{
		return nullptr;
	}


	void XmlTreeNode::SetDataValue(std::string& value)
	{
		switch (Data->Type)
		{
		case XmlNodeType::m_element:
			Data->~data();
			Data->element = new std::string(value);
			break;
		case XmlNodeType::m_object:
			throw std::logic_error("object must be set as key and value");
			break;
		default:
			Data->element = new std::string(value);
			break;
		}
	}


	void XmlTreeNode::SetDataValue(std::string& elementName, std::shared_ptr<XMLTREE> value)
	{
		switch (Data->Type)
		{
		case XmlNodeType::m_element:
			throw std::logic_error("element Type only need value");
			break;
		case XmlNodeType::m_object:
			(*Data->object)[elementName] = value;
			break;
		default:
			Data->object = new std::map<std::string, std::shared_ptr<XMLTREE>>;
			(*Data->object)[elementName] = value;
			break;
		}
	}


	void XmlTreeNode::SetName(std::shared_ptr<std::string>& m_name)
	{
		name = m_name;
	}


	void XmlTreeNode::SetAttrbute(std::shared_ptr<std::string>& name, std::shared_ptr<std::string>& value)
	{
		(*attribute)[*name] = *value;
	}


	XmlTreeNode::XmlTreeNode()
	{
		Data.reset(new union data);
		attribute.reset(new std::map<std::string, std::string>);
	}



	XmlTreeNode::~XmlTreeNode()
	{
	}

	void XmlTreeNode::Serialize(Serializer::XmlSerializer* serializer, bool serialize)
	{
	}


	void XmlSerializer::SerializeElement(const std::string& className, const std::string& elementName, bool var)
	{
	}


	void XmlSerializer::SerializeElement(const std::string& className, const std::string& elementName, int var)
	{
	}


	void XmlSerializer::SerializeElement(const std::string& className, const std::string& elementName, const std::string& var)
	{
	}


	void XmlSerializer::SerializeElement(const std::string& className, const std::string& elementName, IXmlSerializable& var)
	{
	}


	void XmlSerializer::SerializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, bool var)
	{
	}


	void XmlSerializer::SerializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, int var)
	{
	}


	void XmlSerializer::SerializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, const std::string& var)
	{
	}


	void XmlSerializer::DeserializeElement(const std::string& className, const std::string& elementName, bool& var)
	{
	}


	void XmlSerializer::DeserializeElement(const std::string& className, const std::string& elementName, int& var)
	{
	}


	void XmlSerializer::DeserializeElement(const std::string& className, const std::string& elementName, std::string& var)
	{
	}


	void XmlSerializer::DeserializeElement(const std::string& className, const std::string& elementName, IXmlSerializable& var)
	{
	}


	void XmlSerializer::DeserializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, bool& var)
	{
	}


	void XmlSerializer::DeserializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, int& var)
	{
	}


	void XmlSerializer::DeserializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, std::string& var)
	{
	}
}
#endif