#include<api.h>
#include"CatalogManager.h"
using std::string;
extern IndexManager Ind;
extern RecordManager Rec;
extern CatalogManager Cat;
string api::CreateTable(table t)
{
	for(int i=0;i<t.attributes.size();i++)
		if(t.attributes[i].type==0)
			return "Unrecognized data type����";
	for(int i=0;i<Cat.tables.size();i++)
		if(Cat.tables[i].tableName==t.tableName)
			return "Table "+t.tableName+" already exists!!"
 	Cat.createTable(&t);
 	Rec.createTable(t.tableName) ;
 	for(int i=0;i<t.attributes.size();i++)
		if(t.attributes[i].isPrimaryKey==1)
			return CreateIndex(t.tableName+"_"+t.attributes[i].name,t.tableName,t.attributes[i].name);
}
string api:DropTable(string name)
{
	for(int i=0;i<Cat.tables.size();i++)
		if(Cat.tables[i].tableName==name)
		{
			for(int j=0;j<Cat.indexs.size();j++)
				if(Cat.indexs[j].tableName==name)
					Ind.dropIndex(Cat.indexs[j].indexName);
			Buf.dropFile((IndexName + ".index"));
			Rec.dropTable(name);
			Cat.dropTable(name);
			return 0; 
		}
	return "Table doesn't exist";
}
string api::CreateIndex(string inname,string tabname,string arrname)
{
	bool find=0;
	for(int i=0;i<Cat.tables.size();i++)
		if(Cat.tables[i].tableName==tabname)
			find=1;
	if(find==0)
		return "Table doesn't exist";
	for(int i=0;i<Cat.indexs.size();i++)
		if(Cat.indexs[i].indexName==inname)
			return "Index "+inname+" already exists!!"
	for(int i=0;i<Cat.tables.size();i++)
		if(Cat.tables[i].tableName==tabname)
		{
			for(int j=0;j<Cat.tables[i].attributes.size();j++)
				if(Cat.tables[i].attributes[j].name==arrname)
				{
					if(Cat.tables[i].attributes[j].isUnique==0)
						return "Attribute "+arrname+" is not unique!!";
					if(Cat.tables[i].attributes[j].hasIndex==1)
						return "Attribute "+arrname+" already has index!!";
					Index lt;std::string indexName;
					Cat.tables[i].attributes[j].hasIndex=1;
					Cat.tables[i].attributes[j].indexName=inname;
					lt.indexName=inname;
					lt.tableName=tabname;
					lt.attribute=arrname;
					Cat.createIndex(&lt);
					Rec.createIndex(inname,tabname,arrname,Cat.tables[i].attributes[j].type);
					//call void selectAttribute(Table& table, int attributeOrder, Data& data);	 
					//and call bool insertRecord(string IndexName, string key, int ptr) 
					return "";
				}
		}
	return "Attribute "+arrname+" doesn't exist";
}


string api::DropIndex(string inname)
{
	for(int i=0;i<Cat.indexs.size();i++)
		if(Cat.indexs[i].indexName==inname)
		{
			dropIndex(inname);
			Cat.dropIndex(inname);
			return "";
		}
	return "Index "+inname+" dosen't exist!!";
}

