#ifndef XML_CPP
#define XML_CPP
#include <Serializer/Xml.h>

#define ELEMENT 1
#define OBJECT 2
#define ARRAY 3

namespace Serializer
{
	void XmlTreeNode::Serialize(std::stringbuf& result)
	{
		result.sputn("<?xml version = \"1.0\" encoding = \"UTF - 8\"?>", 44);
		result.sputc('<');
		result.sputn(name->c_str(), name->size());
		for (auto& attr : *attribute)
		{
			result.sputc(' ');
			result.sputn(attr.first.c_str(), attr.first.size());
			result.sputn(" = \"", 4);
			result.sputn(attr.second->c_str(), attr.second->size());
			result.sputc('\"');
		}
		result.sputc('>');
		switch (Data.index())
		{
		case ELEMENT:
			result.sputn(std::get<ELEMENT>(Data)->c_str(), std::get<ELEMENT>(Data)->size());
			break;
		case OBJECT:
			for (auto& obj : std::get<OBJECT>(Data))
			{
				obj.second->SubSerialize(result);
			}
			break;
		case ARRAY:
			for (auto& subElement : std::get<ARRAY>(Data))
			{
				subElement->SubSerialize(result);
			}
			break;
		default:
			break;
		}
		result.sputn("</", 2);
		result.sputn(name->c_str(), name->size());
		result.sputc('>');
		
	}

	void XmlTreeNode::SubSerialize(std::stringbuf& result)
	{
		switch (Data.index())
		{
		case ELEMENT:		result.sputc('<');
			result.sputn(name->c_str(), name->size());
			for (auto& attr : *attribute)
			{
				result.sputc(' ');
				result.sputn(attr.first.c_str(), attr.first.size());
				result.sputn(" = \"", 4);
				result.sputn(attr.second->c_str(), attr.second->size());
				result.sputc('\"');
			}
			result.sputc('>');
			result.sputn(std::get<ELEMENT>(Data)->c_str(), std::get<ELEMENT>(Data)->size());
			result.sputn("</", 2);
			result.sputn(name->c_str(), name->size());
			result.sputc('>');
			break;
		case OBJECT:		
			result.sputc('<');
			result.sputn(name->c_str(), name->size());
			for (auto& attr : *attribute)
			{
				result.sputc(' ');
				result.sputn(attr.first.c_str(), attr.first.size());
				result.sputn(" = \"", 4);
				result.sputn(attr.second->c_str(), attr.second->size());
				result.sputc('\"');
			}
			result.sputc('>');
			for (auto& obj : std::get<OBJECT>(Data))
			{
				obj.second->SubSerialize(result);
			}
			result.sputn("</", 2);
			result.sputn(name->c_str(), name->size());
			result.sputc('>');
			break;
		case ARRAY:
			for (auto& subElement : std::get<ARRAY>(Data))
			{
				subElement->SubSerialize(result);
			}
			break;
		default:
			result.sputn("</", 2);
			result.sputn(name->c_str(), name->size());
			result.sputc('>');
			break;
		}
	}

	void XmlTreeNode::SetDataValue(std::shared_ptr<std::string>& value)
	{
		std::shared_ptr<XMLTREE> tmp;
		switch (Data.index())
		{
		case OBJECT:
			throw std::logic_error("object type only can be set as key and value");
			break;
		case ELEMENT:
			tmp.reset(new XMLTREE);
			tmp->SetDataValue(std::get<ELEMENT>(Data));
			Data = std::vector<std::shared_ptr<XMLTREE>>(1, tmp);
			tmp.reset(new XMLTREE);
			tmp->SetDataValue(value);
			std::get<ARRAY>(Data).emplace_back(tmp);
			break; 
		case ARRAY:
			tmp.reset(new XMLTREE);
			tmp->SetDataValue(value);
			std::get<ARRAY>(Data).emplace_back(tmp);
			break;
		default:
			Data = value;
			break;
		}
	}


	void XmlTreeNode::SetAttrbute(const std::string& name,const std::string& value)
	{
		if (Data.index() == ARRAY) throw std::logic_error("array object have no Attrbute");
		(*attribute)[name] = std::make_shared<std::string>(value);
	}


	void XmlTreeNode::SetDataValue(std::shared_ptr<struct XmlTreeNode>& value)
	{
		std::shared_ptr<XMLTREE> tmp;
		switch (Data.index())
		{
		case ELEMENT:
			tmp.reset(new XMLTREE);
			tmp->SetDataValue(std::get<ELEMENT>(Data));
			Data = std::vector<std::shared_ptr<XMLTREE>>(1, tmp);
			std::get<ARRAY>(Data).emplace_back(value);
			break;
		case ARRAY:
			std::get<ARRAY>(Data).emplace_back(value);
			break;
		case OBJECT:
			throw std::logic_error("object type can\'t be set by value");
			break;
		default:
			Data = value->GetValue();
			break;
		}
	}


