#include"BufferManager.h"
#include<fstream>



void Block::readFile(const std::string& fileName, int blockOffset)
{
	initialize();

	std::fstream fileIn(fileName.c_str(), std::ios::in);
	fileIn.seekg(blockOffset*BLOCK_SIZE, std::ios::beg);
	fileIn.read(content, BLOCK_SIZE);
	fileIn.close();
	isPinned = 0;
	isDirty = 0;
	this->fileName = fileName;
	this->blockOffset = blockOffset;
}

void Block::writeFile()
{
	if (isDirty == 1)
	{
		std::fstream fileOut(fileName, std::fstream::out|std::fstream::in);
		fileOut.seekp(BLOCK_SIZE*blockOffset, fileOut.beg);
		fileOut.write(content, BLOCK_SIZE);
		fileOut.close();
	}
	isDirty = 0;
}

std::string Block::getContent(int start, int end)
{
	std::string res = "";
	if (start>=0 && end<BLOCK_SIZE && start <= end)
		for (int i = start; i < end; i++)
			res += content[i];
	return res;
}
/*			是否需要？？
std::string Block::getContent(int position)
{
	std::string res = "";
	if (position >= 0 && position < BLOCK_SIZE)
		res += content[position];
	return res;
}
*/
void Block::changeContent(int start, const std::string &c)
{
	if (start >= 0 && start < BLOCK_SIZE)
		for (int i = 0; i < (int)(c.size()); i++)
			content[start + i] = c.at(i);
	isDirty = 1;
}

std::list<Block*>::iterator BufferManager::readFileBlock(const std::string& fileName, int blockOffset)
{
	std::list<Block*>::iterator it = findBlockInBuffer(fileName, blockOffset);
	if (it != fullBuffer.end())
	{
		fullBuffer.push_front((*it));
		fullBuffer.erase(it);
		return fullBuffer.begin();
	}
	else
		if (isFull())
			return replace(fileName, blockOffset);
		else
		{
			(*(emptyBuffer.begin()))->readFile(fileName, blockOffset);
			fullBuffer.push_front(*(emptyBuffer.begin()));
			emptyBuffer.pop_front();
			return fullBuffer.begin();
		}
	
}
void BufferManager::readFile(const std::string& fileName)
{
	int blockNum; int n;
	if ((n = fileName.find(".table", 0)) != std::string::npos)
	{
		Table* tablePtr = Cat.getTablePtr(fileName.substr(0,n));
		blockNum = tablePtr->blockNum;
	}
	if ((n = fileName.find(".index", 0)) != std::string::npos)
	{
		Index* indexPtr = Cat.getIndexPtr(fileName.substr(0, n));
		blockNum = indexPtr->blockNum;
	}
	std::list<Block*>::iterator it;
	for (int i = 0; i < blockNum; i++)
		it = readFileBlock(fileName, i);
}

void BufferManager::writeFileBlock(const std::string& fileName, int blockOffset)
{
	std::list<Block*>::iterator it = findBlockInBuffer(fileName, blockOffset);
	if (it != fullBuffer.end())
	{
		if ((*it)->isDirty == 1)
			(*it)->writeFile();
		fullBuffer.push_front((*it));
		fullBuffer.erase(it);
	}
	
}
void BufferManager::writeFile(const std::string& fileName)
{
	int blockNum; int n;
	if ((n = fileName.find(".table", 0)) != std::string::npos)
	{
		Table* tablePtr = Cat.getTablePtr(fileName.substr(0, n));
		blockNum = tablePtr->blockNum;
	}
	if ((n = fileName.find(".index", 0)) != std::string::npos)
	{
		Index* indexPtr = Cat.getIndexPtr(fileName.substr(0, n));
		blockNum = indexPtr->blockNum;
	}

	for (int i = 0; i < blockNum; i++)
		writeFileBlock(fileName, i);
}

std::list<Block*>::iterator  BufferManager::replace(const std::string& fileName, int blockOffset)
{
	std::list<Block*>::iterator it = fullBuffer.end();		it--;
	while ((*it)->isPinned == 1)  it--;

	(*it)->readFile(fileName, blockOffset);
	fullBuffer.push_front(*it);
	fullBuffer.erase(it);
	return fullBuffer.begin();
}

bool BufferManager::isFull()
{
	if (emptyBuffer.empty())
		return 1;
	else
		return 0;
}

std::list<Block*>::iterator BufferManager::findBlockInBuffer(const std::string fileName, int blockOffset)
{
	std::list<Block*>::iterator it;
	for (it = fullBuffer.begin(); it != fullBuffer.end(); it++)
		if ((*it)->fileName == fileName && (*it)->blockOffset == blockOffset)
			return it;
	return it;
}

