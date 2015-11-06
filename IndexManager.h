#pragma once

#include"BufferManager.h"
#include<sstream>
using namespace std;

#ifndef ORDER_NUMBER		//B+ tree����
#define ORDER_NUMBER 13
#endif

#define N ORDER_NUMBER

#define NOT_EXIST -1		//offset=-1��ʾ������

extern BufferManager Buf;

class Node {				//�ڵ�ӿ�
public:
	int selfPtr;			//����offset
	string Keys[N - 1];			//��¼
	int Ptrs[N];		//ָ�룬ʵ��Ϊblockoffset
	int Parent;				//���ڵ㣬blockoffset��-1��ʾû��
	static int Type;		//key����
	int num;				//�ڵ��¼��
	bool isLeaf;

	Node() { num = 0; }

	/*��string��ʽ�洢�Ľڵ���Ϣת��Ϊ�ڵ�*/
	Node(string str) {
		stringstream ss;
		char t[256];
		ss << str;
		ss >> selfPtr;
		ss >> isLeaf;
		ss >> Parent;
		ss >> num;
		for (int i = 0; i < num; i++) {
			ss >> Ptrs[i];
			ss >> t[0];
			ss.get(t, 256, '\t');
			Keys[i] = t;
			ss >> t[0];
		}
		if (isLeaf) {
			ss >> Ptrs[N - 1];
		}
		else {
			ss >> Ptrs[num];
		}
	}

	/* ���ڵ���Ϣ��ʽ��ת��Ϊ�ַ���*/
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
		ss >> t;
		return t;
	}

	virtual int insertRecord(string Key, int Ptr) {};
	virtual int deleteRecord(int Order) {};
	
	/*�ҵ�һ��ָ���ڽڵ��е�λ��,-1��ʾ������*/
	virtual int find(int Ptr) {
		int i = num;
		for (; i >= 0;i--)
			if (Ptr==Ptrs[i]){
				break;
			}

		return i;
	}

	/*�ҵ�һ����¼�ڽڵ��е�λ�ã�-1��ʾ������*/
	virtual int find(string Key) {
		int i = num - 1;
		for (; i >= 0; i--)
			if (Key == Keys[i])return i;
		return i;
	}

	/*���ڵ��Ƿ���*/
	virtual bool isFull() {
		return num >= N - 1;
	}

	/*���ڵ��Ƿ�С����С״̬*/
	virtual bool isLess() {
		return num < (N - 1) / 2;
	}

	/*�ҵ�����λ�ã���������*/
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

int Node::Type;		//key����

class banch :public Node {		//��Ҷ�ڵ�
public:
	banch() { isLeaf = false; num = 0; }
	banch(const Node& n) {
		selfPtr = n.selfPtr;
		num = n.num;
		Parent = n.Parent;
		isLeaf = false;
		for (int i = 0; i < num; i++) {
			Ptrs[i] = n.Ptrs[i];
			Keys[i] = n.Keys[i];
		}
		Ptrs[num] = n.Ptrs[num];
	}

	/*�����¼�����ز���λ��*/
	int insertRecord(string key, int ptr){
		if (num == N - 1)return -2;

		int n = getInsertPosition(key);
		if (n == -1)return n;
		int i = num;
		for (; i > n+1; i--) {
			Keys[i] = Keys[i - 1];
			Ptrs[i + 1] = Ptrs[i];
		}
		Keys[i - 1] = key;
		Ptrs[i] = ptr;
		num++;
		return n;
	}

