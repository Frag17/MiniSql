#pragma once
#include <string>
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