std::string BufferManager::getRecord(const std::string& fileName, int blockOffset, int start)
{
	int n = fileName.find(".table",0);
	Table* tablePtr = Cat.getTablePtr(fileName.substr(0, n));
	
	std::list<Block*>::iterator it = readFileBlock(fileName, blockOffset);
	std::string record = "";
	if (start + tablePtr->recordSize > BLOCK_SIZE)
	{
		int length1 = BLOCK_SIZE - start;
		int length2 = tablePtr->recordSize - length1;
		for (int i = 0; i < length1; i++)
			record += (*it)->content[start + i];
		it = readFileBlock(fileName, blockOffset+1);
		for (int i = 0; i < length2; i++)
			record += (*it)->content[i];
	}
	else
		for (int i = 0; i < tablePtr->recordSize; i++)
			record += (*it)->content[i+start];
	return record;
}
std::stristd::string BufferManager::getBlock(const std::string& fileName, int blockOffset,int start,int end)
{
	int n = fileName.find(".index", 0);
	Index *indexPtr = Cat.getIndexPtr(fileName.substr(0, n));

	std::list<Block*>::iterator it = readFileBlock(fileName, blockOffset);
	std::string record = "";
	for (int i = start; i < end; i++)
		record += (*it)->content[i];
	return record;
}

void BufferManager::changeValue(const std::string& fileName, int blockOffset, int start, std::string& c)
{
	std::list<Block*>::iterator it = findBlockInBuffer(fileName, blockOffset);
	if (it != fullBuffer.end())
	{
		(*it)->changeContent(start, c);
		fullBuffer.push_front((*it));
		fullBuffer.erase(it);
	}
	else
	{
		std::list<Block*>::iterator it = readFileBlock(fileName, blockOffset);
		(*it)->changeContent(start, c);
	}
}

void BufferManager::freeWithoutWrite(const std::string& fileName)
{
	std::list<Block*>::iterator it = fullBuffer.begin();
	//for (it = fullBuffer.begin(); it != fullBuffer.end(); it++
	while (it != fullBuffer.end())
		if ((*it)->fileName == fileName)
		{
			std::list<Block*>::iterator it1 = it;
			it++;
			(*it1)->initialize();
			emptyBuffer.push_back(*it1);
			fullBuffer.erase(it1);
		}
		else
			it++;
}
void BufferManager::free(const std::string& fileName)
{
	std::list<Block*>::iterator it = fullBuffer.begin();
	while (it != fullBuffer.end())
		if ((*it)->fileName == fileName)
		{
			std::list<Block*>::iterator it1 = it;
			it++;
			if ((*it1)->isDirty == 1)
				(*it1)->writeFile();
			(*it1)->initialize();
			emptyBuffer.push_back(*it1);
			fullBuffer.erase(it1);
		}
		else
			it++;
}

void BufferManager::createFile(const std::string& fileName)
{
	std::fstream  fout(fileName, std::ios::out);
	fout.close();
}
void BufferManager::dropFile(const std::string& fileName)
{
	if (remove(fileName.c_str()) != 0)
		perror("Error deleting file");
	else
		freeWithoutWrite(fileName);
}
void BufferManager::closeFile(const std::string & fileName)
{
	free(fileName);
}

void BufferManager::deleteRecord(const std::string& fileName, int blockOffset, int start)//给recordManager
{
	int n = fileName.find(".table",0);
	std::string tableName = fileName.substr(0, n);

	Table* tablePtr = Cat.getTablePtr(tableName);
	std::string ss = "";
	for (int i = 0; i < tablePtr->recordSize; i++)
		ss += '\0';
	std::list<Block*>::iterator it = readFileBlock(fileName, blockOffset);
	
	if (start + tablePtr->recordSize>BLOCK_SIZE)
	{
		int length1 = BLOCK_SIZE - start;
		int length2 = tablePtr->recordSize - length1;
		(*it)->changeContent(start,ss.substr(0,length1));
		it = readFileBlock(fileName, blockOffset+1);
		(*it)->changeContent(0,ss.substr(0,length2));
	}
	else
		(*it)->changeContent(start, ss);

	int recordOffset = (blockOffset*BLOCK_SIZE + start) / tablePtr->recordSize;
	tablePtr->emptyRecordOffset.push_back(recordOffset);
}