	/*����ָ��λ��ɾ��һ����¼*/
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

class Leaf :public Node {			//Ҷ�ڵ�
public:
	Leaf() { isLeaf = true; num = 0; }
	Leaf(const Node& n) {
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

	/*�����¼�����ز���λ��*/
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

	/*����ָ�롢��¼λ��ɾ��һ����¼*/
	int deleteRecord(int Order) {
		for (int i = Order; i < num; i++) {
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

	/*��������node�µ���Сֵ*/
	string getMin(Node* nod) {
		if (nod->isLeaf) {
			if (nod->num == 0)return "";
			return nod->Keys[0];
		}

		int offset = nod->Ptrs[0];
		string t = Buf.getValue((indexName + ".index"), offset, 0, 4);
		int size = atoi(t.c_str());
		t = Buf.getValue((indexName	+ ".index"), offset, 5, 5 + size);
		Node node(t);
		return getMin(&node);
	}

	/*һ���ڵ��¼���٣�ʹ�����ڽڵ����ƽ��*/
	bool balanceNode(Node* Left, Node* Right) {
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

				/*���ӽڵ�Parentָ��ı�*/
				Block* bt = *Buf.readFileBlock((indexName + ".index"), P);
				string t = bt->getContent(0, 4);
				//string t = Buf.getValue((indexName + ".index"), P, 0, 4);
				int size = atoi(t.c_str());
				t = bt->getContent(5, 5 + size);
				//t = Buf.getValue((indexName + ".index"), P, 5, 5 + size);
				Node child(t);
				child.Parent = Left->selfPtr;
				t = (string)child;
				int size = t.length();
				stringstream ss;
				ss << size;
				string t2;
				ss >> t2;
				bt->changeContent(0, t2);
				//Buf.changeValue((indexName + ".index"), P, 0, t2);
				bt->changeContent(5, t);
				//Buf.changeValue((indexName + ".index"), P, 5, t);

				Left->insertRecord(k, P);
			}
			while (Right->num < n / 2) {
				string k = getMin(Right);
				int P = Right->Ptrs[0];
				int P2 = Left->Ptrs[Left->num];
				Left->deleteRecord(Left->num);

				/*���ӽڵ�Parentָ��ı�*/
				Block* bt = *Buf.readFileBlock((indexName + ".index"), P2);
				string t = bt->getContent(0, 4);
				//string t = Buf.getValue((indexName + ".index"), P2, 0, 4);
				int size = atoi(t.c_str());
				t = bt->getContent(5, 5 + size);
				//string t = Buf.getValue((indexName + ".index"), P2, 5, 5 + size);
				Node child(t);
				child.Parent = Right->selfPtr;
				t = (string)child;
				int size = t.length();
				stringstream ss;
				ss << size;
				string t2;
				ss >> t2;
				bt->changeContent(0, t2);
				//Buf.changeValue((indexName + ".index"), P2, 0, t2);
				bt->changeContent(5, t);
				//Buf.changeValue((indexName + ".index"), P2, 5, t);

				Right->Ptrs[0] = P2;
				Right->insertRecord(k, P);
			}
		}

		return true;
	}

	/*�ڵ������ڽڵ��̫С���ϲ�*/
	bool mergeNode(Node* Left, Node* Right) {
		if (Left->num + Right->num > N - 1)return false;
		while (Right->num) {
			string k = getMin(Right);
			int P = Right->Ptrs[0];
			Right->deleteRecord(0);
			Left->insertRecord(k, P);
			if (!Left->isLeaf) {
				/*���ӽڵ�Parentָ��ı�*/
				Block* bt = *Buf.readFileBlock((indexName + ".index"), P);
				string t = bt->getContent(0, 4);
				//string t = Buf.getValue((indexName + ".index"), P, 0, 4);
				int size = atoi(t.c_str());
				t = bt->getContent(5, 5 + size);
				//t = Buf.getValue((indexName + ".index"), P, 5, 5 + size);
				Node child(t);
				child.Parent = Left->selfPtr;
				t = (string)child;
				int size = t.length();
				stringstream ss;
				ss << size;
				string t2;
				ss >> t2;
				bt->changeContent(0, t2);
				//Buf.changeValue((indexName + ".index"), P, 0, t2);
				bt->changeContent(5, t);
				//Buf.changeValue((indexName + ".index"), P, 5, t);
			}
		}
		if (Left->isLeaf) {
			Left->Ptrs[N - 1] = Right->Ptrs[N - 1];		//�ֵܽڵ�ת��
		}
		else {
			string k = getMin(Right);
			int P = Right->Ptrs[0];
			Left->insertRecord(k, P);

			/*���ӽڵ�Parentָ��ı�*/
			Block* bt = *Buf.readFileBlock((indexName + ".index"), P);
			string t = bt->getContent(0, 4);
			//string t = Buf.getValue((indexName + ".index"), P, 0, 4);
			int size = atoi(t.c_str());
			t = bt->getContent(5, 5 + size);
			//t = Buf.getValue((indexName + ".index"), P, 5, 5 + size);
			Node child(t);
			child.Parent = Left->selfPtr;
			t = (string)child;
			int size = t.length();
			stringstream ss;
			ss << size;
			string t2;
			ss >> t2;
			bt->changeContent(0, t2);
			//Buf.changeValue((indexName + ".index"), P, 0, t2);
			bt->changeContent(5, t);
			//Buf.changeValue((indexName + ".index"), P, 5, t);
		}

		return true;
	}

	/*�ڵ������޷�����ʱ����*/
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
			NewP = Old->Ptrs[Old->num];
			Old->deleteRecord(Old->num);
			New->Ptrs[0] = NewP;
			New->num;
		}

