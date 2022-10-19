#ifndef XML_H
#define XML_H

#include <string>
#include <streambuf>
#include <type_traits>
#include <Windows.h>
#include <memory>
#include <Serializer/Charset.h>
#include <map>

namespace Serializer
{
	class XmlSerializer;
	class IXmlSerializable;
	template <typename _Tp>
	class XmlReader;
	template <typename _Tp>
	class XmlWriter;


	class IXmlSerializable
	{
	protected:
		virtual void Serialize(Serializer::XmlSerializer* serializer, bool serialize) = 0;
		template <typename _Tp>
		friend class XmlReader;
		template <typename _Tp>
		friend class XmlWriter;
	};


	typedef struct XmlTreeNode : public IXmlSerializable
	{
		virtual void Serialize(Serializer::XmlSerializer* serializer, bool serialize);
		enum class XmlNodeType
		{
			m_element, m_object, m_empty
		};
		union data
		{
			enum XmlNodeType Type = XmlNodeType::m_empty;
			std::string* element;
			std::map<std::string, std::shared_ptr<struct XmlTreeNode>>* object;
			~data();
		};
		void SetDataValue(std::string& value);
		void SetDataValue(std::string& elementName, std::shared_ptr<struct XmlTreeNode> value);
		void SetAttrbute(std::shared_ptr<std::string>& name, std::shared_ptr<std::string>& value);
		void SetName(std::shared_ptr<std::string>& m_name);
		std::shared_ptr<std::string> name;
		std::shared_ptr<std::map<std::string, std::string>> attribute;
		std::shared_ptr<union data> Data;
		XmlTreeNode();
		~XmlTreeNode();
	} XMLTREE;


	class XmlSerializer
	{
	protected:
		XmlSerializer();
		virtual ~XmlSerializer();
		virtual void Clear();
		virtual std::string* Serialize();
		virtual void SerializeElement(const std::string& className, const std::string& elementName, bool var);
		virtual void SerializeElement(const std::string& className, const std::string& elementName, int var);
		virtual void SerializeElement(const std::string& className, const std::string& elementName, const std::string& var);
		virtual void SerializeElement(const std::string& className, const std::string& elementName, IXmlSerializable& var);

