#ifndef _INDEXMANAGER_H_
#define _INDEXMANAGER_H_

#include"StdAfx.h"

#ifndef ORDER_NUMBER		//B+ tree叉数
#define ORDER_NUMBER 13
#endif

#define N ORDER_NUMBER

#define NOT_EXIST -1		//offset=-1表示不存在

extern BufferManager Buf;

class Node {				//节点接口
public:
	int selfPtr;			//自身offset
	string Keys[N - 1];		//记录
	int Ptrs[N];		//指针，实际为blockoffset
	int Parent;				//父节点，blockoffset，-1表示没有
	static int Type;		//key类型
	int num;				//节点记录数
	bool isLeaf;

	Node() {  num = 0; }
	Node(const Node& n):selfPtr(n.selfPtr),Parent(n.Parent), num(n.num), isLeaf(n.isLeaf) {
		int i;
		//Keys = new string[N - 1];
		for (i = 0; i < n.num; i++) {
			Keys[i] = n.Keys[i];
			Ptrs[i] = n.Ptrs[i];
		}
		Ptrs[i] = n.Ptrs[i];
		Ptrs[N - 1] = n.Ptrs[N - 1];
	}
	//~Node() {}

	/*将string形式存储的节点信息转换为节点*/
	Node(string str) {
		stringstream ss;
		//Keys = new string[N - 1];
		char t[256];
		ss << str;
		ss >> selfPtr;
		ss >> isLeaf;
		ss >> Parent;
		ss >> num;
		for (int i = 0; i < num; i++) {
			ss >> Ptrs[i];
			ss.get();
			ss.get(t, 256, '\t');
			Keys[i] = t;
			ss.get();
		}
		if (isLeaf) {
			ss >> Ptrs[N - 1];
		}
		else {
			ss >> Ptrs[num];
		}
	}

	/* 将节点信息格式化转换为字符串*/
	operator string() {
		stringstream ss;
		ss.clear();
		ss << selfPtr << ' ';
		ss << isLeaf << ' ';
		ss << Parent << ' ';
		ss << num << ' ';
		for (int i = 0; i < num; i++) {
			ss << Ptrs[i] << '\t';
			ss << Keys[i] << '\t';
		}

		if (isLeaf) {
			ss << Ptrs[N - 1];
		}
		else {
			ss << Ptrs[num];
		}

		string t;
		char *ch = new char[4096];
		ss.getline(ch, 4095);
		t = ch;
		delete[] ch;
		return t;
	}

	virtual int insertRecord(string Key, int Ptr) { return -1; };
	virtual int deleteRecord(int Order) { return -1; };
	
	/*找到一个指针在节点中的位置,-1表示不存在*/
	virtual int find(int Ptr) {
		int i = num;
		for (; i >= 0;i--)
			if (Ptr==Ptrs[i]){
				break;
			}

		return i;
	}

	/*找到一个记录在节点中的位置，-1表示不存在*/
	virtual int find(string Key) {
		int i = num - 1;
		for (; i >= 0; i--)
			if (Key == Keys[i])return i;
		return i;
	}

	/*检查节点是否满*/
	virtual bool isFull() {
		return num >= N - 1;
	}

	/*检查节点是否小于最小状态*/
	virtual bool isLess() {
		return num < (N - 1) / 2;
	}

	/*找到插入位置，返回序数*/
	virtual int getInsertPosition(string key) {
		if (num == 0)return 0;
		int i = 0, k;
		float f;
		switch(Node::Type){
		case CHAR:											//string
			for (; i < num&&key >= Keys[i]; i++);
			return i;
		case INT:
			k = atoi(key.c_str());
			for (; i < num; i++) {
				int t = atoi(Keys[i].c_str());
				if (k < t)return i;
			}
			return i;
		case FLOAT:
			f = atof(key.c_str());
			for (; i < num; i++) {
				float t = atof(Keys[i].c_str());
				if (f < t)return i;
			}
			return i;
		default:
			return -1;
		}
	}
};

class banch :public Node {		//非叶节点
public:
	banch():Node() { isLeaf = false; num = 0; }
	banch(const banch& b):Node() {
		selfPtr = b.selfPtr;
		Parent = b.Parent;
		num = b.num;
		isLeaf = false;
		for (int i = 0; i < num; i ++) {
			Keys[i] = b.Keys[i];
			Ptrs[i] = b.Ptrs[i];
		}
		Ptrs[num] = b.Ptrs[num];
	}
	banch(const Node& n):Node() {
		selfPtr = n.selfPtr;
		num = n.num;
		Parent = n.Parent;
		isLeaf = false;
		Ptrs[0] = n.Ptrs[0];
		for (int i = 0; i < num; i++) {
			Ptrs[i] = n.Ptrs[i];
			Keys[i] = n.Keys[i];
		}
		Ptrs[num] = n.Ptrs[num];
	}

