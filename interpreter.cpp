#include"StdAfx.h"
using std::vector;

extern api Api;
void interpreter::set_order(string ORDER)
{
	order=ORDER;
}
string interpreter::next_state(char spl)
{
	bool be=0;
	string ty="";
	int l=order.length();
	for(int i=0;i<l;i++)
	{
		char lt=order[0];
		order=order.substr(1,order.length()-1);
		if(lt==spl)
			return ty+spl;
		if(lt==' '||lt=='	')
		{
			if(be==1)
			{
				if(spl==0)
					return ty;
			}
		}
		else 
		{
			if(lt=='\'')
				spl='\'';
			be=1;
			ty+=lt;
		} 
	}
	return ty;
}
string  interpreter::match(string format)
{
	string re="get",lt1="",lt2;
	format+=" ";
	for(int x=0;x<format.length();x++)
	{
		if(format[x]!=' ')
			lt1+=format[x];
		else 
		{
			lt2=next_state();
			if(lt1=="*")
				re=lt2;
			else if(lt2!=lt1)
				return "";
			lt1="";
		}
	}
	return re;
}
void attr(string lt1,string lt2,bool uni,string length,Attribute &v)
{
	v.name=lt1;
	v.isUnique=uni;
	v.hasIndex = 0;
	v.indexName = "";
	v.isPrimaryKey = 0;
	if(lt2=="int")
		v.type=2;
	else if(lt2=="char")
	{
		v.type=1;
		v.length=atoi(length.c_str());
	}	
	else if(lt2=="float")
		v.type=3;
	else 
		v.type=0;
}

void Cond(string attrname,string op,string value,Condition &condition)
{
	condition.attributeName=attrname;
	condition.value=value;
	if(op=="=")
		condition.op=EQ;
	else if(op=="<")
		condition.op=LT;
	else if(op==">")
		condition.op=GT;
	else if(op=="<=")
		condition.op=LE;
	else if(op==">=")
		condition.op=GE;
	else if(op=="<>")
		condition.op=NE;
}
void interpreter::quit()
{
	printf("Quit successful\n");
	exit(0);
}
void interpreter::create_table()
{
	string lt,name,lt1,lt2,lt3;
	Table t;
	Attribute attribute;
	t.tableName=next_state();
	lt=next_state();
	if(lt!="(")
	{
		printf("Syntax error,need( after %s !\n",lt.c_str());
		return ;
	}
	while(true)
	{
		lt1=next_state();
		lt2=next_state();
		lt3=next_state();
		if(lt1=="primary"&&lt2=="key")
		{
			lt3=match("* )");
			if(lt3=="")
			{
				printf("Syntax error near primary key!\n");
				return ;
			}
			lt=match(") ;");
			if(lt=="")
			{
				printf("Syntax error near primary key!\n");
				return ;
			}
			for(int i=0;i<t.attributes.size();i++)
				if(t.attributes[i].name==lt3)
				{
					t.attributes[i].isPrimaryKey=1;
					t.attributes[i].isUnique = 1;
					break;
				}
			break;
		}
		else if(lt3==",")
		{
			attr(lt1,lt2,0,"",attribute);
			t.attributes.push_back(attribute);
		}
		else if(lt2=="char"&&lt3=="(")
		{
			string length="";
			lt3=match("* )");
			if(lt3!="")
				length=lt3;
			else 
			{
				printf("Syntax error near %s %s %s!\n",lt1.c_str(),lt2.c_str(),lt3.c_str());
				return ;
			}
			lt3=next_state();
			if(lt3=="unique")
			{
				lt3=next_state();
				if(lt3==",")
				{
					attr(lt1,lt2,1,length,attribute);
					t.attributes.push_back(attribute);
				}
				else 
				{
					printf("Syntax error near %s %s %s!\n",lt1.c_str(),lt2.c_str(),lt3.c_str());
					return ;
				}
			}
			else if(lt3==",")
			{
				attr(lt1,lt2,0,length,attribute);
				t.attributes.push_back(attribute);
			}
			else 
			{
				printf("Syntax error near %s %s %s!\n",lt1.c_str(),lt2.c_str(),lt3.c_str());
				return ;
			}
		}
		else 
		{
			if(lt3=="unique")
			{
				attr(lt1,lt2,1,"",attribute);
				t.attributes.push_back(attribute);
			}
			else 
			{
				printf("Syntax error near %s %s %s!\n",lt1.c_str(),lt2.c_str(),lt3.c_str());
				return ;
			}
			lt3=next_state();
			if(lt3!=",")
			{
				printf("Syntax error near %s %s %s!\n",lt1.c_str(),lt2.c_str(),lt3.c_str());
				return ;
			}
		}
	}
	string msg=Api.CreateTable(t);
	if (msg != "")
		printf("%s\n", msg.c_str());
	else
		printf("Create table successful\n");
}