		virtual void SerializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, bool var);
		virtual void SerializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, int var);
		virtual void SerializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, const std::string& var);

		virtual void DeserializeElement(const std::string& className, const std::string& elementName, bool& var);
		virtual void DeserializeElement(const std::string& className, const std::string& elementName, int& var);
		virtual void DeserializeElement(const std::string& className, const std::string& elementName, std::string& var);
		virtual void DeserializeElement(const std::string& className, const std::string& elementName, IXmlSerializable& var);

		virtual void DeserializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, bool& var);
		virtual void DeserializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, int& var);
		virtual void DeserializeAttribute(const std::string& className, const std::string& elementName, const std::string& attributeName, std::string& var);
	};



	template <typename _Tp>
	class XmlReader : public XmlSerializer
	{
		static_assert(std::is_base_of<IXmlSerializable, _Tp>::value, "");
	public:
		XmlReader()
		{
		};
		_Tp* Deserialize(std::string* str)
		{
			//root = std::make_shared<XMLTREE>(new XMLTREE);
			//base = std::make_shared<XMLTREE>(new XMLTREE);
			aimStr = str;
			root.reset(new XMLTREE);
			base.reset(new XMLTREE);
			_Tp* obj = new _Tp;
			BuildTree();
			obj->Serialize(this, true);
			return nullptr;
		};
		inline void GetBaseData()
		{
			std::shared_ptr<std::string> name, value;
			while (aimStr->at(ptr) != '>')
			{
				name = GetAttrName();
				if (aimStr->at(ptr) != '=')
				{
					throw std::logic_error("attribute must have value");
				}
				ptr++;
				value = GetAttrValue();
				base->SetAttrbute(name, value);
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
			checkElementName(name);
			return name;
		}


		inline std::shared_ptr<std::string> GetAttrName()
		{
			std::shared_ptr<std::string> name(new std::string);
			SkipBlank();
			while (aimStr->at(ptr) != ' ' && aimStr->at(ptr) != '\n' && aimStr->at(ptr) != '\t')
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
						if (aimStr->compare(ptr, 4, "amp"))
						{
							value->push_back('&');
							ptr += 3;
						}
						else if (aimStr->compare(ptr, 4, "apos"))
						{
							value->push_back('\'');
						}
						else
						{
							value->push_back('&');
						}
						break;
					case 'l':
						if (aimStr->compare(ptr, 3, "lt;"))
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
						if (aimStr->compare(ptr, 3, "gt;"))
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
						if (aimStr->compare(ptr, 3, "quot;"))
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
			SkipBlank();
			AimElement->SetName(thisElementName);
			while (aimStr->at(ptr) != '>')
			{
				if (isalpha(aimStr->at(ptr)) || ispunct(aimStr->at(ptr)))
				{
					SkipBlank();
					while (aimStr->at(ptr) != '>')
					{
						ptr++;
						name = GetAttrName();
						if (aimStr->at(ptr) != '=')
						{
							throw std::logic_error("attribute must have value");
						}
						ptr++;
						value = GetAttrValue();
						AimElement->SetAttrbute(value, name);
					}
				}
			}
			ptr++;
			if (aimStr->at(ptr) != '<')
			{
				value = GetValue();
				AimElement->SetDataValue(*value);
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
			if (aimStr->at(ptr) == '<') ptr++;
			if (aimStr->at(ptr) == '/') ptr++;
			else
			{
				std::shared_ptr<XMLTREE> subElement(new XMLTREE);
				name = GetElementName();
				AimElement->SetDataValue(*name, subElement);
				GetElement(subElement, name);
				if (aimStr->at(ptr) == '<') ptr++;
				if (aimStr->at(ptr) == '/') ptr++;
			}
			name = GetElementName();
			ptr++;
			if (*name != *(AimElement->name)) throw std::logic_error("element must close");
		};


		inline void GetElement(std::shared_ptr<XMLTREE>& AimElement)
		{
			std::shared_ptr<std::string> name, value;
			int counter = 0;
			SkipBlank();
			while (aimStr->at(ptr) == '<')
			{
				ptr++;
				while (aimStr->at(ptr) != '>')
				{
					if (aimStr->at(ptr) == '/') break;
					counter++;
					if (isalpha(aimStr->at(ptr)) || ispunct(aimStr->at(ptr)))
					{
						ptr++;
						name = GetElementName();
						AimElement->SetName(name);
						SkipBlank();
						while (aimStr->at(ptr) != '>')
						{
							name = GetAttrName();
							if (aimStr->at(ptr) == '=')
							{
								throw std::logic_error("attribute must have value");
							}
							ptr++;
							value = GetAttrValue();
							AimElement->SetAttrbute(value, name);
						}
					}
					ptr++;
					if (aimStr->at(ptr) != '<')
					{
						value = GetValue();
						AimElement->SetDataValue(*value);
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
				}
			}
			ptr++;
			name = GetElementName();
			if (*name != *(AimElement->name)) throw std::logic_error("element must close");
		};


		void BuildTree()
		{
			std::shared_ptr<XMLTREE> tmpTree;
			std::shared_ptr<std::string> tmpStr;
			if (aimStr->at(ptr) == '<')
			{
				ptr++;
				if (aimStr->compare(ptr, 4, "?xml") == 0)
				{
					ptr += 4;
					GetBaseData();
				}
				tmpTree.reset(new XMLTREE);
				GetElement(tmpTree);
			}

		};
		void Clear()
		{
			ptr = 0;
			root->~XmlTreeNode();
			base->~XmlTreeNode();
		};
		void checkElementName(std::shared_ptr<std::string>& name)
		{
			char check[] = "xml";
			char t = 0;
			if (name->size() < 3) return;
			for (int var = 0; var < 3; var++)
			{
				if (tolower(name->at(var)) == check[var]) t++;
			}
			if (t == 3) throw std::logic_error("element name shouldn\'t be start as xml");
		};
	protected:
		std::shared_ptr<XMLTREE> root;
		std::shared_ptr<XMLTREE> base;
		ICharsetEncoding* charset;
		size_t ptr = 0;
		std::string* aimStr;
	};


	template <typename _Tp>
	class XmlWriter : public XmlSerializer
	{
		static_assert(std::is_base_of<IXmlSerializable, _Tp>::value);
	public:
		XmlWriter();
		~XmlWriter();
		std::shared_ptr<std::stringbuf> Write(_Tp* object);
	protected:
		std::shared_ptr<std::stringbuf> buffer;
		XmlSerializer seralizer;
	};
#define BEGIN_XML_SERIALIZER_BASE(className, baseClassName)   \
		virtual void Serialize(Serializer::XmlSerializer * serializer, bool serialize) \
		{                                                                  \
			const std::tring __className = #className;                    \
																		   \
			if (std::is_base_of<baseClassName, IXmlSerializable>::Yes)      \
			{                                                               \
				baseClassName::Serialize(serializer, serialize);            \
			}

#define BEGIN_XML_SERIALIZER(className) \
		virtual void Serialize(Serializer::XmlSerializer * serializer, bool serialize) \
		{                                                                  \
			const std::string __className = #className;

#define END_XML_SERIALIZER()    \
		}

#define XML_ELEMENT(var)        \
			if (serialize)          \
			{                       \
				serializer->SerializeElement(__className, #var, var); \
			}   \
			else \
			{   \
				serializer->DeserializeElement(__className, #var, var);   \
			}

#define XML_ATTRIBUTE(element, var)      \
			if (serialize)          \
			{                       \
				serializer->SerializeAttribute(__className, #element, #var, var); \
			}   \
			else \
			{ \
				serializer->DeserializeAttribute(__className, #element, #var, var); \
			}
};
#endif