	/*插入记录，返回插入位置*/
	int insertRecord(string key, int ptr){
		if (num == N - 1)return -2;

		int n = getInsertPosition(key);
		if (n == -1)return n;
		int i = num;
		for (; i > n; i--) {
			Keys[i] = Keys[i - 1];
			Ptrs[i + 1] = Ptrs[i];
		}
		Keys[i] = key;
		Ptrs[i + 1] = ptr;
		num++;
		return n;
	}

	/*根据指针位置删除一条记录*/
	int deleteRecord(int Order) {
		if (num <= 0)return -1;
		if (Order == 0) {
			Ptrs[0] = Ptrs[1];
			Order++;
		}
		for (int i = Order; i < num; i++) {
			Ptrs[i] = Ptrs[i + 1];
			Keys[i - 1] = Keys[i];
		}
		num--;

		return 0;
	}

};

class Leaf :public Node {			//叶节点
public:
	Leaf():Node() { isLeaf = true; num = 0; }
	Leaf(const Leaf& l):Node() {
		selfPtr = l.selfPtr;
		Parent = l.Parent;
		num = l.num;
		isLeaf = true;
		for (int i = 0; i < num; i ++ ) {
			Keys[i] = l.Keys[i];
			Ptrs[i] = l.Ptrs[i];
		}
		Ptrs[N - 1] = l.Ptrs[N - 1];
	}
	Leaf(const Node& n):Node() {
		selfPtr = n.selfPtr;
		num = n.num;
		Parent = n.Parent;
		isLeaf = true;
		for (int i = 0; i < num; i++) {
			Ptrs[i] = n.Ptrs[i];
			Keys[i] = n.Keys[i];
		}
		Ptrs[N - 1] = n.Ptrs[N - 1];
	}

	/*插入记录，返回插入位置*/
	int insertRecord(string key, int ptr) {
		if (num == N - 1)return -2;

		int n = getInsertPosition(key);
		if (n == -1)return n;
		int i = num;
		for (; i > n; i--) {
			Keys[i] = Keys[i - 1];
			Ptrs[i] = Ptrs[i - 1];
		}
		Keys[i] = key;
		Ptrs[i] = ptr;
		num++;
		return n;
	}

	/*根据指针、记录位置删除一条记录*/
	int deleteRecord(int Order) {
		for (int i = Order; i < num - 1; i++) {
			Ptrs[i] = Ptrs[i + 1];
			Keys[i] = Keys[i + 1];
		}
		num--;

		return 0;
	}

};

class BPlusTree {
private:
	int rootPtr;
	string indexName;
	/*string table;
	string attribute;*/

	/*返回整个node下的最小值*/
	string getMin(Node* nod) {
		if (nod->isLeaf) {
			if (nod->num == 0)return "";
			return nod->Keys[0];
		}

		int offset = nod->Ptrs[0];
		string t = Buf.getBlock((indexName + ".index"), offset, 0, 4);
		int size = atoi(t.c_str());
		t = Buf.getBlock((indexName	+ ".index"), offset, 5, 5 + size);
		Node node(t);
		return getMin(&node);
	}