string api::typeCheck(Tuple& Old,vector<Attribute>& attr, Tuple& New) {
	vector<string>::iterator pt;
	vector<Attribute>::iterator pa;
	stringstream ss;
	for (pt = Old.begin(), pa = attr.begin(); pt != Old.end() && pa != attr.end(); pt++, pa++) {
		char a[5];
		int type = pa->type, in;
		float fl;
		string s = *pt;
		if (s.find('\t') != -1)return "'" + s + "'���ڷǷ��ַ�'\t'";
		int length = pa->length;
		switch (type) {
		case CHAR:
			if (s[0] != '\'' || s[s.length() - 1] != '\'')return "�ַ���" + s + "��char���Ͳ�ƥ��";
			s = s.substr(1, s.length());
			if (s.length() > length)return "�ַ���'" + s + "'��������";
			for (int i = s.length(); i < length; i++)s += " ";
			break;
		case INT:
			if ((in = atoi(s.c_str())) == 0) {
				for (int i = 0; i < s.length(); i++) {
					if (s[i] != '0' && s[i] != ' ')return "'" + s + "'��int���Ͳ���";
				}
			}
			ss.clear();
			ss.fill(' ');
			ss << in;
			ss >> s;
			while (s.length() < 11)s = " " + s;
			New.push_back(s);
			break;
		case FLOAT:
			if ((fl = atof(s.c_str())) == 0) {
				for (int i = 0; i < s.length(); i++) {
					if (s[i] != '0' && s[i] != '.' && s[i] != ' ')return "'" + s + "'��int���Ͳ���";
				}
			}
			ss.clear();
			ss << fl;
			ss >> s;
			while (s.length() < 11)s = " " + s;
			if (s.length() > 11)s = s.substr(0, 11);
			New.push_back(s);
			break;
		}
	}
	if (pt != Old.end())return "�������ݹ���";
	if (pa != attr.end())return "�������ݹ���";
	return "";
}
string api::nameCheck(string name) {
	for (int i = 0; i < name.length(); i++) {
	if ((name[i] >= '0'&&name[i] <= '9') || (name[i] <= 'Z'&&name[i] >= 'A') || (name[i] <= 'z'&&name[i] >= 'a') || name[i] == '_')continue;
		return "����'" + name + "'���ڷǷ��ַ�";
	}
	if (name == "select" || name == "from" || name == "where" || name == "create" || name == "table" || name == "unique" || name == "insert" || name == "into" || name == "delete" || name == "index" || name == "on")
		return name + "Ϊ���ݿⱣ����";
	return "";
}
void api::insert(string tableName, Tuple& tup) {
	Table* tab = *Cat.findTable(tableName);
	/*if (tab == nullptr) {
		cout << "����ʧ�ܣ��Ҳ�����" + tableName;
		return;
	}*/
	Tuple ne;
	string s = typeCheck(tup, tab->attributes, ne);
	if (s != "") {
		cout << "����ʧ�ܣ�" + s;
		return;
	}
	vector<Attribute>* pattr = &tab->attributes;
	vector<IndexInfo> hasindex;
	vector<AttributeInfo> isattr;
	for (int i = 0; i < pattr->size(); i++) {
		if ((*pattr)[i].isUnique) {
			AttributeInfo att;
			att.attribute = (*pattr)[i].name;
			att.attributeOrder = i;
			if ((*pattr)[i].hasIndex) {			//��index��unique����index��ѯ�ظ�
				IndexInfo indx;
				indx.IndexName = (*pattr)[i].indexName;
				indx.attribute = att;
				hasindex.push_back(indx);
			}
			else {
				isattr.push_back(att);
			}
		}
	}
	for (vector<IndexInfo>::iterator pin = hasindex.begin(); pin != hasindex.end; pin++) {
		BPlusTree bpt = Ind.openIndex(pin->IndexName);
		int i = pin->attribute.attributeOrder;
		if (bpt.find(ne[i]) != -1) {
			cout << "����ʧ�ܣ�����" + pin->attribute.attribute + "ֵ" + ne[i] + "�ظ�";
			return;
		}
	}
	if (!isattr.empty()) {			//û��index��unique,������ѯֵ�ظ�
		Data t;
		for (vector<AttributeInfo>::iterator pat = isattr.begin(); pat != isattr.end(); pat++) {
			Rec.selectAttribute(*tab, pat->attributeOrder, t);
			int i = pat->attributeOrder;
			for (vector<Tuple>::iterator pstr = t.begin(); pstr != t.end(); pstr ++) {
				if ((*pstr)[0] == ne[i]) {
					cout << "����ʧ�ܣ�����" + pat->attribute + "ֵ" + ne[i] + "�ظ�";
					return;
				}
			}
		}
	}
	int ptr = Rec.insertRecord(*tab, ne);
	for (vector<IndexInfo>::iterator pin = hasindex.begin(); pin != hasindex.end; pin++) {
		BPlusTree bpt = Ind.openIndex(pin->IndexName);
		bpt.insertRecord(ne[pin->attribute.attributeOrder], ptr);
	}
	cout << "����ɹ�";
	return;
}