void BufferManager::deleteBlock(const std::string &fileName, int blockOffset)//给indexManager
{
	int n = fileName.find(".index", 0);
	std::string indexName = fileName.substr(0, n);

	std::vector<Index*>::iterator indexPtr = Cat.findIndex(indexName);
	std::string ss = "";
	for (int i = 0; i < BLOCK_SIZE; i++)
		ss += '\0';
	std::list<Block*>::iterator it = findBlockInBuffer(fileName, blockOffset);
	if (it != fullBuffer.end())
	{
		(*it)->changeContent(0, ss);
		fullBuffer.push_front((*it));
		fullBuffer.erase(it);
	}
	else
	{
		it = readFileBlock(fileName, blockOffset);
		(*it)->changeContent(0, ss);
	}
	(*indexPtr)->emptyBlockOffset.push_back(blockOffset);
}

int BufferManager::insertRecord(const std::string& fileName, const std::string& content)
{
	int n = fileName.find(".table", 0);
	std::string tableName = fileName.substr(0, fileName.size()-6);
	Table* tablePtr = Cat.getTablePtr(tableName);

	std::list<Block*>::iterator it;

	if (tablePtr->emptyRecordOffset.empty() == 1)//没有空位
	{
		it = addBlockInFile(fileName);
		(*it)->blockOffset = tablePtr->blockNum;
		(*it)->fileName = fileName;
		(*it)->isDirty = 1;
		tablePtr->blockNum++;
		(*it)->changeContent(0, content);
		int recordOffset = ((tablePtr->blockNum - 1)*BLOCK_SIZE )/ tablePtr->recordSize;
		int num;
		if (BLOCK_SIZE % tablePtr->recordSize == 0)
			num = BLOCK_SIZE / tablePtr->recordSize;
		else
			num = BLOCK_SIZE / tablePtr->recordSize + 1;

		for (int i = 1; i < num; i++)
			tablePtr->emptyRecordOffset.push_back(recordOffset + i);
		
		return (*it)->blockOffset << 16;
	}
	else//有空位
	{
		int recordOffset = *((tablePtr->emptyRecordOffset).begin());
		(tablePtr->emptyRecordOffset).erase((tablePtr->emptyRecordOffset).begin());

		int blockOffset = (recordOffset*tablePtr->recordSize) / BLOCK_SIZE;
		int start = (recordOffset*tablePtr->recordSize) % BLOCK_SIZE;
		it = readFileBlock(fileName, blockOffset);
		if (start + tablePtr->recordSize > BLOCK_SIZE)//记录跨block
		{
			int length1 = BLOCK_SIZE - start;
			int length2 = tablePtr->recordSize - length1;
			(*it)->changeContent(start, content.substr(0, length1));
			if (blockOffset + 1 == tablePtr->blockNum)//所跨block的下一个需要生成
			{
				it = addBlockInFile(fileName);
				(*it)->blockOffset = tablePtr->blockNum;
				(*it)->fileName = fileName;
				(*it)->isDirty = 1;
				tablePtr->blockNum++;
				(*it)->changeContent(0, content.substr(length1, length2));
				int i = recordOffset+1;
				int size = length2;
				while (size < BLOCK_SIZE)
				{
					tablePtr->emptyRecordOffset.push_back(i);
					size += tablePtr->recordSize;
					i++;
				}
			}
			else//所跨block的下一个不需要生成
			{
				it = readFileBlock(fileName,blockOffset+1);
				(*it)->changeContent(0, content.substr(length1, length2));
			}
			
		}
		else//记录不跨block
			(*it)->changeContent(start, content); 

		return (blockOffset << 16) | start;
			
	}
}
int BufferManager::insertBlock(const std::string &fileName, const std::string& content)
{
	int n = fileName.find(".index", 0);
	std::string indexName = fileName.substr(0, n);
	Index* indexPtr = Cat.getIndexPtr(indexName);

	std::list<Block*>::iterator it;

	if (indexPtr->emptyBlockOffset.empty() == 1)
	{
		it = addBlockInFile(fileName);
		(*it)->blockOffset = indexPtr->blockNum;
		(*it)->fileName = fileName;
		(*it)->isDirty = 1;
		indexPtr->blockNum++;
		(*it)->changeContent(0, content);
	}
	else
	{
		int blockOffset = indexPtr->emptyBlockOffset.back();
		indexPtr->emptyBlockOffset.pop_back();
		it = readFileBlock(fileName, blockOffset);
		(*it)->changeContent(0, content);
	}
	return (*it)->blockOffset;
}

std::list<Block*>::iterator BufferManager::addBlockInFile(const std::string& fileName)
{
	
	std::list<Block*>::iterator it;
	if (isFull())
	{
		it = fullBuffer.end();		it--;
		while ((*it)->isPinned == 1)  it--;
		fullBuffer.push_front(*it);
		(*it)->writeFile();
		fullBuffer.erase(it);
	}
	else
	{
		it = emptyBuffer.begin();
		fullBuffer.push_front(*it);
		emptyBuffer.pop_front();
	}
	it = fullBuffer.begin();
	(*it)->initialize();
	return it;
}