		return balanceNode(Old, New);
	}

	/*�ҵ�һ����¼Ӧ���ڵ��ӽڵ�*/
	Leaf findInLeaf(string Key) {
		if (rootPtr == NOT_EXIST)return Leaf();
		Node t;
		string s = Buf.getValue((indexName + ".index"), rootPtr, 0, 4);
		int size = atoi(s.c_str());
		s = Buf.getValue((indexName + ".index"), rootPtr, 5, 5 + size);
		t = s;
		while (!(t.isLeaf)) {
			int n = t.getInsertPosition(Key);
			if (n < 0)return Leaf();
			string s = Buf.getValue((indexName + ".index"), n, 0, 4);
			int size = atoi(s.c_str());
			s = Buf.getValue((indexName + ".index"), n, 5, 5 + size);
			t = s;
		}

		return (Leaf)t;
	}

public:
	BPlusTree() { rootPtr = -1; }

	/*�½�index*/
	void NewIndex(string indexname, int type) {
		rootPtr = -1;
		Node::Type = type;
		indexName = indexname;
		Buf.createFile((indexname + ".index"));
		stringstream ss;
		ss.clear();
		ss << type << ' ' << rootPtr;
		string t;
		ss >> t;
		Buf.changeValue((indexname + ".index"), 0, 0, t);

		/*TODO:�޸�catalog*/
	}

	/*��index*/
	void OpenIndex(string indexname) {
		indexName = indexname;
		string t = Buf.getValue((indexname + ".index"), 0, 0, 10);
		stringstream ss;
		ss.clear();
		ss >> Node::Type;
		ss >> rootPtr;
	}

	/*����һ����¼*/
	int find(string Key) {
		Leaf* t = &findInLeaf(Key);
		if (t->num == 0)return NOT_EXIST;
		return t->find(Key);
	}

	/*�������ң��������ֵ����Сֵ*/
	void findBetween(string min, string max, list<int>& results) {
		if (rootPtr == NOT_EXIST)return;

		string s2 = Buf.getValue((indexName + ".index"), rootPtr, 0, 4);
		int size = atoi(s2.c_str);
		string s = Buf.getValue((indexName + ".index"), rootPtr, 0, 4);
		Node* t = &(Node)s;

		if (min.empty()) {
			min = getMin(t);
		}
		Leaf* t = &findInLeaf(min);
		int Order = t->getInsertPosition(min);
		string maxbored = max;
		if (!max.empty()) {
			Leaf* end = &findInLeaf(max);
			int ord = end->getInsertPosition(max);
			maxbored = end->Keys[ord];
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
			string s2 = Buf.getValue((indexName + ".index"), offset, 0, 4);
			int size = atoi(s2.c_str());
			string s = Buf.getValue((indexName + ".index"), offset, 5, 5 + size);
			t = &(Leaf)s;
			for (i = 0; i < t->num; i++) {
				Key = t->Keys[i];
				Ptr = t->Ptrs[i];
				if (Key == maxbored)break;
				results.push_back(Ptr);
			}
		}

		if (Key == max)results.push_back(Ptr);

		return;
	}