	std::shared_ptr<std::string> XmlTreeNode::GetAttrByName(const std::string& attrName)
	{
		std::map<std::string, std::shared_ptr<std::string>>::iterator&& iter = attribute->find(attrName);
		if (iter == attribute->end()) return std::make_shared<std::string>(std::string(""));
		return iter->second;
	}


	std::shared_ptr<XMLTREE> XmlTreeNode::GetArrayByNo(size_t No)
	{
		switch (Data.index())
		{
		case ARRAY:
			return std::get<ARRAY>(Data)[No];
		default:
			throw std::logic_error("only array type can be access by No.");
		}
	}


	const size_t XmlTreeNode::GetArraySize()
	{
		return std::get<ARRAY>(Data).size();
	}


	void XmlTreeNode::SetDataValue(const std::string& elementName, std::shared_ptr<XMLTREE>& value)
	{
		switch (Data.index())
		{
		case ELEMENT:
			throw std::logic_error("element type can not be set as key and value!");
			break;
		case OBJECT:
		{
			std::map<std::string, std::shared_ptr<struct XmlTreeNode>>& result = std::get<OBJECT>(Data);
			std::map<std::string, std::shared_ptr<struct XmlTreeNode>>::iterator&& iter = result.find(elementName);
			if (iter != result.end())
			{
				std::shared_ptr<XMLTREE> tmp(iter->second);
				switch (iter->second->Data.index())
				{
				case OBJECT: case ELEMENT:
					iter->second = std::shared_ptr<XMLTREE>(new XMLTREE);
					iter->second->Data = std::vector<std::shared_ptr<struct XmlTreeNode>>();
					iter->second->SetDataValue(tmp);
					iter->second->SetDataValue(value);
					break;

				case ARRAY:
					iter->second->SetDataValue(value);
					break;
				default:
					throw std::logic_error("unknow Error");
					break;
				}

			}
			else
			{
				result[elementName] = value;
			}
			break; 
		}
		case ARRAY:
			throw std::logic_error("array type can not be set as key and value");
		default:
			Data = std::map<std::string, std::shared_ptr<struct XmlTreeNode>>();
			std::get<OBJECT>(Data)[elementName] = value;
			break;
		}
	}


	std::shared_ptr<std::string> XmlTreeNode::GetName()
	{
		return name;
	}

	void XmlTreeNode::SetName(std::shared_ptr<std::string>& m_name)
	{
		name = m_name;
	}


	std::shared_ptr<XMLTREE> XmlTreeNode::GetValueByObjectName(const std::string& objectName)
	{
		return std::get<OBJECT>(Data)[objectName];
	}


	void XmlTreeNode::Clear()
	{
		Data = std::monostate();
	}


	std::shared_ptr<std::string> XmlTreeNode::GetValue()
	{
		return std::get<ELEMENT>(Data);
	}


	std::shared_ptr<std::string> XmlTreeNode::GetValue(const std::string& objectName)
	{
		return GetValueByObjectName(objectName)->GetValue();
	}


	void XmlTreeNode::SetAttrbute(const std::string& name, std::shared_ptr<std::string>& value)
	{
		if (Data.index() == ARRAY) throw std::logic_error("array object have no Attrbute");
		(*attribute)[name] = value;
	}


	XmlTreeNode::XmlTreeNode()
	{
		attribute.reset(new std::map<std::string, std::shared_ptr<std::string>>);
	}



	XmlTreeNode::~XmlTreeNode()
	{
	}


	void SerializeElement(const std::string& className, const std::string& elementName, bool var, std::shared_ptr<XMLTREE> current)
	{
		std::shared_ptr<std::string> tmp1(new std::string(className));
		std::shared_ptr<XMLTREE> tmp2(new XMLTREE);
		current->SetName(tmp1);
		tmp1.reset(new std::string(elementName));
		tmp2->SetName(tmp1);
		if (var) tmp1.reset(new std::string("true"));
		else tmp1.reset(new std::string("false"));
		tmp2->SetDataValue(tmp1);
		current->SetDataValue(elementName, tmp2);
	}


	void SerializeElement(const std::string& className, const std::string& elementName, int var, std::shared_ptr<XMLTREE> current)
	{
		std::shared_ptr<std::string> tmp1(new std::string(className));
		std::shared_ptr<XMLTREE> tmp2(new XMLTREE);
		current->SetName(tmp1);
		tmp1.reset(new std::string(elementName));
		tmp2->SetName(tmp1);
		tmp1.reset(new std::string(std::to_string(var)));
		tmp2->SetDataValue(tmp1);
		current->SetDataValue(elementName, tmp2);
	}


	void SerializeElement(const std::string& className, const std::string& elementName, const std::string& var, std::shared_ptr<XMLTREE> current)
	{
		std::shared_ptr<std::string> tmp1(new std::string(className));
		std::shared_ptr<XMLTREE> tmp2(new XMLTREE);
		current->SetName(tmp1);
		tmp1.reset(new std::string(elementName));
		tmp2->SetName(tmp1);
		tmp1.reset(new std::string(var));
		tmp2->SetDataValue(tmp1);
		current->SetDataValue(elementName, tmp2);
	}