	/*一个节点记录过少，使用相邻节点进行平衡*/
	bool balanceNode(Node* Left, Node* Right) {
		int size;
		int n = (Left->num + Right->num);
		if (n < N - 1)return false;
		if (Left->isLeaf) {
			while (Left->num < n/2) {
				string k = Right->Keys[0];
				int P = Right->Ptrs[0];
				Right->deleteRecord(0);
				Left->insertRecord(k, P);
			}
			while (Right->num < n/2) {
				string k = Left->Keys[Left->num - 1];
				int P = Left->Ptrs[Left->num - 1];
				Left->deleteRecord(Left->num - 1);
				Right->insertRecord(k, P);
			}
		}
		else {
			while (Left->num < n/2) {
				string k = getMin(Right);
				int P = Right->Ptrs[0];
				Right->deleteRecord(0);

				/*将子节点Parent指针改变*/
				//Block* bt = *Buf.readFileBlock((indexName + ".index"), P);
				//string t = bt->getContent(0, 4);
				string t = Buf.getBlock((indexName + ".index"), P, 0, 4);
				size = atoi(t.c_str());
				//t = bt->getContent(5, 5 + size);
				t = Buf.getBlock((indexName + ".index"), P, 5, 5 + size);
				Node child(t);
				child.Parent = Left->selfPtr;
				t = (string)child;
				int size = t.length();
				stringstream ss;
				ss << size;
				string t2;
				ss >> t2;
				//bt->changeContent(0, "    ");
				//bt->changeContent(0, t2);
				Buf.changeValue((indexName + ".index"), P, 0, (string)"    ");
				Buf.changeValue((indexName + ".index"), P, 0, t2);
				//bt->changeContent(5, t);
				Buf.changeValue((indexName + ".index"), P, 5, t);

				Left->insertRecord(k, P);
			}
			while (Right->num < n / 2) {
				string k = getMin(Right);
				int P = Right->Ptrs[0];
				int P2 = Left->Ptrs[Left->num];
				Left->deleteRecord(Left->num);

				/*将子节点Parent指针改变*/
				//Block* bt = *Buf.readFileBlock((indexName + ".index"), P2);
				//string t = bt->getContent(0, 4);
				string t = Buf.getBlock((indexName + ".index"), P2, 0, 4);
				size = atoi(t.c_str());
				//t = bt->getContent(5, 5 + size);
				t = Buf.getBlock((indexName + ".index"), P2, 5, 5 + size);
				Node child(t);
				child.Parent = Right->selfPtr;
				t = (string)child;
				size = t.length();
				stringstream ss;
				ss << size;
				string t2;
				ss >> t2;
				//bt->changeContent(0, "    ");
				//bt->changeContent(0, t2);
				Buf.changeValue((indexName + ".index"), P2, 0, (string)"    ");
				Buf.changeValue((indexName + ".index"), P2, 0, t2);
				//bt->changeContent(5, t);
				Buf.changeValue((indexName + ".index"), P2, 5, t);

				Right->Ptrs[0] = P2;
				Right->insertRecord(k, P);
			}
		}

		return true;
	}

	/*节点与相邻节点均太小，合并*/
	bool mergeNode(Node* Left, Node* Right) {
		if (Left->num + Right->num > N - 1)return false;
		while (Right->num) {
			string k = getMin(Right);
			int P = Right->Ptrs[0];
			Right->deleteRecord(0);
			Left->insertRecord(k, P);
			if (!Left->isLeaf) {
				/*将子节点Parent指针改变*/
				//Block* bt = *Buf.readFileBlock((indexName + ".index"), P);
				//string t = bt->getContent(0, 4);
				string t = Buf.getBlock((indexName + ".index"), P, 0, 4);
				int size = atoi(t.c_str());
				//t = bt->getContent(5, 5 + size);
				t = Buf.getBlock((indexName + ".index"), P, 5, 5 + size);
				Node child(t);
				child.Parent = Left->selfPtr;
				t = (string)child;
				size = t.length();
				stringstream ss;
				ss << size;
				string t2;
				ss >> t2;
				//bt->changeContent(0, "    ");
				//bt->changeContent(0, t2);
				Buf.changeValue((indexName + ".index"), P, 0, (string)"    ");
				Buf.changeValue((indexName + ".index"), P, 0, t2);
				//bt->changeContent(5, t);
				Buf.changeValue((indexName + ".index"), P, 5, t);
			}
		}
		if (Left->isLeaf) {
			Left->Ptrs[N - 1] = Right->Ptrs[N - 1];		//兄弟节点转移
		}
		else {
			string k = getMin(Right);
			int P = Right->Ptrs[0];
			Left->insertRecord(k, P);

			/*将子节点Parent指针改变*/
			//Block* bt = *Buf.readFileBlock((indexName + ".index"), P);
			//string t = bt->getContent(0, 4);
			string t = Buf.getBlock((indexName + ".index"), P, 0, 4);
			int size = atoi(t.c_str());
			//t = bt->getContent(5, 5 + size);
			t = Buf.getBlock((indexName + ".index"), P, 5, 5 + size);
			Node child(t);
			child.Parent = Left->selfPtr;
			t = (string)child;
			size = t.length();
			stringstream ss;
			ss << size;
			string t2;
			ss >> t2;
			//bt->changeContent(0, "    ");
			//bt->changeContent(0, t2);
			Buf.changeValue((indexName + ".index"), P, 0, (string)"    ");
			Buf.changeValue((indexName + ".index"), P, 0, t2);
			//bt->changeContent(5, t);
			Buf.changeValue((indexName + ".index"), P, 5, t);
		}

		return true;
	}