	/*����һ����¼*/
	bool insertRecord(string Key, int Ptr) {
		if (rootPtr == NOT_EXIST) {					//index�½������޽ڵ�
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
			Buf.changeValue((indexName + ".index"), offset, 0, s2);
			Buf.changeValue((indexName + ".index"), offset, 5, s);
			return true;
		}
		Node* t = &findInLeaf(Key);
		banch r;
		while (t->isFull() && t->selfPtr != rootPtr) {		//�ڵ�����������
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

			/*д��*/
			string s = *New;
			int size = s.length();
			string s2;
			stringstream ss;
			ss << size;
			ss >> s2;
			Buf.changeValue((indexName + ".index"), offset, 0, s2);
			Buf.changeValue((indexName + ".index"), offset, 5, s);

			s = *t;
			int size = s.length();
			ss.clear();
			ss << size;
			ss >> s2;
			Buf.changeValue((indexName + ".index"), t->selfPtr, 0, s2);
			Buf.changeValue((indexName + ".index"), t->selfPtr, 5, s);

			/*TODO:block++*/

			Key = getMin(New);
			Ptr = t->Parent;
			s2 = Buf.getValue((indexName + ".index"), Ptr, 0, 5);
			size = atoi(s2.c_str());
			s = Buf.getValue((indexName + ".index"), Ptr, 5, 5 + size);
			r = (Node)s;
			t = &r;
			delete New;
		}

		if (t->isFull() && t->selfPtr == rootPtr) {
			int offset0 = Buf.insertBlock((indexName + ".index"), (string)"0    ");
			r.selfPtr = offset0;
			r.Parent = NOT_EXIST;
			rootPtr = offset0;
			t->Parent = r.selfPtr;
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
			r.Ptrs[0] = t->selfPtr;
			r.Ptrs[1] = New->selfPtr;
			r.Keys[0] = getMin(New);
			r.num = 1;

			/*д��*/
			string s = *New;
			int size = s.length();
			string s2;
			stringstream ss;
			ss << size;
			ss >> s2;
			Buf.changeValue((indexName + ".index"), offset, 0, s2);
			Buf.changeValue((indexName + ".index"), offset, 5, s);

			s = *t;
			size = s.length();
			ss.clear();
			ss << size;
			ss >> s2;
			Buf.changeValue((indexName + ".index"), t->selfPtr, 0, s2);
			Buf.changeValue((indexName + ".index"), t->selfPtr, 5, s);

			s = r;
			size = s.length();
			ss.clear();
			ss << size;
			ss >> s2;
			Buf.changeValue((indexName + ".index"), offset0, 0, s2);
			Buf.changeValue((indexName + ".index"), offset0, 5, s);

			delete New;
			return true;
		}
		else {
			t->insertRecord(Key, Ptr);
			/*д��*/
			string s = *t;
			int size = s.length();
			string s2;
			stringstream ss;
			ss << size;
			ss >> s2;
			Buf.changeValue((indexName + ".index"), t->selfPtr, 0, s2);
			Buf.changeValue((indexName + ".index"), t->selfPtr, 5, s);

			return true;
		}

	}

