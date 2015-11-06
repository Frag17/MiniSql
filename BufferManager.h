#ifndef _Buffer_Manager_
#define _Buffer_Manager_

#define MAX_BLOCK_AMOUNT 100
#define BLOCK_SIZE 4096
#include<string>
#include<list>
#include<map>
#include<vector>
#include"CatalogManager.h"

extern CatalogManager catalog;

class Block
{
public:
	Block()
	{
		initialize();
	}
	~Block(){};

	//���ļ��ж�һ���鵽������. isEmpty=0
	void readFile(std::string& fileName, int blockOffset);

	//���ļ���д��,isDirty=0
	void writeFile();

	void initialize()
	{
		isPinned=0;isDirty=0;fileName="";blockOffset=0;
		for (int i = 0; i < BLOCK_SIZE; i++)
			content[BLOCK_SIZE] = 0;
	}

	void setPin()
	{
		isPinned = 1;return;
	}
	void clearPin()
	{
		isPinned = 0;
	}

	std::string getContent(int start,int end);
	//std::string getContent(int position);
	
	void changeContent(int start, std::string& c);

	bool isPinned;
	bool isDirty;
	//isDirty  ������������Ƿ���ͬ����ʼֵΪ0�������޸Ĺ�������Ϊ1
	//����д�ص������ϣ�����Ϊ0
	std::string fileName;
	int blockOffset;
	char content[BLOCK_SIZE];
};



class BufferManager
{
public:

	BufferManager()
	{
		for (int i = 0; i < MAX_BLOCK_AMOUNT; i++)
			emptyBuffer.push_back(new Block());
	}
	~BufferManager(){}

	//�������ļ������ݶ���������
	void readFile(std::string& fileName);

	//���ļ���һ��block�����ݣ���blcokoffset��λ�����������С�����һ��ָ���block���ڻ����ָ��
	std::list<Block*>::iterator readFileBlock(std::string& fileName, int blockOffset);

	//�ѻ������������ڵ�д��Ӳ��
	void writeFile(std::string& fileName);
	//�ѻ�����һ��block������д��Ӳ����
	void writeFileBlock(std::string& fileName, int blockOffset);

	//��û�пջ����ˣ������滻
	std::list<Block*>::iterator replace(std::string& fileName, int blockOffset);//ʹ��LRU�㷨
	//�жϻ����Ƿ�Ϊ��
	bool isFull();
	//�ڻ�������ĳ���ļ���ĳ��block����û�У������ļ���ȡ�������С�Ȼ�󷵻�һ��ָ�����ڻ����ָ��
	std::list<Block*>::iterator findBlockInBuffer(std::string fileName, int blockOffset);//�õ�ָ����block��buffer�е�iterator

	void setPin();//��˵
	void clearPin();//��˵

	//�õ��ļ���ĳһ����ĳ�������ݡ����ظ�������ɵ��ַ���
	std::string getValue(std::string& fileName, int blockOffset, int start, int end);
	//std::string getValue(std::string& fileName, int blockOffset,int position);
	//insertValue.����


	//�ı��ļ��е�ֵ��start����block����Ҫ�ı����ʼλ�ã��ַ�����Ҫ�ı�ɵ�����
	void changeValue(std::string& fileName, int blockOffset, int start, std::string& c);

	//���ļ�ɾ�ˣ�free�������й��ڸ��ļ��Ŀ飬����Ҫд�ļ�
	void freeWithoutWrite(std::string& fileName);//����drop���free

	//����ĳ�ļ��е����ݣ���free�������й��ڸ��ļ��Ŀ飬��Ҫд�ļ�
	void free(std::string& fileName);//���ڲ��ñ��free


	void deleteRecord(std::string& fileName, int blockOffset, int start);//��recordManager
	
	//ɾ��һ��block���ڲ�ʵ�֣�����block������д0
	void deleteBlock(std::string &fileName, int blockOffset);//��indexManager

	void insertRecord(std::string& fileName, std::string& content);//��recordManager
	
	//����һ��block���ڲ�ʵ�֣����ļ�����֮ǰ��ɾ���Ŀ�λ������롣��û�У�����뵽�ļ�ĩβ��
	//���ļ�ĩβû�п��õĿ�λ�����ļ�ĩβ���ӿ�λ��Ȼ������ַ�������
	//����������Ҫ������ļ������ַ������ݣ���index��Ϊһ��block���ȣ���
	//����catalog�ĸ��¸ú�����ʵ�֣���˲���ҪindexManager��recordMAnager��ʵ��
	int insertBlock(std::string &fileName,  std::string& content);//��indexManager

	//���ļ�ĩβ���µĿհ׿�
	std::list<Block*>::iterator addBlockInFile(std::string& fileName);

	//�����ļ�
	void createFile(std::string& fileName);
	
	//ɾ���ļ�
	void dropFile(std::string& fileName);
	//�ر��ļ�����ʱ�����ļ����������и��ļ��������ͷţ�
	void closeFile(std::string & fileName);

	std::list<Block*> fullBuffer;
	std::list<Block*> emptyBuffer;

};



#endif