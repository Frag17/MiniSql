#include"api.h"
#include"CatalogManager.h"
using std::string;
extern IndexManager Ind;
extern RecordManager Rec;
extern CatalogManager Cat;
string api::CreateTable(Table t)
{
	for(int i=0;i<t.attributes.size();i++)
		if(t.attributes[i].type==0)
			return "Unrecognized data type����";
	for (int i = 0; i < Cat.tables.size(); i++)
		if (Cat.tables[i]->tableName == t.tableName)
			return "Table " + t.tableName + " already exists!!";
 	Cat.createTable(&t);
 	Rec.createTable(t.tableName) ;
 	for(int i=0;i<t.attributes.size();i++)
		if(t.attributes[i].isPrimaryKey==1)
			return CreateIndex(t.tableName+"_"+t.attributes[i].name,t.tableName,t.attributes[i].name);
}

string api::DropTable(string name)
{
	for(int i=0;i<Cat.tables.size();i++)
		if(Cat.tables[i]->tableName==name)
		{
			for(int j=0;j<Cat.indexs.size();j++)
				if(Cat.indexs[j]->tableName==name)
					Ind.dropIndex(Cat.indexs[j]->indexName);
			//Buf.dropFile((IndexName + ".index"));			����Ҫ��index�Լ���ɾ��
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
		if(Cat.tables[i]->tableName==tabname)
			find=1;
	if(find==0)
		return "Table doesn't exist";
	for (int i = 0; i < Cat.indexs.size(); i++)
		if (Cat.indexs[i]->indexName == inname)
			return "Index " + inname + " already exists!!";
	for(int i=0;i<Cat.tables.size();i++)
		if(Cat.tables[i]->tableName==tabname)
		{
			for(int j=0;j<Cat.tables[i]->attributes.size();j++)
				if(Cat.tables[i]->attributes[j].name==arrname)
				{
					if(Cat.tables[i]->attributes[j].isUnique==0)
						return "Attribute "+arrname+" is not unique!!";
					if(Cat.tables[i]->attributes[j].hasIndex==1)
						return "Attribute "+arrname+" already has index!!";
					Index lt;std::string indexName;
					Cat.tables[i]->attributes[j].hasIndex=1;
					Cat.tables[i]->attributes[j].indexName=inname;
					lt.indexName=inname;
					lt.tableName=tabname;
					lt.attribute=arrname;
					Cat.createIndex(&lt);
					Ind.createIndex(inname,tabname,arrname,Cat.tables[i]->attributes[j].type);
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
		if(Cat.indexs[i]->indexName==inname)
		{
			Ind.dropIndex(inname);
			Cat.dropIndex(inname);
			return "";
		}
	return "Index "+inname+" dosen't exist!!";
}

string api::singleCheck(string& s, int type) {
	int in;
	float fl;
	stringstream ss;
	if (s.find('\t') != -1)return "'" + s + "'���ڷǷ��ַ�'\t'";
	switch (type) {
	case CHAR:
		if (s[0] != '\'' || s[s.length() - 1] != '\'')return "�ַ���" + s + "��char���Ͳ�ƥ��";
		s = s.substr(1, s.length() - 2);
		break;
	case INT:
		for (int i = 0; i < s.length(); i++) {
			if ((s[i]<'0' || s[i]>'9') && s[i] != ' ')return "'" + s + "'��int���Ͳ���";
		}
		in = atoi(s.c_str());
		ss.clear();
		ss.fill(' ');
		ss << in;
		ss >> s;
		while (s.length() < 11)s = " " + s;
		break;
	case FLOAT:
		for (int i = 0; i < s.length(); i++) {
			if ((s[i]<'0' || s[i]>'9') && s[i] != '.' && s[i] != ' ')return "'" + s + "'��int���Ͳ���";
		}
		fl = atof(s.c_str());
		ss.clear();
		ss << fl;
		ss >> s;
		while (s.length() < 11)s = " " + s;
		if (s.length() > 11)s = s.substr(0, 11);
		break;
	}

	return "";
}

string api::typeCheck(Tuple& Old,vector<Attribute>& attr, Tuple& New) {
	vector<string>::iterator pt;
	vector<Attribute>::iterator pa;
	stringstream ss;
	for (pt = Old.begin(), pa = attr.begin(); pt != Old.end() && pa != attr.end(); pt++, pa++) {
		int type = pa->type, in;
		float fl;
		string s = *pt;
		if (s.find('\t') != -1)return "'" + s + "'���ڷǷ��ַ�'\t'";
		int length = pa->length;
		string t = singleCheck(s, type);
		if (t != "")return t;
		if (type == CHAR) {
			if (s.length() > length)return "�ַ���'" + s + "'����";
			while (s.length < length)s += " ";
		}
		New.push_back(s);
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

string api::insert(string tableName, Tuple& tup) {
	if (Cat.existTable(tableName) == false)return "����ʧ�ܣ��Ҳ�����" + tableName;
	Table* tab = *Cat.findTable(tableName);
	/*if (tab == nullptr) {
		cout << "����ʧ�ܣ��Ҳ�����" + tableName;
		return;
	}*/
	Tuple ne;
	string s = typeCheck(tup, tab->attributes, ne);
	if (s != "") {
		return "����ʧ�ܣ�" + s;
		
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
			return "����ʧ�ܣ�����" + pin->attribute.attribute + "ֵ" + ne[i] + "�ظ�";
		}
	}
	if (!isattr.empty()) {			//û��index��unique,������ѯֵ�ظ�
		Data t;
		for (vector<AttributeInfo>::iterator pat = isattr.begin(); pat != isattr.end(); pat++) {
			Rec.selectAttribute(*tab, pat->attributeOrder, t);
			int i = pat->attributeOrder;
			for (vector<Tuple>::iterator pstr = t.begin(); pstr != t.end(); pstr ++) {
				if ((*pstr)[0] == ne[i]) {
					return "����ʧ�ܣ�����" + pat->attribute + "ֵ" + ne[i] + "�ظ�";
				}
			}
		}
	}
	int ptr = Rec.insertRecord(*tab, ne);
	for (vector<IndexInfo>::iterator pin = hasindex.begin(); pin != hasindex.end; pin++) {
		BPlusTree bpt = Ind.openIndex(pin->IndexName);
		bpt.insertRecord(ne[pin->attribute.attributeOrder], ptr);
	}
	return "����ɹ�";
}

string api::Select(string tableName, vector<string>& attributes, vector<Condition>& conditions) {
	if (Cat.existTable(tableName) == false)return "��ѯʧ�ܣ��Ҳ�����" + tableName;
	Table* tab = *Cat.findTable(tableName);
	vector<AttributeInfo> outputAttr;
	/*������Ҫ������е���Ϣ*/
	if (attributes[0] == "*") {										//select*
		for (vector<Attribute>::iterator pat = tab->attributes.begin(), int i = 0; pat != tab->attributes.end(); pat++, i++) {
			AttributeInfo t;
			t.attribute = pat->name;
			t.attributeOrder = i;
			outputAttr.push_back(t);
		}
	}
	else {
		for (vector<string>::iterator ptb = attributes.begin(); ptb != attributes.end(); ptb++) {
			vector<Attribute>::iterator pat;
			int i;
			for (pat = tab->attributes.begin(), i = 0; pat != tab->attributes.end(); pat++, i++) {
				if (pat->name == *ptb) {
					AttributeInfo t;
					t.attribute = pat->name;
					t.attributeOrder = i;
					outputAttr.push_back(t);
					break;
				}
			}
			if (pat == tab->attributes.end())return "��ѯʧ�ܣ��Ҳ�������" + *ptb;
		}
	}

	struct ConditionWithIndex {
		IndexInfo indx;
		int type;
		Op op;
		string value;
	};
	vector<ConditionWithIndex> CwIs;

	vector<Condition>::iterator itc = conditions.begin();
	while (itc!=conditions.end()) {
		vector<Attribute>::iterator pat;
		int i;
		for (pat = tab->attributes.begin(), i = 0; pat != tab->attributes.end(); pat++, i++) {
			if (pat->name == itc->attribute.attribute)break;
		}
		if (pat == tab->attributes.end())return "��ѯʧ�ܣ�δ�ҵ�����" + itc->attribute.attribute;
		if (pat->hasIndex) {			//���࣬��Ϊ��index��������ѯ����index��������ѯ
			ConditionWithIndex t;
			t.indx.IndexName = pat->indexName;
			t.indx.attribute = itc->attribute;
			t.type = pat->type;
			t.op = itc->op;
			t.value = itc->value;
			string st = singleCheck(t.value, t.type);
			if (st != "")return "��ѯʧ�ܣ�" + st;
			CwIs.push_back(t);

			conditions.erase(itc);
		}
		else {
			itc->attribute.attributeOrder = i;
			itc->type = pat->type;
			itc++;
		}
	}

	Data dat;
	if (CwIs.size() == 0) {
		Rec.selectRecord(*tab, dat);
	}
	else {
		int n = CwIs.size();
		vector<int> resPtrs, tPtrs;
		int ptr;
		vector<ConditionWithIndex>::iterator itcwi = CwIs.begin();
		string indexName = itcwi->indx.IndexName;
		switch (itcwi->op) {
		case EQ:ptr = Ind.find(indexName, itcwi->value);
			resPtrs.push_back(ptr);
			break;
		case GT:Ind.findBetween(indexName, itcwi->value, "", resPtrs);

		}
	}

}

