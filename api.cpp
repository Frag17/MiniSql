#include"StdAfx.h"

extern IndexManager Ind;
extern RecordManager Rec;
extern CatalogManager Cat;
string api::CreateTable(Table t)
{
	t.blockNum=0;
	t.recordSize=1;
	
	for(int i=0;i<t.attributes.size();i++)
	{
		for (int j = 0; j < i; j++)
			if (t.attributes[i].name == t.attributes[j].name)
				return "Attributes with the same name";
		if(t.attributes[i].type==0)
			return "Unrecognized data type！！";
		if(t.attributes[i].type==1)
		    t.recordSize+=t.attributes[i].length;
  		else 
		{
			t.attributes[i].length=11;
			t.recordSize+=t.attributes[i].length;
		}
	}
	for (int i = 0; i < Cat.tables.size(); i++)
		if (Cat.tables[i]->tableName == t.tableName)
			return "Table " + t.tableName + " already exists!!";
 	Cat.createTable(t);
 	Rec.createTable(t);
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
			//Buf.dropFile((IndexName + ".index"));			不需要，index自己会删掉
			Rec.dropTable(*Cat.tables[i]);
			Cat.dropTable(name);
			return ""; 
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
					Index lt;
					Cat.tables[i]->attributes[j].hasIndex=1;
					Cat.tables[i]->attributes[j].indexName=inname;
					lt.indexName=inname;
					lt.tableName=tabname;
					lt.attribute = Cat.tables[i]->attributes[j];
					lt.blockNum = 0;
					Cat.createIndex(lt);
					Ind.createIndex(inname, Cat.tables[i]->attributes[j].type);
					vector <int>Recpoint;
					SingleData Recattr;
					Rec.selectAttribute(*Cat.tables[i],j,Recattr,Recpoint);
					for(int k=0;k<Recattr.size();k++)
						Ind.insertRecord(inname,Recattr[k],Recpoint[k]);
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
			std::string tableName;
			Attribute attribute;
			for (int j = 0; j < Cat.tables.size(); j++)
			{

				if (Cat.tables[j]->tableName == Cat.indexs[i]->tableName)
				{
					for (int k = 0; k < Cat.tables[j]->attributes.size(); k++)
						if (Cat.tables[j]->attributes[k].indexName == inname)
						{
							Cat.tables[j]->attributes[k].hasIndex = 0;
							Cat.tables[j]->attributes[k].indexName = "";
						}
				}
			}
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
	if (s.find('\t') != -1)return "'" + s + "'Illegal character'\t'";
	switch (type) {
	case CHAR:
		if (s[0] != '\'' || s[s.length() - 1] != '\'')return "string" + s + "dose not match type char";
		s = s.substr(1, s.length() - 2);
		break;
	case INT:
		for (int i = 0; i < s.length(); i++) {
			if ((s[i]<'0' || s[i]>'9') && s[i] != ' ')return "'" + s + "'dose not match type char";
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
			if ((s[i]<'0' || s[i]>'9') && s[i] != '.' && s[i] != ' ')return "'" + s + "'dose not match type int";
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
		int type = pa->type;
		string s = *pt;
		if (s.find('\t') != -1)return "'" + s + "'exits illegal character'\t'";
		int length = pa->length;
		string t = singleCheck(s, type);
		if (t != "")return t;
		if (type == CHAR) {
			if (s.length() > length)return "string'" + s + "'is too long";
			while (s.length() < length)s += " ";
		}
		New.push_back(s);
	}
	if (pt != Old.end())return "too many arguments to insert";
	if (pa != attr.end())return "too few arguments to insert";
	return "";
}

string api::nameCheck(string name) {
	for (int i = 0; i < name.length(); i++) {
	if ((name[i] >= '0'&&name[i] <= '9') || (name[i] <= 'Z'&&name[i] >= 'A') || (name[i] <= 'z'&&name[i] >= 'a') || name[i] == '_')continue;
		return "name'" + name + "' exits illegal character";
	}
	if (name == "select" || name == "from" || name == "where" || name == "create" || name == "table" || name == "unique" || name == "insert" || name == "into" || name == "delete" || name == "index" || name == "on")
		return name + "is key words of database";
	return "";
}

int api::datacmp(const string& Lstr, const string& Rstr, const int& type) {
	int l, r;
	float fll, flr;
	switch (type) {
	case CHAR:
		if (Lstr < Rstr)return -1;
		if (Lstr == Rstr)return 0;
		return 1;
	case INT:
		l = atoi(Lstr.c_str());
		r = atoi(Rstr.c_str());
		if (l < r)return -1;
		if (l == r)return 0;
		return 1;
	case FLOAT:
		fll = atof(Lstr.c_str());
		flr = atof(Rstr.c_str());
		if (fll < flr)return -1;
		if (fll == flr)return 0;
		return 1;
	default:
		return -2;
	}
}

string api::Insert(string tableName, Tuple& tup) {
	if (Cat.existTable(tableName) == false)return "Insert failure：can not find table " + tableName;
	Table* tab = *Cat.findTable(tableName);
	/*if (tab == nullptr) {
	cout << "Insert failure：can not find table+ tableName;
	return;
	}*/
	Tuple ne;
	string s = typeCheck(tup, tab->attributes, ne);
	if (s != "") {
		return "Insert failure：" + s;

	}
	vector<Attribute>* pattr = &tab->attributes;
	vector<IndexInfo> hasindex;
	vector<AttributeInfo> isattr;
	for (int i = 0; i < pattr->size(); i++) {
		if ((*pattr)[i].isUnique) {
			AttributeInfo att;
			att.attribute = (*pattr)[i];
			att.attributeOrder = i;
			if ((*pattr)[i].hasIndex) {			//有index的unique，用index查询重复
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
	for (vector<IndexInfo>::iterator pin = hasindex.begin(); pin != hasindex.end(); pin++) {
		BPlusTree bpt = Ind.openIndex(pin->IndexName);
		int i = pin->attribute.attributeOrder;
		if (bpt.find(ne[i]) != -1) {
			return "Insert failure，Repeated attributes value " + pin->attribute.attribute.name +"   "+ ne[i] ;
		}
	}
	if (!isattr.empty()) {			//没有index的unique,遍历查询值重复
		SingleData t;
		for (vector<AttributeInfo>::iterator pat = isattr.begin(); pat != isattr.end(); pat++) {
			Rec.selectAttribute(*tab, pat->attributeOrder, t, vector<int>());
			int i = pat->attributeOrder;
			for (vector<string>::iterator pstr = t.begin(); pstr != t.end(); pstr++) {
				if ((*pstr) == ne[i]) {
					return "Insert failure，Repeated attributes value " + pat->attribute.name +"   "+ ne[i];
				}
			}
		}
	}
	int ptr = Rec.insertRecord(*tab, ne);
	for (vector<IndexInfo>::iterator pin = hasindex.begin(); pin != hasindex.end(); pin++) {
		BPlusTree bpt = Ind.openIndex(pin->IndexName);
		bpt.insertRecord(ne[pin->attribute.attributeOrder], ptr);
	}
	return "";
}

string api::Select(string tableName, vector<string>& attributes, vector<Condition>& conditions) {
	if (Cat.existTable(tableName) == false)return "select failure：can not     find table" + tableName;
	Table* tab = *Cat.findTable(tableName);
	vector<AttributeInfo> outputAttr;
	/*保存需要输出的列的信息*/
	if (attributes[0] == "*") {	//select*
		int i = 0;
		for (vector<Attribute>::iterator pat = tab->attributes.begin(); pat != tab->attributes.end(); pat++, i++) {
			AttributeInfo t;
			t.attribute = *pat;
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
					t.attribute = *pat;
					t.attributeOrder = i;
					outputAttr.push_back(t);
					break;
				}
			}
			if (pat == tab->attributes.end())return "select failure，can not find attribute" + *ptb;
		}
	}

	struct ConditionWithIndex {
		string indxnam;
		int type;
		Op op;
		string value;
	};
	vector<ConditionWithIndex> CwIs;

	vector<Condition>::iterator itc = conditions.begin();
	while (itc != conditions.end()) {
		vector<Attribute>::iterator pat;
		int i;
		for (pat = tab->attributes.begin(), i = 0; pat != tab->attributes.end(); pat++, i++) {
			if (pat->name == itc->attributeName)break;
		}
		if (pat == tab->attributes.end())return "select failure，can not find attribute" + itc->attributeName;
		if (pat->hasIndex) {			//分类，分为有index的条件查询与无index的条件查询
			ConditionWithIndex t;
			t.indxnam = pat->indexName;
			t.type = pat->type;
			t.op = itc->op;
			t.value = itc->value;
			string st = singleCheck(t.value, t.type);
			if (st != "")return "select failure，" + st;
			CwIs.push_back(t);

			itc = conditions.erase(itc);
		}
		else {
			itc->attributeOrder = i;
			itc->type = pat->type;
			string st = singleCheck(itc->value, itc->type);
			if (st != "")return "select failure，" + st;
			itc++;
		}
	}

	/*查询条件有index的，优先用index*/
	Data dat;
	if (CwIs.size() == 0) {
		Rec.selectRecord(*tab, dat);
	}
	else {
		vector<int> resPtrs, tPtrs;
		int ptr;
		vector<ConditionWithIndex>::iterator itcwi = CwIs.begin();	//先查询一个，然后查询后面的并做交集
		string indexName = itcwi->indxnam;
		switch (itcwi->op) {
		case EQ:
			ptr = Ind.find(indexName, itcwi->value);
			if (ptr != NOT_EXIST)
				resPtrs.push_back(ptr);
			break;
		case NE:
			Ind.findBetween(indexName, (string)"", (string)"", resPtrs);
			ptr = Ind.find(indexName, itcwi->value);
			if (ptr != -1) {
				for (vector<int>::iterator itr = resPtrs.begin(); itr != resPtrs.end(); itr++) {
					if (*itr == ptr) {
						resPtrs.erase(itr);
						break;
					}
				}
			}
			break;
		case GT:
			Ind.findBetween(indexName, itcwi->value, (string)"", resPtrs);
			break;
		case LT:
			Ind.findBetween(indexName, (string)"", itcwi->value, resPtrs);
			break;
		case GE:
			ptr = Ind.find(indexName, itcwi->value);
			if (ptr != -1) {
				resPtrs.push_back(ptr);
			}
			Ind.findBetween(indexName, itcwi->value, (string)"", resPtrs);
			break;
		case LE:
			Ind.findBetween(indexName, (string)"", itcwi->value, resPtrs);
			ptr = Ind.find(indexName, itcwi->value);
			if (ptr != -1) {
				resPtrs.push_back(ptr);
			}
			break;
		}

		for (; itcwi != CwIs.end(); itcwi++) {
			indexName = itcwi->indxnam;
			switch (itcwi->op) {
			case EQ:
				ptr = Ind.find(indexName, itcwi->value);
				if (ptr != NOT_EXIST)
					tPtrs.push_back(ptr);
				break;
			case NE:
				Ind.findBetween(indexName, (string)"", (string)"", tPtrs);
				ptr = Ind.find(indexName, itcwi->value);
				if (ptr != -1) {
					for (vector<int>::iterator itr = tPtrs.begin(); itr != tPtrs.end(); itr++) {
						if (*itr == ptr) {
							tPtrs.erase(itr);
							break;
						}
					}
				}
				break;
			case GT:
				Ind.findBetween(indexName, itcwi->value, (string)"", tPtrs);
				break;
			case LT:
				Ind.findBetween(indexName, (string)"", itcwi->value, tPtrs);
				break;
			case GE:
				ptr = Ind.find(indexName, itcwi->value);
				if (ptr != -1) {
					tPtrs.push_back(ptr);
				}
				Ind.findBetween(indexName, itcwi->value, (string)"", tPtrs);
				break;
			case LE:
				Ind.findBetween(indexName, (string)"", itcwi->value, tPtrs);
				ptr = Ind.find(indexName, itcwi->value);
				if (ptr != -1) {
					tPtrs.push_back(ptr);
				}
				break;
			}

			/*合并两个指针，做交集*/
			for (vector<int>::iterator itr = resPtrs.begin(); itr != resPtrs.end();) {
				bool include = false;
				for (vector<int>::iterator itt = tPtrs.begin(); itt != tPtrs.end();) {
					if (*itt == *itr) {
						itt = tPtrs.erase(itt);
						include = true;
						break;
					}
					else {
						itt++;
					}
				}
				if (include) {
					itr++;
				}
				else {
					itr = resPtrs.erase(itr);
				}
			}
		}
		/*得到data*/
		if (resPtrs.size() != 0)
			Rec.selectByPointer(*tab, resPtrs, dat);
	}

	/*无index的条件*/
	for (Data::iterator itdat = dat.begin(); itdat != dat.end(); ) {
		for (itc = conditions.begin(); itc != conditions.end(); itc++) {
			int order = itc->attributeOrder;
			int cmp = datacmp((*itdat)[order], itc->value, itc->type);
			if (itc->op < GT) {
				if (itc->op == EQ) {
					if (cmp != 0) {
						itdat = dat.erase(itdat);
						break;
					}
				}
				else if (itc->op == NE) {
					if (cmp == 0) {
						itdat = dat.erase(itdat);
						break;
					}
				}
				else {
					if (cmp != -1) {
						itdat = dat.erase(itdat);
						break;
					}
				}
			}
			else {
				if (itc->op == GT) {
					if (cmp != 1) {
						itdat = dat.erase(itdat);
						break;
					}
				}
				else if (itc->op == LE) {
					if (cmp == 1) {
						itdat = dat.erase(itdat);
						break;
					}
				}
				else {
					if (cmp == -1) {
						itdat = dat.erase(itdat);
						break;
					}
				}

			}
		}
		if (itc == conditions.end())itdat++;
	}

	/*select完成， 输出*/
	for (vector<AttributeInfo>::iterator itattr = outputAttr.begin(); itattr != outputAttr.end();) {
		stringstream ss;
		ss.clear();
		ss << "%";
		ss << itattr->attribute.length;
		ss << "s";
		string t;
		ss >> t;
		printf(t.c_str(), itattr->attribute.name.c_str());
		itattr++;
		if (itattr != outputAttr.end())
			printf("|");
	}
	printf("\n");
	for (vector<AttributeInfo>::iterator itattr = outputAttr.begin(); itattr != outputAttr.end();itattr++) {
		for (int i = 0; i < itattr->attribute.length; i++)printf("-");
		if (itattr != outputAttr.end())
			printf("|");
	}
	printf("\n");
	for (Data::iterator itdat = dat.begin(); itdat != dat.end(); itdat++) {
		for (vector<AttributeInfo>::iterator itattr = outputAttr.begin(); itattr != outputAttr.end();itattr++) {
			printf("%s", (*itdat)[itattr->attributeOrder].c_str());
			if (itattr != outputAttr.end())
				printf("|");
		}
		printf("\n");
	}
	return "";
}

string api::Del(string tableName, vector<Condition>& conditions){
	if (Cat.existTable(tableName) == false)return "Delete failure：can not find table" + tableName;
	Table* tab = *Cat.findTable(tableName);
	
	int primarykeyOrder;
	vector<Condition>::iterator itc = conditions.begin();
	int i;
	vector<Attribute>::iterator pat;
	while (itc != conditions.end()) {
		for (pat = tab->attributes.begin(), i = 0; pat != tab->attributes.end(); pat++, i++) {
			if (pat->name == itc->attributeName)break;
		}
		if (pat == tab->attributes.end())return "Delete failure：can not find attrbute" + itc->attributeName;
		itc->attributeOrder = i;
		itc->type = pat->type;
		string st = singleCheck(itc->value, itc->type);
		if (st != "")return "Delete failure，" + st;
		itc++;
	}

	vector<IndexInfo> Ind2Del; 
	for (pat = tab->attributes.begin(), i = 0; pat != tab->attributes.end(); pat++, i++) {
		if(pat->hasIndex){
			IndexInfo tind;
			tind.IndexName=pat->indexName;
			tind.attribute.attribute=*pat;
			tind.attribute.attributeOrder=i;
			Ind2Del.push_back(tind);
		}
		if (pat->isPrimaryKey == true) {
			primarykeyOrder = i;
		}
	}
	
	int n;
	vector<string> KeytoDel;
	SingleData dat;
	SingleData* dat2Del=new SingleData[Ind2Del.size()];
	Data data;

	vector<IndexInfo>::iterator itind;
	Rec.selectRecord(*tab, data);
	for(vector<Tuple>::iterator itd=data.begin();itd!=data.end();itd++){
		bool ifDel = true;
		for(vector<Condition>::iterator itc=conditions.begin();itc!=conditions.end();itc++){
			int cmp = datacmp((*itd)[itc->attributeOrder], itc->value, itc->type);
			if (itc->op < GT) {
				if (itc->op == EQ) {
					if (cmp != 0) {
						ifDel = false;
						break;
					}
				}
				else if (itc->op == NE) {
					if (cmp == 0) {
						ifDel = false;
						break;
					}
				}
				else {
					if (cmp != -1) {
						ifDel = false;
						break;
					}
				}
			}
			else {
				if (itc->op == GT) {
					if (cmp != 1) {
						ifDel = false;
						break;
					}
				}
				else if (itc->op == LE) {
					if (cmp == 1) {
						ifDel = false;
						break;
					}
				}
				else {
					if (cmp == -1) {
						ifDel = false;
						break;
					}
				}
			}
		}

		if (ifDel) {
			dat.push_back((*itd)[primarykeyOrder]);
			for (itind = Ind2Del.begin(), i = 0; itind != Ind2Del.end(); itind++, i++) {
				dat2Del[i].push_back((*itd)[itind->attribute.attributeOrder]);
			}
		}
	}
	
	n = dat.size();
	if (n != 0) {
		Rec.deleteByPrimaryKey(*tab, dat);
		for (itind = Ind2Del.begin(), i = 0; itind != Ind2Del.end(); itind++, i++) {
			for (vector<string>::iterator itstr = dat2Del[i].begin(); itstr != dat2Del[i].end(); itstr++) {
				Ind.deleteRecord(itind->IndexName, *itstr);
			}
		}
	}

	delete[] dat2Del;
	stringstream ss;                                                           
	ss<<n;                                                            
	string t;
	ss>>t;
	t+="rows have be changes";
	t="delete successful，"+t; 
	return t;
}