	/*节点已满无法插入时分裂*/
	bool split(Node* Old, string NewKey, int NewP, Node* New) {
		if (!Old->isFull())return nullptr;
		if (Old->isLeaf) {
			int n = Old->getInsertPosition(NewKey);
			if (n != Old->num) {
				string k = Old->Keys[Old->num - 1];
				int p = Old->Ptrs[Old->num - 1];
				Old->deleteRecord(Old->num);
				Old->insertRecord(NewKey, NewP);
				NewKey = k;
				NewP = p;
			}
			New->insertRecord(NewKey, NewP);
		}
		else {
			int n = Old->getInsertPosition(NewKey);
			if (n != Old->num) {
				string k = Old->Keys[Old->num - 1];
				int p = Old->Ptrs[Old->num];
				Old->deleteRecord(Old->num);
				Old->insertRecord(NewKey, NewP);
				NewKey = k;
				NewP = p;
			}
			New->Keys[0] = NewKey;
			New->Ptrs[1] = NewP;
			/**/
			string s2 = Buf.getBlock((indexName + ".index"), NewP, 0, 4);
			int size = atoi(s2.c_str());
			string s = Buf.getBlock((indexName + ".index"), NewP, 5, 5 + size);
			Node no = s;
			no.Parent = New->selfPtr;
			s = no;
			size = s.length();
			stringstream ss;
			ss << size;
			ss >> s2;
			Buf.changeValue((indexName + ".index"), NewP, 0, (string)"    ");
			Buf.changeValue((indexName + ".index"), NewP, 0, s2);
			Buf.changeValue((indexName + ".index"), NewP, 5, s);

			NewP = Old->Ptrs[Old->num];
			Old->deleteRecord(Old->num);
			New->Ptrs[0] = NewP;
			/**/
			s2 = Buf.getBlock((indexName + ".index"), NewP, 0, 4);
			size = atoi(s2.c_str());
			s = Buf.getBlock((indexName + ".index"), NewP, 5, 5 + size);
			no = s;
			no.Parent = New->selfPtr;
			s = no;
			size = s.length();
			ss.clear();
			ss << size;
			ss >> s2;
			Buf.changeValue((indexName + ".index"), NewP, 0, (string)"    ");
			Buf.changeValue((indexName + ".index"), NewP, 0, s2);
			Buf.changeValue((indexName + ".index"), NewP, 5, s);

			New->num++;
		}

		return balanceNode(Old, New);
	}

	/*找到一个记录应该在的子节点*/
	Leaf* findInLeaf(string& Key) {
		if (rootPtr == NOT_EXIST)return new Leaf();
		Node t;
		string s = Buf.getBlock((indexName + ".index"), rootPtr, 0, 4);
		int size = atoi(s.c_str());
		s = Buf.getBlock((indexName + ".index"), rootPtr, 5, 5 + size);
		t = s;
		while (!(t.isLeaf)) {
			int n = t.getInsertPosition(Key);
			if (n < 0)return new Leaf();
			int offset = t.Ptrs[n];
			string s = Buf.getBlock((indexName + ".index"), offset, 0, 4);
			int size = atoi(s.c_str());
			s = Buf.getBlock((indexName + ".index"), offset, 5, 5 + size);
			t = s;
		}

		return new Leaf(t);
	}

public:
	BPlusTree() { rootPtr = -1; }

	/*新建index*/
	void NewIndex(string& indexname,const int& type) {
		rootPtr = -1;
		Node::Type = type;
		indexName = indexname;
		Buf.createFile((indexname + ".index"));
		stringstream ss;
		ss.clear();
		ss << type << ' ' << rootPtr;
		string t;
		char ch[256];
		ss.getline(ch, 256);
		t = ch;
		Buf.insertBlock((indexname + ".index"), t);

		/*TODO:修改catalog*/
	}

	/*打开index*/
	void OpenIndex(string& indexname) {
		indexName = indexname;
		string t = Buf.getBlock((indexname + ".index"), 0, 0, 10);
		stringstream ss;
		ss.clear();
		ss << t;
		ss >> Node::Type;
		ss >> rootPtr;
	}

	/*查找一条记录*/
	int find(string& Key) {
		Leaf* t = findInLeaf(Key);
		if (t->num == 0) { 
			delete t;
			return NOT_EXIST; 
		}
		int i = t->find(Key);
		if (i == NOT_EXIST) {
			delete t;
			return NOT_EXIST;
		}
		i = t->Ptrs[i];
		delete t;
		return i;
	}