void interpreter::drop_table()
{
	string lt1;
	lt1=match("* ;");
	if(lt1!="")
	{
		string msg=Api.DropTable(lt1);
		if(msg!="")
			printf("%s\n",msg.c_str());
		else
			printf("Drop table successful\n");
	}
	else 
	{
		printf("Syntax error!\n");
		return ;
	}
}
void interpreter::create_index()
{
	string inname,tabname,arrname;
	inname=match("* on");
	tabname=match("* (");
	arrname=match("* ) ;");
	if(inname==""||tabname==""||arrname=="")
	{
		printf("Syntax error!\n");
		return ;
	}
	else 
	{
		string msg=Api.CreateIndex(inname,tabname,arrname);
		if(msg!="")
			printf("%s\n",msg.c_str());
		else
			printf("Create index successful\n");
	}
}
void interpreter::drop_index()
{
	string inname;
	inname=match("* ;");
	if(inname!="")
	{
		string msg=Api.DropIndex(inname);
		if (msg != "")
			printf("%s\n", msg.c_str());
		else
			printf("Drop index successful\n");
	}
	else 
	{
		printf("Syntax error!\n");
		return ;
	}
}

void interpreter::select()
{
	vector<Condition> v;
	string lt,tabname,lt1,lt2,lt3;
	Condition condition;
	lt=match("* from");
	if(lt!="*")
 	{
		printf("Syntax error!(need * before from)\n");
		return ;
	}
	tabname=next_state();
	lt=next_state();
	if(tabname=="")
 	{
		printf("Syntax error!(need table name before where)\n");
		return ;
	}
	if(lt=="where")
	{
		while(true)
		{
			lt1=next_state();
			if(lt1.find("<>")!=lt.npos)
			{
				lt2=lt1.substr(lt1.find("<>"),lt1.length()-lt1.find("<>"));
				lt1=lt1.substr(0,lt1.find("<>"));
			}
			else if(lt1.find("<=")!=lt.npos)
			{
				lt2=lt1.substr(lt1.find("<="),lt1.length()-lt1.find("<>"));
				lt1=lt1.substr(0,lt1.find("<="));
			}
			else if(lt1.find(">=")!=lt.npos)
			{
				lt2=lt1.substr(lt1.find(">="),lt1.length()-lt1.find("<>"));
				lt1=lt1.substr(0,lt1.find(">="));
			}
			else if(lt1.find("=")!=lt.npos)
			{
				lt2=lt1.substr(lt1.find("="),lt1.length()-lt1.find("<>"));
				lt1=lt1.substr(0,lt1.find("="));
			}
			else if(lt1.find(">")!=lt.npos)
			{
				lt2=lt1.substr(lt1.find(">"),lt1.length()-lt1.find("<>"));
				lt1=lt1.substr(0,lt1.find(">"));
			}
			else if(lt1.find("<")!=lt.npos)
			{
				lt2=lt1.substr(lt1.find("<"),lt1.length()-lt1.find("<>"));
				lt1=lt1.substr(0,lt1.find("<"));
			}
			else 
				lt2=next_state();
			if(lt2!="="&&lt2!="<>"&&lt2!="<"&&lt2!=">"&&lt2!="<="&&lt2!=">=")
			{
				if(lt2[0]=='=')
				{
					lt3=lt2.substr(1,lt2.length()-1);
					lt2=lt2.substr(0,1);
				}
				else if((lt2[0]=='<'||lt2[0]=='>')&&lt2[1]=='=')
				{
					lt3=lt2.substr(2,lt2.length()-2);
					lt2=lt2.substr(0,2);
				}
				else if((lt2[0]=='<'&&lt2[1]=='>'))
				{
					lt3=lt2.substr(2,lt2.length()-2);
					lt2=lt2.substr(0,2);
				}
				else if(lt2[0]=='<'||lt2[0]=='>')
				{
					lt3=lt2.substr(1,lt2.length()-1);
					lt2=lt2.substr(0,1);
				}
				else 
					lt3=next_state();
			}
			else 
				lt3=next_state();
			if(lt1==""||lt2==""||lt3=="")
			{
				printf("Syntax error!near %s %s %s\n",lt1.c_str(),lt2.c_str(),lt3.c_str());
				return ;
			}
			if(lt2!="="&&lt2!="<>"&&lt2!="<"&&lt2!=">"&&lt2!="<="&&lt2!=">=")
			{
				printf("Syntax error!%s must be one of '=	<>	<	>	<=	>='\n",lt2.c_str());
				return ;
				
			}
			Cond(lt1,lt2,lt3,condition);
			v.push_back(condition);
			lt=next_state();
			if(lt==";")
				break;
			else if(lt!="and")
			{
				printf("Syntax error!%s must be and\n",lt.c_str());
				return ;
			}
		}
	}
	else if(lt!=";")
	{
		printf("Syntax error!(after %s)\n",tabname.c_str());
		return ;
	}
	vector<string> vt;
	vt.push_back("*");
	string msg = Api.Select(tabname, vt, v);
	if(msg!="")
		printf("%s\n",msg.c_str());
}

