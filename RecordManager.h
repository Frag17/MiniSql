#ifndef _Record_Manager_
#define _Record_Manager_
#include"CatalogManager.h"
#include"BufferManager.h"

#include<vector>
#include<string>

#define INTSIZE sizeof(int);
#define CHARSIZE sizeof(char);
#define FLOATSIZE sizeof(float);

//enum Op
//{
//	EQ,NE,LT,GT,LE,GE,
//};
//
//
//class Condition
//{
//public:
//	Attribute attribute;
//	Op op;
//	std::string value;
//};
//typedef std::vector<Condition> Conditions;

typedef std::vector<std::string> Tuple;
typedef std::vector<Tuple> Data;
typedef std::vector<std::string> SingleData;

class RecordManager
{
public:
	RecordManager(){}; 
	~RecordManager(){};

	void createTable(Table& table);	//创建文件
	void dropTable(Table& table);		//删除文件
	void closeTable(Table & table);//关闭暂时不用的文件,释放出占用的缓存

	int insertRecord(Table& table,Tuple& tuple);//将tuple插入文件，返回一个ptr = (blockOffset << 16)|start
												//没有index情况下的插入
	
	void deleteTable(Table& table);
	void deleteByPrimaryKey(Table& table, std::vector<std::string>& PrimaryKey);

	void selectRecord(Table& table,Data &data);//data是装有tuple的vector     加入：参数Data 
	void selectByPointer(Table& table, std::vector<int>& Pointers, Data& data);							//新增


	void selectAttribute(Table& table, int attributeOrder, SingleData& data,std::vector<int> &ptrs);//再加个ptr??

	Tuple recordToTuple(Table & table,std::string record);

	std::string tupleToRecord(Tuple & tuple);
	
	

};



#endif

