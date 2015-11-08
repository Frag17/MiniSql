#ifndef _Buffer_Manager_
#define _Buffer_Manager_

#define MAX_BLOCK_AMOUNT 10000
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

	//���ļ��ж�һ���鵽������
	void readFile(const std::string& fileName, int blockOffset);

	//���ļ���д��,isDirty=0
	void writeFile();

	void initialize()
	{
		isPinned=0;isDirty=0;fileName="";blockOffset=-1;
		for (int i = 0; i <BLOCK_SIZE; i++)
			content[i] = 0;
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
	
	void changeContent(int start, const std::string& c);

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
	~BufferManager()
	{
		std::list<Block*>::iterator it;
		for (it = fullBuffer.begin();it!=fullBuffer.end(); it++)
		{
			(*(*it)).writeFile();
			delete *it;
		}
		for (it = emptyBuffer.begin(); it != emptyBuffer.end(); it++)
		{
			delete *it;
		}
	}

	//�������ļ������ݶ���������
	void readFile(const std::string& fileName);

	//���ļ���һ��block�����ݣ���blcokoffset��λ�����������С�����һ��ָ���block���ڻ����ָ��
	std::list<Block*>::iterator readFileBlock(const std::string& fileName, int blockOffset);

	//�ѻ������������ڵ�д��Ӳ��
	void writeFile(const std::string& fileName);
	//�ѻ�����һ��block������д��Ӳ����
	void writeFileBlock(const std::string& fileName, int blockOffset);

	//��û�пջ����ˣ������滻
	std::list<Block*>::iterator replace(const std::string& fileName, int blockOffset);//ʹ��LRU�㷨
	//�жϻ����Ƿ�Ϊ��
	bool isFull();
	//�ڻ�������ĳ���ļ���ĳ��block����û�У������ļ���ȡ�������С�Ȼ�󷵻�һ��ָ�����ڻ����ָ��
	std::list<Block*>::iterator findBlockInBuffer(const std::string fileName, int blockOffset);//�õ�ָ����block��buffer�е�iterator

	void setPin();//��˵
	void clearPin();//��˵

	//�õ��ļ���ĳһ����ĳ�������ݡ����ظ�������ɵ��ַ���
	std::string getValue(const std::string& fileName, int blockOffset, int start, int end);
	//std::string getValue(std::string& fileName, int blockOffset,int position);
	//insertValue.����


	//�ı��ļ��е�ֵ��start����block����Ҫ�ı����ʼλ�ã��ַ�����Ҫ�ı�ɵ�����
	void changeValue(const std::string& fileName, int blockOffset, int start, std::string& c);

	//���ļ�ɾ�ˣ�free�������й��ڸ��ļ��Ŀ飬����Ҫд�ļ�
	void freeWithoutWrite(const std::string& fileName);//����drop���free

	//����ĳ�ļ��е����ݣ���free�������й��ڸ��ļ��Ŀ飬��Ҫд�ļ�
	void free(const std::string& fileName);//���ڲ��ñ��free


	void deleteRecord(const std::string& fileName, int blockOffset, int start);//��recordManager
	
	//ɾ��һ��block���ڲ�ʵ�֣�����block������д0
	void deleteBlock(const std::string &fileName, int blockOffset);//��indexManager

	int insertRecord(const std::string& fileName, const std::string& content);//��recordManager
	
	//����һ��block���ڲ�ʵ�֣����ļ�����֮ǰ��ɾ���Ŀ�λ������롣��û�У�����뵽�ļ�ĩβ��
	//���ļ�ĩβû�п��õĿ�λ�����ļ�ĩβ���ӿ�λ��Ȼ������ַ�������
	//����������Ҫ������ļ������ַ������ݣ���index��Ϊһ��block���ȣ���
	//����catalog�ĸ��¸ú�����ʵ�֣���˲���ҪindexManager��recordMAnager��ʵ��
	int insertBlock(const std::string &fileName, const std::string& content);//��indexManager

	//���ļ�ĩβ���µĿհ׿�
	std::list<Block*>::iterator addBlockInFile(const std::string& fileName);

	//�����ļ�
	void createFile(const std::string& fileName);
	
	//ɾ���ļ�
	void dropFile(const std::string& fileName);
	//�ر��ļ�����ʱ�����ļ����������и��ļ��������ͷţ�
	void closeFile(const std::string & fileName);

	std::list<Block*> fullBuffer;
	std::list<Block*> emptyBuffer;

};



#endif