void interpreter::insert()
{
	
	vector<string> v;
	int cnt=0;
	string lt,tabname;
	tabname=match("into * values (");
	if(tabname=="")
 	{
		printf("Syntax error!(need table name before where)\n");
		return ;
	}
	while(true)
	{
		lt=next_state();
		if(lt=="")
		{
			printf("Syntax error!value can't be null\n");
			return ;
		}
		v.push_back(lt);
		lt=next_state();
		if(lt==")")
			break;
		else if(lt!=",")
		{
			printf("Syntax error! can't recognize %s\n",lt.c_str());
			return ;
		}
	}
	lt=next_state();
	if(lt!=";")
	{
		printf("Syntax error! can't recognize %s\n",lt.c_str());
		return ;
	}
	string msg=Api.Insert(tabname,v);
	if(msg!="")
		printf("%s\n",msg.c_str());
	else
		printf("Insert successful\n");
}
void interpreter::del()
{
	vector<Condition> v;
	string lt,tabname,lt1,lt2,lt3;
	Condition condition;
	tabname=match("from *");
	if(tabname=="")
	{
		printf("Syntax error! table name can't be null");
		return ;
	}
	lt=next_state();
	if(lt=="where")
	{
		while(true)
		{
			lt1=next_state();
			if(lt1.find("<>")!=lt.npos)
			{
				lt2=lt1.substr(lt1.find("<>"),lt1.length()-lt1.find("<>")-2);
				lt1=lt1.substr(0,lt1.find("<>"));
			}
			else if(lt1.find("<=")!=lt.npos)
			{
				lt2=lt1.substr(lt1.find("<="),lt1.length()-lt1.find("<>")-2);
				lt1=lt1.substr(0,lt1.find("<="));
			}
			else if(lt1.find(">=")!=lt.npos)
			{
				lt2=lt1.substr(lt1.find(">="),lt1.length()-lt1.find("<>")-2);
				lt1=lt1.substr(0,lt1.find(">="));
			}
			else if(lt1.find("=")!=lt.npos)
			{
				lt2=lt1.substr(lt1.find("="),lt1.length()-lt1.find("<>")-1);
				lt1=lt1.substr(0,lt1.find("="));
			}
			else if(lt1.find(">")!=lt.npos)
			{
				lt2=lt1.substr(lt1.find(">"),lt1.length()-lt1.find("<>")-1);
				lt1=lt1.substr(0,lt1.find(">"));
			}
			else if(lt1.find("<")!=lt.npos)
			{
				lt2=lt1.substr(lt1.find("<"),lt1.length()-lt1.find("<>")-1);
				lt1=lt1.substr(0,lt1.find("<"));
			}
			else 
				lt2=next_state();
			if(lt2!="="&&lt2!="<>"&&lt2!="<"&&lt2!=">"&&lt2!="<="&&lt2!=">=")
			{
				if(lt2[0]=='=')
				{
					lt3=lt2.substr(1,lt2.length()-1);
					lt2=lt2.substr(0,1);
				}
				else if((lt2[0]=='<'||lt2[0]=='>')&&lt2[1]=='=')
				{
					lt3=lt2.substr(2,lt2.length()-2);
					lt2=lt2.substr(0,2);
				}
				else if((lt2[0]=='<'&&lt2[1]=='>'))
				{
					lt3=lt2.substr(2,lt2.length()-2);
					lt2=lt2.substr(0,2);
				}
				else if(lt2[0]=='<'||lt2[0]=='>')
				{
					lt3=lt2.substr(1,lt2.length()-1);
					lt2=lt2.substr(0,1);
				}
				else 
					lt3=next_state();
			}
			else 
				lt3=next_state();
			if(lt1==""||lt2==""||lt3=="")
			{
				printf("Syntax error!near %s %s %s\n",lt1.c_str(),lt2.c_str(),lt3.c_str());
				return ;
			}
			if(lt2!="="&&lt2!="<>"&&lt2!="<"&&lt2!=">"&&lt2!="<="&&lt2!=">=")
			{
				printf("Syntax error!%s must be one of '=	<>	<	>	<=	>='\n",lt2.c_str());
				return ;
				
			}
			Cond(lt1,lt2,lt3,condition);
			v.push_back(condition);
			lt=next_state();
			if(lt==";")
				break;
			else if(lt!="and")
			{
				printf("Syntax error!%s must be and\n",lt.c_str());
				return ;
			}
		}
	}
	else if(lt!=";")
	{
		printf("Syntax error!(after %s)\n",tabname.c_str());
		return ;
	}
	string msg=Api.Del(tabname,v);
	if(msg!="")
		printf("%s\n",msg.c_str());
	else
		printf("Delete successful\n");
}

