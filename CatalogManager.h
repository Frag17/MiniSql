#ifndef _Catalog_Manager_
#define _Catalog_Manager_

#define CHAR 1
#define INT 2
#define FLOAT 3

class Attribute
{
public:
	Attribute(const std::string&name,int type,int length,bool isUnique,bool isPrimaryKey,bool hasIndex,const std::string& indexName)
		:name(name),type(type),length(length),isUnique(isUnique),
			isPrimaryKey(isPrimaryKey),hasIndex(hasIndex),indexName(indexName){}
	Attribute(){}
	~Attribute(){}

	std::string name;
	int type;//宏定义定义字段类型
	int length;
	bool isUnique;
	bool isPrimaryKey;
	bool hasIndex;
	std::string indexName;
};




class Table
{
public:
	Table(const std::string& tableName,const std::vector<Attribute>& attributes,int blockNum,int recordSize,const std::vector<int>& emptyRecordOffset)
		:tableName(tableName),attributes(attributes),blockNum(blockNum),
				recordSize(recordSize),emptyRecordOffset(emptyRecordOffset){}
	Table(){}
	~Table(){}
	std::string tableName;
	std::vector<Attribute> attributes;
	int blockNum;
	int recordSize;//一个record所占字节数
	std::vector<int> emptyRecordOffset;
};

class Index
{

public:
	Index(std::string indexName,std::string tableName,Attribute& attribute,int blockNum,std::vector<int>& emptyBlockOffset)
		:indexName(indexName),tableName(tableName),attribute(attribute),blockNum(blockNum),emptyBlockOffset(emptyBlockOffset){}
	Index(){}
	~Index(){}

	std::string indexName;
	std::string tableName;
	Attribute attribute;  //改成Attribute类 
	int blockNum;
	std::vector<int> emptyBlockOffset;
};

class CatalogManager
{

public:
	CatalogManager();
	~CatalogManager();
	
	void readCatalog();
	void writeCatalog();

	std::vector<Table*>::iterator findTable(const std::string& tableName);
	std::vector<Index*>::iterator findIndex(const std::string& indexName);

	void createTable(Table &table);
	void createIndex(Index &index);

	void dropTable(const std::string& tableName);
	void dropIndex(const std::string& indexName);

	bool existTable(const std::string& tableName);
	bool existIndex(const std::string& indexName);

	Table* getTablePtr(const std::string& tableName);
	Index* getIndexPtr(const std::string& indexName);

	std::vector<Table*>tables;
	std::vector<Index*>indexs;
};

#endif
