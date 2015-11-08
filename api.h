#include"RecordManager.h"
#include"IndexManager.h"
#include"CatalogManager.h"


enum Op {
	EQ, NE, LT, GT, LE, GE
};

struct AttributeInfo {
	Attribute attribute;
	int attributeOrder;
};

struct Condition {
	string attributeName;
	int attributeOrder;
	int type;
	Op op;
	string value;
};

struct IndexInfo {
	string IndexName;
	AttributeInfo attribute;
};


class api
{
public:
	string CreateTable(Table t);
	string DropTable(string name);
	string CreateIndex(string inname,string tabname,string arrname);
	string DropIndex(string inname);
	string Select(string tableName, vector<string>& attributes, vector<Condition>& conditions);
	//string Insert(string tabname,Tuple &v);
	string Del();
	string insert(string tableName, Tuple& tup);
private:
	string singleCheck(string& value, int type);
	string typeCheck(Tuple& Old,vector<Attribute>& attr, Tuple& New);
	string nameCheck(string name);
	int datacmp(const string& Lstr, const string& rstr, const int& type);	//�����ͷ��رȽ�ֵ,-1��ʾС�ڣ�0��ʾ���ڣ�1��ʾ����
};