	/*ɾ��һ����¼*/
	bool deleteRecord(string Key) {
		if (rootPtr == NOT_EXIST)return false;
		Node* t = &findInLeaf(Key);
		if (t->num == 0)return false;
		bool Update=false;
		string UpdateKey;
		int order = t->find(Key);
		if (order < 0)return false;
		t->deleteRecord(order);
		if (order == 0) {						//�ӽڵ�0λ�ü�¼��ɾ��ʱ���и��ڵ��¼��Ҫ����
			Update = true;
			UpdateKey = getMin(t);
		}
		Node *Left, *Right;
		while(t->isLess()&&t->selfPtr!=rootPtr) {						//�ڵ��¼��̫��
			int offset = t->Parent;
			Block* ParB = *Buf.findBlockInBuffer((indexName + ".index"), offset);
			string s2 = ParB->getContent(0, 4);
			int size = atoi(s2.c_str());
			string s = ParB->getContent(5, 5 + size);
			banch Prt = (Node)s;
			order = Prt.find(t->selfPtr);
			if (order == 0) {
				offset = Prt.Ptrs[1];
				Block* Sib = *Buf.findBlockInBuffer((indexName + ".index"), offset);
				s = Sib->getContent(0, 4);
				size = atoi(s.c_str());
				s = Sib->getContent(5, 5 + size);
				Node t2(s);
				if (t2.isLeaf) {
					Right = &(Leaf)t2;
				}
				else {
					Right = &(banch)t2;
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
					Buf.changeValue((indexName + ".index"), Left->selfPtr, 0, s2);
					Buf.changeValue((indexName + ".index"), Left->selfPtr, 5, s);

					Buf.deleteBlock((indexName + ".index"), Right->selfPtr);
					Prt.deleteRecord(1);
					t = &Prt;
				}
				else {
					balanceNode(Left, Right);
					s = *Left;
					size = s.length();
					stringstream ss;
					ss.clear();
					ss << size;
					ss >> s2;
					Buf.changeValue((indexName + ".index"), Left->selfPtr, 0, s2);
					Buf.changeValue((indexName + ".index"), Left->selfPtr, 5, s);

					s = *Right;
					size = s.length();
					ss.clear();
					ss << size;
					ss >> s2;
					Sib->changeContent(0, s2);
					Sib->changeContent(5, s);

					Prt.Keys[0] = getMin(Right);
					s = Prt;
					size = s.length();
					ss.clear();
					ss << size;
					ss >> s2;
					ParB->changeContent(0, s2);
					ParB->changeContent(5, s);

					t = &Prt;
					break;
				}
			}
			else {
				offset = Prt.Ptrs[order - 1];
				Block* Sib = *Buf.findBlockInBuffer((indexName + ".index"), offset);
				s = Sib->getContent(0, 4);
				size = atoi(s.c_str());
				s = Sib->getContent(5, 5 + size);
				Node t2(s);
				if (t2.isLeaf) {
					Left = &(Leaf)t2;
				}
				else {
					Left = &(banch)t2;
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
					Buf.changeValue((indexName + ".index"), Left->selfPtr, 0, s2);
					Buf.changeValue((indexName + ".index"), Left->selfPtr, 5, s);

					Buf.deleteBlock((indexName + ".index"), Right->selfPtr);
					Prt.deleteRecord(1);
					t = &Prt;
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
					Buf.changeValue((indexName + ".index"), Right->selfPtr, 0, s2);
					Buf.changeValue((indexName + ".index"), Right->selfPtr, 5, s);

					s = *Left;
					size = s.length();
					ss.clear();
					ss << size;
					ss >> s2;
					Sib->changeContent(0, s2);
					Sib->changeContent(5, s);

					Prt.Keys[0] = getMin(Right);
					s = Prt;
					size = s.length();
					ss.clear();
					ss << size;
					ss >> s2;
					ParB->changeContent(0, s2);
					ParB->changeContent(5, s);

					Update = false;
					break;
				}
			}
		}

	
		int offset = t->selfPtr;
		string s = *t;
		int size = s.length();

		while (Update&&t->selfPtr != rootPtr) {
			int offset = t->Parent;
			string s2 = Buf.getValue((indexName + ".index"), offset, 0, 4);
			int size = atoi(s2.c_str());
			string s = Buf.getValue((indexName + ".index"), offset, 5, 5 + size);
			banch Par = (Node)s;
			order = Par.find(t->selfPtr);
			if (order > 0) {
				Par.Keys[order - 1] = UpdateKey;
				
				s = Par;
				size = s.length();
				stringstream ss;
				ss << size;
				ss >> s2;
				Buf.changeValue((indexName + ".index"), offset, 0, s2);
				Buf.changeValue((indexName + ".index"), offset, 5, s);

				Update = false;
			}
			else {
				t = &Par;
			}
		}

		return true;
	}

};

class IndexManager {
public:
	/*�½�һ������*/
	BPlusTree createIndex(string IndexName, string table, string attribute, int type) {
		//TODO: ����index catalog

		BPlusTree bpt;
		bpt.NewIndex(IndexName, type);
		return bpt;
	}

	/*����������*/
	BPlusTree openIndex(string IndexName) {
		BPlusTree bpt;
		bpt.OpenIndex(IndexName);
		return bpt;
	}

	/*����һ����¼����ָ��*/
	bool insertRecord(string IndexName, string key, int ptr) {
		BPlusTree bpt;
		bpt.OpenIndex(IndexName);
		return bpt.insertRecord(key, ptr);
	}

	/*ɾ��һ����¼*/
	bool deleteRecord(string IndexName, string key) {
		BPlusTree bpt;
		bpt.OpenIndex(IndexName);
		return bpt.deleteRecord(key);
	}

	/*����һ����¼��ָ�룬����-1��ʾ��¼������*/
	int find(string IndexName, string key) {
		BPlusTree bpt;
		bpt.OpenIndex(IndexName);
		return bpt.find(key);
	}

	/*������ң�������Сֵ���ֵ��һ����list�����������ڵ�ֵ�������䣬���ܰ����������С�������߲�ѯ�뽫��һ�߽߱���Ϊ���ַ�����""��*/
	void findBetween(string IndexName, string min, string max, list<int>& results) {
		BPlusTree bpt;
		bpt.OpenIndex(IndexName);
		bpt.findBetween(min, max, results);
		return;
	}

	/*ɾ������*/
	void dropIndex(string IndexName) {
		Buf.dropFile((IndexName + ".index"));
	}
};

