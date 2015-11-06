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

	bool compare(Table &table, Tuple& tuple, Conditions& conditions);//�Ƚ�һ���Ƿ����ɸѡ����

	bool createTable(std::string& tableName);	//table    �����ļ��ɹ�����1��ʧ�ܷ���0
	bool dropTable(std::string& tableName);		//table    ɾ���ļ��ɹ�����1��ʧ�ܷ���0


	int insertRecord(Table& table, Tuple& tuple);//table��record			�ɹ�����ptr��ʧ�ܷ���-1			//�޸�	
												 //Tuple Ϊװstring ��vector��ÿһ��string��Ϊÿһ��ֵת��Ϊ�ַ���

	bool deleteRecord(Table& table);
	bool deleteByPrimaryKey(Table& table, vector<std::string>& PrimaryKey);							//����

	void selectRecord(Table& table, Data& data);//data��װ��tuple��vector							//�޸�
	void selectByPointer(Table& table, vector<int>& Pointers, Data& data);						//����
	void selectAttribute(Table& table, int attributeOrder, Data& data);						//����

	Tuple recordToTuple(Table & table, std::string record);
	std::string tupleToRecord(Table & table, Tuple & tuple);

};



#endif