	/*批量查找，给出最大值与最小值*/
	void findBetween(string min, string max, vector<int>& results) {
		if (rootPtr == NOT_EXIST)return;

		bool include = false;

		string s2 = Buf.getBlock((indexName + ".index"), rootPtr, 0, 4);
		int size = atoi(s2.c_str());
		string s = Buf.getBlock((indexName + ".index"), rootPtr, 5, 5 + size);
		Node nod = s;
		Node* t = &nod;

		string minbored = min;
		if (min.empty()) {
			minbored = getMin(t);
		}
		t = findInLeaf(minbored);
		Leaf t2 = *t;
		delete t;
		t = &t2;
		
		int Order = t->getInsertPosition(minbored);
		if (min == "")Order--;
		string maxbored = max;
		if (!max.empty()) {
			Leaf* end = findInLeaf(max);
			int ord = end->getInsertPosition(max);
			if (ord == 0)include = true;
			ord--;
			maxbored = end->Keys[ord + include];
			if (maxbored != max)include = true;
			delete end;
		}
		int i;
		int Ptr;
		string Key;
		for (i = Order; i < t->num; i++) {
			Key = t->Keys[i];
			Ptr = t->Ptrs[i];
			if (Key == maxbored)break;
			results.push_back(Ptr);
		}
		while (i >= t->num && t->Ptrs[N - 1] != NOT_EXIST) {
			int offset = t->Ptrs[N - 1];
			string s2 = Buf.getBlock((indexName + ".index"), offset, 0, 4);
			int size = atoi(s2.c_str());
			string s = Buf.getBlock((indexName + ".index"), offset, 5, 5 + size);
			Leaf tl = (Node)s;
			t = &tl;
			for (i = 0; i < t->num; i++) {
				Key = t->Keys[i];
				Ptr = t->Ptrs[i];
				if (Key == maxbored)break;
				results.push_back(Ptr);
			}
		}
		if (include)results.push_back(Ptr);

		return;
	}