void interpreter::exec()
{
	string filename=match("* ;");
	if(filename=="")
	{
		printf("Syntax error!file name can't be null\n");
		return ;
	}
	interpreter_begin(filename);
}

void interpreter::process()
{
	string ty=next_state();
	if(ty=="quit")
		exit(0);
	else if(ty=="create")
	{
		ty=next_state();
		if(ty=="table")
			create_table();
		else if(ty=="index")
			create_index();
		else
			printf("unrecogenized command\n");
	}
	else if(ty=="drop")
	{
		ty=next_state();
		if(ty=="table")
			drop_table();
		else if(ty=="index")
			drop_index();
		else
			printf("unrecogenized command\n");
	}
	else if(ty=="select")
		select();
	else if(ty=="insert")
		insert();
	else if(ty=="delete")
		del();
	else if(ty=="execfile")
		exec();
	else
		printf("unrecogenized command\n");
}

void interpreter::interpreter_begin(string filename)
{
	if(filename!="")
	{
		freopen(filename.c_str(),"r",stdin);
	}
	char nt = 0;
	while(scanf("%c", &nt)!=EOF)
	{
		string command="";
		do
		{
			if(nt=='\n')
				command+=" ";
			else if(nt=='(')
				command+=" ( ";
			else if(nt==')')
				command+=" ) ";
			else if(nt==',')
				command+=" , ";
			else if(nt==';')
			{
				command+=" ; ";
				break;
			}
			else if(nt=='	')
				command+=" ";
			else 
				command+=nt;
		} while (scanf("%c", &nt) != EOF);
		if (command != " " && command!="")
		{
			set_order(command);
			process();
		}
	}
	if(filename!="")
	{
		fclose(stdin);
		freopen("Con", "r", stdin);
	}
}
