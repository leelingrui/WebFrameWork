#ifndef JSON_H
#define JSON_H
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <type_traits>
namespace Serializer
{
	class Json
	{
		enum DataType
		{
			m_null,
			m_bool,
			m_int,
			m_float,
			m_string,
			m_array,
			m_object
		};
		union Data
		{
			bool m_bool;
			int m_int;
			float m_float;
			std::shared_ptr<std::string> m_str;
			std::shared_ptr<std::vector<Json>> m_array;
			std::shared_ptr<std::map<std::shared_ptr<std::string> ,Json>> m_object;
		};
		Json();
	};

	IXmlSerizable::IXmlSerizable()
	{
	}

	IXmlSerizable::~IXmlSerizable()
	{
	}
}
#endif 