	/*插入一条记录*/
	bool insertRecord(string& Key,int Ptr) {
		if (rootPtr == NOT_EXIST) {					//index新建立，无节点
			int offset = Buf.insertBlock((indexName + ".index"), (string)"0   ");
			Leaf t;
			t.selfPtr = offset;
			t.Parent = NOT_EXIST;
			t.Ptrs[N - 1] = NOT_EXIST;
			rootPtr = t.selfPtr;
			t.insertRecord(Key, Ptr);
			string s = t;
			int size = s.length();
			stringstream ss;
			ss << size;
			string s2;
			ss >> s2;
			Buf.changeValue((indexName + ".index"), offset, 0, (string)"    ");
			Buf.changeValue((indexName + ".index"), offset, 0, s2);
			Buf.changeValue((indexName + ".index"), offset, 5, s);
			ss.clear();
			ss << Node::Type << ' ' << rootPtr;
			char ch[256];
			ss.getline(ch, 256);
			s = ch;
			s += " ";
			Buf.changeValue((indexName + ".index"), 0, 0, s);
			return true;
		}
		Node* t = findInLeaf(Key);
		Leaf t2 = *t;
		delete t;
		t = &t2;
		banch r;
		while (t->isFull() && t->selfPtr != rootPtr) {		//节点已满，分裂
			Node* New;
			int offset = Buf.insertBlock((indexName + ".index"), (string)"0   ");
			if (t->isLeaf) {
				New = new Leaf();
				New->selfPtr = offset;
				New->Ptrs[N - 1] = t->Ptrs[N - 1];
				t->Ptrs[N - 1] = New->selfPtr;
			}
			else {
				New = new banch();
				New->selfPtr = offset;
			}
			New->Parent = t->Parent;
			split(t, Key, Ptr, New);

			/*写回*/
			string s = *New;
			int size = s.length();
			string s2;
			stringstream ss;
			ss << size;
			ss >> s2;
			Buf.changeValue((indexName + ".index"), offset, 0, (string)"    ");
			Buf.changeValue((indexName + ".index"), offset, 0, s2);
			Buf.changeValue((indexName + ".index"), offset, 5, s);

			s = *t;
			size = s.length();
			ss.clear();
			ss << size;
			ss >> s2;
			Buf.changeValue((indexName + ".index"), t->selfPtr, 0, (string)"    ");
			Buf.changeValue((indexName + ".index"), t->selfPtr, 0, s2);
			Buf.changeValue((indexName + ".index"), t->selfPtr, 5, s);

			/*TODO:block++*/

			Key = getMin(New);
			Ptr = t->Parent;
			s2 = Buf.getBlock((indexName + ".index"), Ptr, 0, 5);
			size = atoi(s2.c_str());
			s = Buf.getBlock((indexName + ".index"), Ptr, 5, 5 + size);
			Ptr = New->selfPtr;
			r = (Node)s;
			t = &r;
			delete New;
		}

		if (t->isFull() && t->selfPtr == rootPtr) {
			banch r2;
			int offset0 = Buf.insertBlock((indexName + ".index"), (string)"0    ");
			r2.selfPtr = offset0;
			r2.Parent = NOT_EXIST;
			rootPtr = offset0;
			t->Parent = r2.selfPtr;
			Node* New;
			int offset = Buf.insertBlock((indexName + ".index"), (string)"0   ");
			if (t->isLeaf) {
				New = new Leaf();
				New->selfPtr = offset;
				New->Ptrs[N - 1] = t->Ptrs[N - 1];
				t->Ptrs[N - 1] = New->selfPtr;
			}
			else {
				New = new banch();
				New->selfPtr = offset;
			}
			New->Parent = t->Parent;
			split(t, Key, Ptr, New);
			r2.Ptrs[0] = t->selfPtr;
			r2.Ptrs[1] = New->selfPtr;
			r2.Keys[0] = getMin(New);
			r2.num = 1;

			/*写回*/
			string s = *New;
			int size = s.length();
			string s2;
			stringstream ss;
			ss << size;
			ss >> s2;
			Buf.changeValue((indexName + ".index"), offset, 0, (string)"    ");
			Buf.changeValue((indexName + ".index"), offset, 0, s2);
			Buf.changeValue((indexName + ".index"), offset, 5, s);

			s = *t;
			size = s.length();
			ss.clear();
			ss << size;
			ss >> s2;
			Buf.changeValue((indexName + ".index"), t->selfPtr, 0, (string)"    ");
			Buf.changeValue((indexName + ".index"), t->selfPtr, 0, s2);
			Buf.changeValue((indexName + ".index"), t->selfPtr, 5, s);

			s = r2;
			size = s.length();
			ss.clear();
			ss << size;
			ss >> s2;
			Buf.changeValue((indexName + ".index"), offset0, 0, (string)"    ");
			Buf.changeValue((indexName + ".index"), offset0, 0, s2);
			Buf.changeValue((indexName + ".index"), offset0, 5, s);

			ss.clear();
			ss << Node::Type << ' ' << rootPtr;
			char ch[256];
			ss.getline(ch, 256);
			s = ch;
			s += " ";
			Buf.changeValue((indexName + ".index"), 0, 0, s);

			delete New;
			return true;
		}
		else {
			t->insertRecord(Key, Ptr);
			/*写回*/
			string s = *t;
			int size = s.length();
			string s2;
			stringstream ss;
			ss << size;
			ss >> s2;
			Buf.changeValue((indexName + ".index"), t->selfPtr, 0, (string)"    ");
			Buf.changeValue((indexName + ".index"), t->selfPtr, 0, s2);
			Buf.changeValue((indexName + ".index"), t->selfPtr, 5, s);

			return true;
		}

	}

