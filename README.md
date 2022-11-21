# project WebFrameWork

## Serialize

>### XMLSerialize
>---
>xml tree object will be store at this xml data structure$$\Downarrow$$
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
>this structure inclue set tree node attribute¡¢name¡¢value functons you can safty store your data by
>use these provided functions.

