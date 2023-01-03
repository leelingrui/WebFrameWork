#ifndef XML_H
#define XML_H

#include <string>
#include <streambuf>
#include <type_traits>
#include <Windows.h>
#include <string_view>
#include <memory>
#include <Serializer/Charset.h>
#include <variant>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Serializer
{
	class IXmlSerializable;
	template <typename _Tp>
	class XmlReader;
	template <typename _Tp>
	class XmlWriter;
	typedef struct XmlTreeNode XMLTREE;

	void SerializeElement(const std::string& className, const std::string& elementName, bool var, std::shared_ptr<XMLTREE> current);
	void SerializeElement(const std::string& className, const std::string& elementName, int var, std::shared_ptr<XMLTREE> current);
	void SerializeElement(const std::string& className, const std::string& elementName, const std::string& var, std::shared_ptr<XMLTREE> current);
	void SerializeElement(const std::string& className, const std::string& elementName, IXmlSerializable& var, std::shared_ptr<XMLTREE> current);

	void SerializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, bool var, std::shared_ptr<XMLTREE> current);
	void SerializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, int var, std::shared_ptr<XMLTREE> current);
	void SerializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, const std::string& var, std::shared_ptr<XMLTREE> current);

	void DeserializeElement(const std::string& className, const std::string& elementName, bool& var, std::shared_ptr<XMLTREE> current);
	void DeserializeElement(const std::string& className, const std::string& elementName, int& var, std::shared_ptr<XMLTREE> current);
	void DeserializeElement(const std::string& className, const std::string& elementName, std::string& var, std::shared_ptr<XMLTREE> current);
	void DeserializeElement(const std::string& className, const std::string& elementName, IXmlSerializable& var, std::shared_ptr<XMLTREE> current);

	void DeserializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, bool& var, std::shared_ptr<XMLTREE> current);
	void DeserializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, int& var, std::shared_ptr<XMLTREE> current);
	void DeserializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, std::string& var, std::shared_ptr<XMLTREE> current);
	class IXmlSerializable
	{
	public:
		virtual void Serialize(std::shared_ptr<XMLTREE> XmlTree, bool serialize) = 0;
	};


	typedef struct XmlTreeNode
	{
		void Serialize(std::streambuf* result);
		void SubSerialize(std::streambuf* result);
		void SetDataValue(std::shared_ptr<std::string>& value);
		std::shared_ptr<struct XmlTreeNode> GetValueByObjectName(const std::string& objectName);
		std::shared_ptr<std::string> GetValue();
		std::shared_ptr<std::string> GetValue(const std::string& objectName);
		std::shared_ptr<std::string> GetAttrByName(const std::string& attrName);
		const size_t GetArraySize();
		void SetDataValue(const std::string& elementName, std::shared_ptr<struct XmlTreeNode>& value);
		void SetAttrbute(const std::string& name, std::shared_ptr<std::string>& value);
		void SetAttrbute(const std::string& name,const std::string& value);
		void SetName(std::shared_ptr<std::string>& m_name);
		void SetDataValue(std::shared_ptr<struct XmlTreeNode>& value);
		std::shared_ptr<std::string> GetName();
		std::shared_ptr<struct XmlTreeNode> GetArrayByNo(size_t No);
		void Clear();
		std::shared_ptr<std::string> name;
		std::shared_ptr<std::map<std::string, std::shared_ptr<std::string>>> attribute;
		std::variant<std::monostate, std::shared_ptr<std::string>, std::map<std::string, std::shared_ptr<struct XmlTreeNode>>, std::vector<std::shared_ptr<struct XmlTreeNode>>> Data;
		XmlTreeNode();
		~XmlTreeNode();
	} XMLTREE;


	template <typename _Tp>
	void SerializeElement(const std::string& className, const std::string& elementName, std::vector<_Tp>& var, std::shared_ptr<XMLTREE> current)
	{
		std::shared_ptr<XMLTREE> tmp(new XMLTREE), loop;
		tmp->Data = std::vector<std::shared_ptr<XMLTREE>>();
		current->SetDataValue(elementName, tmp);
		for (int var1 = 0; var1 < var.size(); var1++)
		{
			loop.reset(new XMLTREE);
			var[var1].Serialize(loop, true);
			tmp->SetDataValue(loop);
		}
	}


	template <typename _Tp>
	void DeserializeElement(const std::string& className, const std::string& elementName, std::vector<_Tp>& var, std::shared_ptr<XMLTREE> current)
	{
		if (className != *(current->GetName())) throw std::logic_error("class name not correct");
		size_t size = current->GetValueByObjectName(elementName)->GetArraySize();
		var.resize(size);
		for (int var1 = 0; var1 < size; var1++)
		{
			var[var1].Serialize(current->GetValueByObjectName(elementName)->GetArrayByNo(var1), false);
		}
	}


	template <typename _Tp>
	class XmlReader
	{
		static_assert(std::is_base_of<IXmlSerializable, _Tp>::value, "attribute must be devided from IXmlSerializable");
	public:
		XmlReader() : aimStr(nullptr), charset(nullptr)
		{
		};

		_Tp* Deserialize(std::string* str)
		{
			aimStr = str;
			root.reset(new XMLTREE);
			base.reset(new XMLTREE);
			_Tp* obj = new _Tp;
			BuildTree();
			obj->Serialize(root, false);
			return obj;
		};

		void Clear()
		{
			ptr = 0;
		};
	protected:
		inline void GetBaseData()
		{
			std::shared_ptr<std::string> name, value;
			while (aimStr->at(ptr) != '>')
			{
				name = GetAttrName();
				SkipBlank();
				if (aimStr->at(ptr) != '=')
				{
					throw std::logic_error("attribute must have value");
				}
				ptr++;
				SkipBlank();
				value = GetAttrValue();
				base->SetAttrbute(*name, value);
				if (aimStr->at(ptr) == '?') ptr++;
			}
			ptr++;
		};
		inline void SkipBlank()
		{
			while (aimStr->at(ptr) == '\n' || aimStr->at(ptr) == '\t' || aimStr->at(ptr) == ' ') ptr++;
		};



		inline std::shared_ptr<std::string> GetElementName()
		{
			std::shared_ptr<std::string> name(new std::string);
			while (aimStr->at(ptr) != ' ' && aimStr->at(ptr) != '\n' && aimStr->at(ptr) != '\t' && aimStr->at(ptr) != '>')
			{
				name->push_back(aimStr->at(ptr));
				ptr++;
			}
			return name;
		}


		inline std::shared_ptr<std::string> GetAttrName()
		{
			std::shared_ptr<std::string> name(new std::string);
			SkipBlank();
			while (aimStr->at(ptr) != ' ' && aimStr->at(ptr) != '\n' && aimStr->at(ptr) != '\t' && aimStr->at(ptr) != '=')
			{
				name->push_back(aimStr->at(ptr));
				ptr++;
			}
			SkipBlank();
			return name;
		}


		inline std::shared_ptr<std::string> GetAttrValue()
		{
			std::shared_ptr<std::string> value(new std::string);
			SkipBlank();
			if (aimStr->at(ptr) == '\"')
			{
				ptr++;
				while (aimStr->at(ptr) != '\"')
				{
					value->push_back(aimStr->at(ptr));
					ptr++;
				}
			}
			ptr++;
			SkipBlank();
			return value;
		}


		inline std::shared_ptr<std::string> GetValue()
		{
			std::shared_ptr<std::string> value(new std::string);
			while (aimStr->at(ptr) != '<')
			{
				if (aimStr->at(ptr) != '&')
				{
					value->push_back(aimStr->at(ptr));
					ptr++;
				}
				else
				{
					ptr++;
					switch (aimStr->at(ptr))
					{
					case 'a':
						if (!aimStr->compare(ptr, 4, "amp;"))
						{
							value->push_back('&');
							ptr += 4;
						}
						else if (!aimStr->compare(ptr, 4, "apos;"))
						{
							value->push_back('\'');
							ptr += 4;
						}
						else
						{
							value->push_back('&');
						}
						break;
					case 'l':
						if (!aimStr->compare(ptr, 3, "lt;"))
						{
							value->push_back('<');
							ptr += 3;
						}
						else
						{
							value->push_back('&');
						}
						break;
					case 'g':
						if (!aimStr->compare(ptr, 3, "gt;"))
						{
							value->push_back('>');
							ptr += 3;
						}
						else
						{
							value->push_back('&');
						}
						break;
					case 'q':
						if (!aimStr->compare(ptr, 3, "quot;"))
						{
							value->push_back('\"');
							ptr += 3;
						}
						else
						{
							value->push_back('&');
						}
						break;
					default:
						value->push_back('&');
						break;
					}
				}
			}
			return value;
		}


		inline void GetElement(std::shared_ptr<XMLTREE>& AimElement, std::shared_ptr<std::string> thisElementName)
		{
			std::shared_ptr<std::string> name, value;
			AimElement->SetName(thisElementName);
			SkipBlank();
			if (isalpha(aimStr->at(ptr)) || ispunct(aimStr->at(ptr)))
			{
				while (aimStr->at(ptr) != '>')
				{
					name = GetAttrName();
					SkipBlank();
					if (aimStr->at(ptr) != '=')
					{
						throw std::logic_error("attribute must have value");
					}
					ptr++;
					SkipBlank();
					value = GetAttrValue();
					AimElement->SetAttrbute(*name, value);
				}
			}
			ptr++;
			if (aimStr->at(ptr) != '<')
			{
				value = GetValue();
				AimElement->SetDataValue(value);
			}
			else
			{
				ptr++;
				std::shared_ptr<XMLTREE> subElement(new XMLTREE);
				name = GetElementName();
				AimElement->SetDataValue(*name, subElement);
				GetElement(subElement, name);
			}
			SkipBlank();
			while (true)
			{
				if (aimStr->at(ptr) == '<') ptr++;
				if (aimStr->at(ptr) == '/') break;
				else
				{
					std::shared_ptr<XMLTREE> subElement(new XMLTREE);
					name = GetElementName();
					AimElement->SetDataValue(*name, subElement);
					GetElement(subElement, name);
				}
			}
			ptr++;
			name = GetElementName();
			ptr++;
			if (*name != *(AimElement->name)) throw std::logic_error("element must close");
		};


		inline void GetElement(std::shared_ptr<XMLTREE>& AimElement)
		{
			std::shared_ptr<std::string> name, value;
			int counter = 0;
			SkipBlank();
			ptr++;
			if (isalpha(aimStr->at(ptr)) || ispunct(aimStr->at(ptr)))
			{
				name = GetElementName();
				AimElement->SetName(name);
				SkipBlank();
				while (aimStr->at(ptr) != '>')
				{
					name = GetAttrName();
					SkipBlank();
					if (aimStr->at(ptr) != '=')
					{
						throw std::logic_error("attribute must have value");
					}
					ptr++;
					SkipBlank();
					value = GetAttrValue();
					AimElement->SetAttrbute(*name, value);
				}
			}
			ptr++;
			if (aimStr->at(ptr) != '<')
			{
				value = GetValue();
				AimElement->SetDataValue(value);
			}
			else
			{
				ptr++;
				std::shared_ptr<XMLTREE> subElement(new XMLTREE);
				name = GetElementName();
				AimElement->SetDataValue(*name, subElement);
				GetElement(subElement, name);
			}
			SkipBlank();
			while (true)
			{
				if (aimStr->at(ptr) == '<') ptr++;
				if (aimStr->at(ptr) == '/') break;
				else
				{
					std::shared_ptr<XMLTREE> subElement(new XMLTREE);
					name = GetElementName();
					AimElement->SetDataValue(*name, subElement);
					GetElement(subElement, name);
				}
			}
			ptr++;
			name = GetElementName();
			ptr++;
			if (*name != *(AimElement->name)) throw std::logic_error("element must close");
		};


		void BuildTree()
		{
			if (aimStr->at(ptr) == '<')
			{
				ptr++;
				if (aimStr->compare(ptr, 4, "?xml") == 0)
				{
					ptr += 4;
					GetBaseData();
				}
				else ptr--;
				GetElement(root);
			}

		};

		std::shared_ptr<XMLTREE> root, base;
		ICharsetEncoding* charset;
		size_t ptr = 0;
		std::string* aimStr;
	};


	template <typename _Tp>
	class XmlWriter
	{
		static_assert(std::is_base_of<IXmlSerializable, _Tp>::value, "attribute must be devided from IXmlSerializable");
	public:
		XmlWriter() : charset(nullptr) {};
		~XmlWriter() {};
		void Clear()
		{
			buffer.str("");
		}
		std::string Write(_Tp& object)
		{
			std::shared_ptr<XMLTREE> root(new XMLTREE);
			object.Serialize(root, true);
			root->Serialize(&buffer);
			return std::move(buffer.str());
		}
		void Write(_Tp& object, std::ostream* output)
		{
			std::shared_ptr<XMLTREE> root(new XMLTREE);
			object.Serialize(root, true);
			if (output->good())
			{
				root->Serialize(output->rdbuf());
				*output << std::flush;
			}
			else
			{
				throw std::runtime_error("output stream not open!");
			}
		}
	protected:
		ICharsetEncoding* charset;
		std::stringbuf buffer;
	};
#define BEGIN_XML_SERIALIZER(className) \
		virtual void Serialize(std::shared_ptr<XMLTREE> XmlTree, bool serialize) \
		{                                                                  \
			const std::string __className = #className;\
			if(serialize) \
			{\
				std::shared_ptr<std::string> __objectName(new std::string(__className)); \
				XmlTree->SetName(__objectName); \
			}

#define END_XML_SERIALIZER()    \
		}

#define XML_ELEMENT(var)        \
			if (serialize)          \
			{                       \
				Serializer::SerializeElement(__className, #var, var, XmlTree); \
			}   \
			else \
			{   \
				Serializer::DeserializeElement(__className, #var, var, XmlTree);   \
			}

#define XML_ATTRIBUTE(element, var)      \
			if (serialize)          \
			{                       \
				Serializer::SerializeAttribute(__className, #element, #var, var, XmlTree); \
			}   \
			else \
			{ \
				Serializer::DeserializeAttribute(__className, #element, #var, var, XmlTree); \
			}
};
#endif