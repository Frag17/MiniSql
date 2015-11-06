#ifndef _Catalog_Manager_
#define _Catalog_Manager_

#include<string>
#include<vector>
#include<map>

#define CHAR 1
#define INT 2
#define FLOAT 3

class Attribute
{
public:
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
	Index(){}
	~Index(){}

	std::string indexName;
	std::string tableName;
	std::string attribute;
	int type;
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

	std::vector<Table*>::iterator findTable(std::string& tableName);
	std::vector<Index*>::iterator findIndex(std::string& indexName);

	void createTable(Table *table);
	void createIndex(Index *index);

	void dropTable(std::string& tableName);
	void dropIndex(std::string& indexName);

	bool existTable(std::string& tableName);
	bool existIndex(std::string& indexName);

	Table* getTablePtr(std::string& tableName);
	Index* getIndexPtr(std::string& indexName);

	std::vector<Table*>tables;
	std::vector<Index*>indexs;
};

#endif