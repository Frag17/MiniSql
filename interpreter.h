#include<string>
#include"CatalogManager.h"
#include"RecordManager.h"
using std::string;
struct attr
{
	string name;
	string ty;
	bool uni;
	attr()
	{
		name="";
		ty="";
		uni=0;
	}
	attr(string NAME,string TY,bool UNI)
	{
		name=NAME;
		ty=TY;
		uni=UNI;
	}
};

struct condition
{
	string seg1;
	string op;
	string seg2;
	condition()
	{
		seg1="";
		seg2="";
		op="";
	}
	condition(string SEG1,string OP,string SEG2)
	{
		seg1=SEG1;
		op=OP;
		seg2=SEG2;
	}
};

class interpreter
{
private:
	string order;
	void set_order(string ORDER);
	void process();
	void quit();
	void create_table();
	void create_index();
	void drop_table();
	void drop_index();
	void select();
	void insert();
	void del();
	void exec();
	string next_state(char spl=0);
	string match(string format);
public:
	void interpreter_begin(string filename="");
};
