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

	void createTable(Table& table);	//�����ļ�
	void dropTable(Table& table);		//ɾ���ļ�
	void closeTable(Table & table);//�ر���ʱ���õ��ļ�,�ͷų�ռ�õĻ���

	int insertRecord(Table& table,Tuple& tuple);//��tuple�����ļ�������һ��ptr = (blockOffset << 16)|start
												//û��index����µĲ���
	
	void deleteTable(Table& table);
	void deleteByPrimaryKey(Table& table, std::vector<std::string>& PrimaryKey);

	void selectRecord(Table& table,Data &data);//data��װ��tuple��vector     ���룺����Data 
	void selectByPointer(Table& table, std::vector<int>& Pointers, Data& data);							//����


	void selectAttribute(Table& table, int attributeOrder, SingleData& data,std::vector<int> &ptrs);//�ټӸ�ptr??

	Tuple recordToTuple(Table & table,std::string record);

	std::string tupleToRecord(Tuple & tuple);
	
	

};



#endif