	/*删除一条记录*/
	bool deleteRecord(string& Key) {
		if (rootPtr == NOT_EXIST)return false;
		Node* t = findInLeaf(Key);
		Leaf nol;
		banch nob, child, Prt, nob2;
		Leaf t2 = *t;
		delete t;
		t = &t2;
		if (t->num == 0)return false;
		bool Update=false;
		string UpdateKey;
		int order = t->find(Key);
		if (order < 0)return false;
		t->deleteRecord(order);
		if (order == 0) {						//子节点0位置记录被删除时，有父节点记录需要更新
			Update = true;
			UpdateKey = getMin(t);
		}
		Node *Left, *Right;
		while(t->isLess()&&t->selfPtr!=rootPtr) {						//节点记录数太少
			int offset = t->Parent;
			//*Buf.findBlockInBuffer((indexName + ".index"), offset);
			//string s2 = ParB->getContent(0, 4);
			string s2 = Buf.getBlock((indexName + ".index"), offset, 0, 4);
			int size = atoi(s2.c_str());
			string s = Buf.getBlock((indexName + ".index"), offset, 5, 5 + size);
			Prt = (Node)s;
			order = Prt.find(t->selfPtr);
			if (order == 0) {
				int offset1 = Prt.Ptrs[1];
				//Block* Sib = *Buf.findBlockInBuffer((indexName + ".index"), offset);
				//s = Sib->getContent(0, 4);
				string s = Buf.getBlock((indexName + ".index"), offset1, 0, 4);
				size = atoi(s.c_str());
				s = Buf.getBlock((indexName + ".index"), offset1, 5, 5 + size);
				Node t2(s);
				if (t2.isLeaf) {
					nol = t2;
					Right = &nol;
				}
				else {
					nob = t2;
					Right = &nob;
				}
				Left = t;
				if (Right->num <= (N - 1) / 2) {
					mergeNode(Left, Right);
					s = *Left;
					size = s.length();
					stringstream ss;
					ss.clear();
					ss << size;
					ss >> s2;
					Buf.changeValue((indexName + ".index"), Left->selfPtr, 0, (string)"    ");
					Buf.changeValue((indexName + ".index"), Left->selfPtr, 0, s2);
					Buf.changeValue((indexName + ".index"), Left->selfPtr, 5, s);

					Buf.deleteBlock((indexName + ".index"), Right->selfPtr);
					Prt.deleteRecord(1);
					nob2 = Prt;
					t = &nob2;
				}
				else {
					balanceNode(Left, Right);
					s = *Left;
					size = s.length();
					stringstream ss;
					ss.clear();
					ss << size;
					ss >> s2;
					Buf.changeValue((indexName + ".index"), Left->selfPtr, 0, (string)"    ");
					Buf.changeValue((indexName + ".index"), Left->selfPtr, 0, s2);
					Buf.changeValue((indexName + ".index"), Left->selfPtr, 5, s);

					s = *Right;
					size = s.length();
					ss.clear();
					ss << size;
					ss >> s2;
					//Sib->changeContent(0, s2);
					Buf.changeValue((indexName + ".index"), offset1, 0, (string)"    ");
					Buf.changeValue((indexName + ".index"), offset1, 0, s2);
					//Sib->changeContent(5, s);
					Buf.changeValue((indexName + ".index"), offset1, 5, s);

					Prt.Keys[0] = getMin(Right);
					s = Prt;
					size = s.length();
					ss.clear();
					ss << size;
					ss >> s2;
					//ParB->changeContent(0, s2);
					Buf.changeValue((indexName + ".index"), offset, 0, (string)"    ");
					Buf.changeValue((indexName + ".index"), offset, 0, s2);
					//ParB->changeContent(5, s);
					Buf.changeValue((indexName + ".index"), offset, 5, s);

					nob2 = Prt;
					t = &nob2;
					break;
				}
			}
			else {
				int offset1 = Prt.Ptrs[order - 1];
				//Block* Sib = *Buf.findBlockInBuffer((indexName + ".index"), offset);
				//s = Sib->getContent(0, 4);
				s = Buf.getBlock((indexName + ".index"), offset1, 0, 4);
				size = atoi(s.c_str());
				//s = Sib->getContent(5, 5 + size);
				s = Buf.getBlock((indexName + ".index"), offset1, 5, 5 + size);
				Node t2(s);
				if (t2.isLeaf) {
					nol = t2;
					Left = &nol;
				}
				else {
					nob = t2;
					Left = &nob;
				}
				Right = t;
				if (Left->num <= (N - 1) / 2) {
					mergeNode(Left, Right);
					s = *Left;
					size = s.length();
					stringstream ss;
					ss.clear();
					ss << size;
					ss >> s2;
					Buf.changeValue((indexName + ".index"), Left->selfPtr, 0, (string)"    ");
					Buf.changeValue((indexName + ".index"), Left->selfPtr, 0, s2);
					Buf.changeValue((indexName + ".index"), Left->selfPtr, 5, s);

					Buf.deleteBlock((indexName + ".index"), Right->selfPtr);
					Prt.deleteRecord(order);
					nob2 = Prt;
					t = &nob2;
					Update = false;
				}
				else {
					balanceNode(Left, Right);
					s = *Right;
					size = s.length();
					stringstream ss;
					ss.clear();
					ss << size;
					ss >> s2;
					Buf.changeValue((indexName + ".index"), Right->selfPtr, 0, (string)"    ");
					Buf.changeValue((indexName + ".index"), Right->selfPtr, 0, s2);
					Buf.changeValue((indexName + ".index"), Right->selfPtr, 5, s);

					s = *Left;
					size = s.length();
					ss.clear();
					ss << size;
					ss >> s2;
					//Sib->changeContent(0, s2);
					Buf.changeValue((indexName + ".index"), offset1, 0, (string)"    ");
					Buf.changeValue((indexName + ".index"), offset1, 0, s2);
					//Sib->changeContent(5, s);
					Buf.changeValue((indexName + ".index"), offset1, 5, s);

					Prt.Keys[order - 1] = getMin(Right);
					s = Prt;
					size = s.length();
					ss.clear();
					ss << size;
					ss >> s2;
					//ParB->changeContent(0, s2);
					Buf.changeValue((indexName + ".index"), offset, 0, (string)"    ");
					Buf.changeValue((indexName + ".index"), offset, 0, s2);
					//ParB->changeContent(5, s);
					Buf.changeValue((indexName + ".index"), offset, 5, s);

					Update = false;
					break;
				}
			}
		}

		if (t->selfPtr == rootPtr&&t->num == 0 && t->isLeaf == false) {		//根节点太少，删除
			Node* child;
			int offset = t->Ptrs[0];
			string s2 = Buf.getBlock((indexName + ".index"), offset, 0, 4);
			int size = atoi(s2.c_str());
			string s = Buf.getBlock((indexName + ".index"), offset, 5, 5 + size);
			Node no(s);
			if (no.isLeaf) {
				nol = no;
				child = &nol;
			}
			else {
				nob = no;
				child = &nob;
			}
			child->Parent = NOT_EXIST;
			rootPtr = child->selfPtr;
			Buf.deleteBlock((indexName + ".index"), t->selfPtr);

			/*写回*/
			s = *child;
			size = s.length();
			stringstream ss;
			ss << size;
			ss >> s2;
			Buf.changeValue((indexName + ".index"), offset, 0, (string)"    ");
			Buf.changeValue((indexName + ".index"), offset, 0, s2);
			Buf.changeValue((indexName + ".index"), offset, 5, s);

			ss.clear();
			ss << Node::Type << ' ' << rootPtr;
			char ch[256];
			ss.getline(ch, 256);
			s = ch;
			s += " ";
			Buf.changeValue((indexName + ".index"), 0, 0, s);
		}
		else {
			int offset = t->selfPtr;
			string s = *t;
			int size = s.length();
			stringstream ss;
			ss << size;
			string s2;
			ss >> s2;
			Buf.changeValue((indexName + ".index"), offset, 0, (string)"    ");
			Buf.changeValue((indexName + ".index"), offset, 0, s2);
			Buf.changeValue((indexName + ".index"), offset, 5, s);
		}

		while (Update&&t->selfPtr != rootPtr) {
			int offset = t->Parent;
			string s2 = Buf.getBlock((indexName + ".index"), offset, 0, 4);
			int size = atoi(s2.c_str());
			string s = Buf.getBlock((indexName + ".index"), offset, 5, 5 + size);
			banch Par = (Node)s;
			order = Par.find(t->selfPtr);
			if (order > 0) {
				Par.Keys[order - 1] = UpdateKey;
				
				s = Par;
				size = s.length();
				stringstream ss;
				ss << size;
				ss >> s2;
				Buf.changeValue((indexName + ".index"), offset, 0, (string)"    ");
				Buf.changeValue((indexName + ".index"), offset, 0, s2);
				Buf.changeValue((indexName + ".index"), offset, 5, s);

				Update = false;
			}
			else {
				child = Par;
				t = &child;
			}
		}

		return true;
	}

};

