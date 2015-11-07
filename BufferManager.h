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

	//从文件中读一个块到缓存中
	void readFile(const std::string& fileName, int blockOffset);

	//向文件中写块,isDirty=0
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
	//isDirty  与磁盘中数据是否相同。初始值为0，若被修改过，则置为1
	//若被写回到磁盘上，则置为0
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

	//把整个文件的内容读到缓存中
	void readFile(const std::string& fileName);

	//把文件中一个block的内容（由blcokoffset定位）读到缓存中。返回一个指向该block所在缓存的指针
	std::list<Block*>::iterator readFileBlock(const std::string& fileName, int blockOffset);

	//把缓存中所有属于的写入硬盘
	void writeFile(const std::string& fileName);
	//把缓存中一个block的内容写到硬盘中
	void writeFileBlock(const std::string& fileName, int blockOffset);

	//当没有空缓存了，则用替换
	std::list<Block*>::iterator replace(const std::string& fileName, int blockOffset);//使用LRU算法
	//判断缓存是否为满
	bool isFull();
	//在缓存中找某个文件的某个block。若没有，则向文件读取到缓存中。然后返回一个指向所在缓存的指针
	std::list<Block*>::iterator findBlockInBuffer(const std::string fileName, int blockOffset);//得到指定的block在buffer中的iterator

	void setPin();//再说
	void clearPin();//再说

	//得到文件中某一块中某处的内容。返回该内容组成的字符串
	std::string getValue(const std::string& fileName, int blockOffset, int start, int end);
	//std::string getValue(std::string& fileName, int blockOffset,int position);
	//insertValue.加入


	//改变文件中的值。start是在block中想要改变的起始位置，字符串是要改变成的内容
	void changeValue(const std::string& fileName, int blockOffset, int start, std::string& c);

	//把文件删了，free掉缓存中关于该文件的块，不需要写文件
	void freeWithoutWrite(const std::string& fileName);//由于drop表而free

	//不用某文件中的内容，则free掉缓存中关于该文件的块，需要写文件
	void free(const std::string& fileName);//由于不用表而free


	void deleteRecord(const std::string& fileName, int blockOffset, int start);//给recordManager
	
	//删除一个block。内部实现：将该block的内容写0
	void deleteBlock(const std::string &fileName, int blockOffset);//给indexManager

	int insertRecord(const std::string& fileName, const std::string& content);//给recordManager
	
	//插入一个block。内部实现：若文件中有之前被删除的空位，则加入。若没有，则加入到文件末尾。
	//若文件末尾没有可用的空位，则文件末尾增加空位，然后加入字符窜内容
	//参数：给出要插入的文件名和字符窜内容（在index中为一个block长度）。
	//对于catalog的更新该函数已实现，因此不需要indexManager和recordMAnager来实现
	int insertBlock(const std::string &fileName, const std::string& content);//给indexManager

	//在文件末尾加新的空白块
	std::list<Block*>::iterator addBlockInFile(const std::string& fileName);

	//创建文件
	void createFile(const std::string& fileName);
	
	//删除文件
	void dropFile(const std::string& fileName);
	//关闭文件（暂时不用文件，将缓存中该文件的内容释放）
	void closeFile(const std::string & fileName);

	std::list<Block*> fullBuffer;
	std::list<Block*> emptyBuffer;

};



#endif