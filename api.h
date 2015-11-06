
enum Op {
	EQ, NE, LT, GT, LE, GE
};

struct AttributeInfo {
	string attribute;
	int attributeOrder;
};

struct Condition {
	AttributeInfo attributeOrder;
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
	string CreateTable(table t);
	string DropTable(string name);
	string CreateIndex(string inname,string tabname,string arrname);
	string DropIndex(string inname);
	string Select();
	string Insert(string tabname,Tuple &v);
	string Del();
	string insert(string tableName, Tuple& tup);
private:
	string typeCheck(Tuple& Old,vector<Attribute>& attr, Tuple& New);
	string nameCheck(string name);
};