class IndexManager {
public:
	/*新建一个索引*/
	BPlusTree createIndex(string& IndexName,const int& type) {
		//TODO: 更新index catalog

		BPlusTree bpt;
		bpt.NewIndex(IndexName, type);
		return bpt;
	}

	/*打开已有索引*/
	BPlusTree openIndex(string& IndexName) {
		BPlusTree bpt;
		bpt.OpenIndex(IndexName);
		return bpt;
	}

	/*插入一条记录及其指针*/
	bool insertRecord(string& IndexName, string& key,const int& ptr) {
		BPlusTree bpt;
		bpt.OpenIndex(IndexName);
		return bpt.insertRecord(key, ptr);
	}

	/*删除一条记录*/
	bool deleteRecord(string& IndexName, string& key) {
		BPlusTree bpt;
		bpt.OpenIndex(IndexName);
		return bpt.deleteRecord(key);
	}

	/*查找一个记录的指针，返回-1表示记录不存在*/
	int find(string& IndexName, string& key) {
		BPlusTree bpt;
		bpt.OpenIndex(IndexName);
		return bpt.find(key);
	}

	/*区间查找，给定最小值最大值与一个空list，返回区间内的值（闭区间，可能包括最大与最小），单边查询请将另一边边界设为空字符串（""）*/
	void findBetween(string& IndexName, string& min, string& max, vector<int>& results) {
		BPlusTree bpt;

		bpt.OpenIndex(IndexName);
		string tmin = min, tmax = max;
		bpt.findBetween(tmin, tmax, results);
		return;
	}

	/*删除索引*/
	void dropIndex(string& IndexName) {
		Buf.dropFile((IndexName + ".index"));
	}
};

#endif _INDEXMANAGER_H_
