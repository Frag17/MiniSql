#ifndef _Record_Manager_
#define _Record_Manager_
#include"CatalogManager.h"
#include"BufferManager.h"

#include<vector>
#include<string>

//enum Op
//{
//	EQ,NE,LT,GT,LE,GE,
//};
//
//struct Condition
//{
//	Attribute attribute;
//	Op op;
//	std::string value;
//};

typedef std::vector<std::string> Tuple;
typedef std::vector<Tuple> Data;
typedef std::vector<Condition> Conditions;

class RecordManager
{
public:
	RecordManager() {};
	~RecordManager() { };

	bool compare(Table &table, Tuple& tuple, Conditions& conditions);//比较一行是否符合筛选条件

	bool createTable(std::string& tableName);	//table    创建文件成功返回1，失败返回0
	bool dropTable(std::string& tableName);		//table    删除文件成功返回1，失败返回0


	int insertRecord(Table& table, Tuple& tuple);//table，record			成功返回ptr，失败返回-1			//修改	
												 //Tuple 为装string 的vector。每一个string即为每一个值转换为字符串

	bool deleteRecord(Table& table);
	bool deleteByPrimaryKey(Table& table, vector<std::string>& PrimaryKey);							//新增

	void selectRecord(Table& table, Data& data);//data是装有tuple的vector							//修改
	void selectByPointer(Table& table, vector<int>& Pointers, Data& data);						//新增
	void selectAttribute(Table& table, int attributeOrder, Data& data);						//新增

	Tuple recordToTuple(Table & table, std::string record);
	std::string tupleToRecord(Table & table, Tuple & tuple);

};



#endif