	void SerializeElement(const std::string& className, const std::string& elementName, IXmlSerializable& var, std::shared_ptr<XMLTREE> current)
	{
		std::shared_ptr<XMLTREE> subTree(new XMLTREE);
		var.Serialize(subTree, true);
		current->SetDataValue(elementName, subTree);
	}


	void SerializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, bool var, std::shared_ptr<XMLTREE> current)
	{
		if (className == elementName)
		{
			if(var) current->SetAttrbute(attributeName, "true");
			else current->SetAttrbute(attributeName, "false");
		}
		else
		{
			if (className != *(current->GetName())) throw std::logic_error("class name incorrect");
			if(var) current->GetValueByObjectName(elementName)->SetAttrbute(attributeName, "true");
			else current->GetValueByObjectName(elementName)->SetAttrbute(attributeName, "false");
		}
	}


	void SerializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, int var, std::shared_ptr<XMLTREE> current)
	{
		if (className == elementName)
		{
			current->SetAttrbute(attributeName, std::to_string(var));
		}
		else
		{
			current->GetValueByObjectName(elementName)->SetAttrbute(attributeName, std::to_string(var));
		}
	}


	void SerializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, const std::string& var, std::shared_ptr<XMLTREE> current)
	{
		if (className == elementName)
		{
			current->SetAttrbute(attributeName, var);
		}
		else
		{
			current->GetValueByObjectName(elementName)->SetAttrbute(attributeName, var);
		}
	}


	void DeserializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, bool&var, std::shared_ptr<XMLTREE> current)
	{
		if (className != *(current->GetName())) throw std::logic_error("class name incorrect");
		if (className == elementName)
		{
			if (current->GetAttrByName(attributeName)->compare("true") == 0) var = true;
			else if(current->GetAttrByName(attributeName)->compare("false") == 0) var = false;
			else throw std::logic_error("the string \'" + (*current->GetAttrByName(attributeName)) + "\' is not a valid Boolean value");
		}
		else
		{
			if (className != *(current->GetName())) throw std::logic_error("class name incorrect");
			if (current->GetValueByObjectName(elementName)->GetAttrByName(attributeName)->compare("true")) var = true;
			else if (current->GetValueByObjectName(elementName)->GetAttrByName(attributeName)->compare("false") == 0) var = false;
			else throw std::logic_error("the string \'" + (*current->GetValueByObjectName(elementName)->GetAttrByName(attributeName)) + "\' is not a valid Boolean value");
		}
	}


	void DeserializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, int&var, std::shared_ptr<XMLTREE> current)
	{
		if (className != *(current->GetName())) throw std::logic_error("class name incorrect");
		if (className == elementName)
		{
			var = std::stoi(*current->GetAttrByName(attributeName));
		}
		else
		{
			var = std::stoi(*current->GetValueByObjectName(elementName)->GetAttrByName(attributeName));
		}
	}

	void DeserializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, std::string& var, std::shared_ptr<XMLTREE> current)
	{
		if (className != *(current->GetName())) throw std::logic_error("class name incorrect");
		if (className == elementName)
		{
			var = *current->GetAttrByName(attributeName);
		}
		else
		{
			var = *current->GetValueByObjectName(elementName)->GetAttrByName(attributeName);
		}
	}


	void DeserializeElement(const std::string& className, const std::string& elementName, bool& var, std::shared_ptr<XMLTREE> current)
	{
		if (className != *(current->GetName())) throw std::logic_error("class name incorrect");
		if (current->GetValueByObjectName(elementName)->GetValue()->compare("true")) var = true;
		else if (current->GetValueByObjectName(elementName)->GetValue()->compare("false")) var = false;
		else throw std::logic_error("the string \'" + (*current->GetValueByObjectName(elementName)->GetValue()) + "\' is not a valid Boolean value");
	}

	void DeserializeElement(const std::string& className, const std::string& elementName, IXmlSerializable& var, std::shared_ptr<XMLTREE> current)
	{
		if (className != *(current->GetName())) throw std::logic_error("class name incorrect");
		var.Serialize(current->GetValueByObjectName(elementName), false);
	}


	void DeserializeElement(const std::string& className, const std::string& elementName, std::string& var, std::shared_ptr<XMLTREE> current)
	{
		if (className != *(current->GetName())) throw std::logic_error("class name incorrect");
		var = *(current->GetValue(elementName));
	}


	void DeserializeElement(const std::string& className, const std::string& elementName, int& var, std::shared_ptr<XMLTREE> current)
	{
		if (className != *(current->GetName())) throw std::logic_error("class name incorrect");
		var = std::stoi(*(current->GetValue(elementName)));
	}
}
#undef OBJECT
#undef ELEMENT
#undef ARRAY
#endif