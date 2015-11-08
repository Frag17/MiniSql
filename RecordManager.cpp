#include"StdAfx.h"

extern BufferManager Buf;
extern CatalogManager Cat;
extern RecordManager Rec;

void RecordManager::createTable(Table &table)
{
	std::string fileName = table.tableName + ".table";
	Buf.createFile(fileName);
}
void RecordManager::dropTable(Table &table)
{
	std::string fileName = table.tableName + ".table";
	Buf.dropFile(fileName);
}
void RecordManager::closeTable(Table & table)
{
	std::string fileName = table.tableName + ".table";
	Buf.closeFile(fileName);
}

int RecordManager::insertRecord(Table& table, Tuple& tuple)
{
	std::string fileName = table.tableName + ".table";
	std::string record = tupleToRecord(tuple);
	int ptr = Buf.insertRecord(fileName,record);
	return ptr;
}

void RecordManager::deleteTable(Table& table)
{
	std::string fileName = table.tableName + ".table";
	std::fstream fileOut(fileName, std::ios::out);
	fileOut.close();
	table.emptyRecordOffset.clear();
	table.blockNum = 0;
}
void RecordManager::deleteByPrimaryKey(Table& table, std::vector<std::string>& PrimaryKey)
{
	std::string fileName = table.tableName + ".table";
	int num;
	for (num = 0; num < table.attributes.size(); num++)
		if (table.attributes[num].isPrimaryKey == 1)
			break;

	int start = 0; int i = 0;
	for (int blockOffset = 0; blockOffset < table.blockNum; blockOffset++)
	{
		start = start%BLOCK_SIZE;
		while (start<BLOCK_SIZE)
		{
			std::string content = Buf.getRecord(fileName, blockOffset, start);
			if (content[0] != '\0')
			{
				Tuple tuple = recordToTuple(table,content);
				if (tuple[num] == PrimaryKey[i])
				{
					Buf.deleteRecord(fileName, blockOffset, start);
					i++;
					if (i == PrimaryKey.size())
						return;
					
				}
			}
			start += table.recordSize;
		}
	}
}

void RecordManager::selectRecord(Table& table,Data &data)
{
	std::vector<std::string> tempData;
	std::string fileName = table.tableName + ".table";

	int start = 0;
	for (int blockOffset = 0; blockOffset < table.blockNum; blockOffset++)
	{
		start = start%BLOCK_SIZE;
		while (start<BLOCK_SIZE)
		{
			if (blockOffset + 1 == table.blockNum && start + table.recordSize > BLOCK_SIZE)//已经在最后一个block，并且所在record跨block，则必为空
				break;
			std::string record = Buf.getRecord(fileName, blockOffset, start);
			if (record[0] != '\0')
				tempData.push_back(record);
			start += table.recordSize;
		}
	}

	for (int i = 0; i < (int)(tempData.size()); i++)
	{
		Tuple tuple = recordToTuple(table, tempData[i]);
		data.push_back(tuple);
	}

}
void RecordManager::selectByPointer(Table& table, std::vector<int>& Pointers, Data& data)
{
	std::vector<std::string> tempData;
	std::string fileName = table.tableName + ".table";

	for (int i = 0; i < (int)Pointers.size(); i++)
	{
		int blockOffset = Pointers[i] >> 16;
		int start = Pointers[i] & 0x00FF;
		std::string record = Buf.getRecord(fileName,blockOffset,start);
		tempData.push_back(record);
	}

	for (int i = 0; i < (int)(tempData.size()); i++)
	{
		Tuple tuple = recordToTuple(table, tempData[i]);
		data.push_back(tuple);
	}
}

void RecordManager::selectAttribute(Table& table, int attributeOrder, SingleData& data, std::vector<int> &ptrs)
{
	std::vector<std::string> tempData;
	std::string fileName = table.tableName + ".table";

	int start = 0;
	for (int blockOffset = 0; blockOffset < table.blockNum; blockOffset++)
	{
		start = start%BLOCK_SIZE;
		while (start<BLOCK_SIZE)
		{
			if (blockOffset + 1 == table.blockNum && start + table.recordSize > BLOCK_SIZE)//已经在最后一个block，并且所在record跨block，则必为空
				break;
			std::string record = Buf.getRecord(fileName, blockOffset, start);
			if (record[0] != '\0')
			{
				tempData.push_back(record);
				int ptr = (blockOffset << 16) | start;
				ptrs.push_back(ptr);
			}
			start += table.recordSize;
		}
	}

	for (int i = 0; i < (int)(tempData.size()); i++)
	{
		Tuple tuple = recordToTuple(table, tempData[i]);
		data.push_back(tuple[attributeOrder]);
	}
	

}

Tuple RecordManager::recordToTuple(Table & table, std::string record)
{
	int recordPtr = 1;
	Tuple tuple;
	std::vector<Attribute>::iterator it_attribute;
	for (it_attribute = table.attributes.begin(); it_attribute != table.attributes.end(); it_attribute++)
	{
		int attributeSize;
		Attribute attribute = *it_attribute;
		attributeSize = attribute.length;
		std::string content = record.substr(recordPtr,attributeSize);
		recordPtr += attributeSize;
		tuple.push_back(content);
	}
	return tuple;
	

}
std::string RecordManager::tupleToRecord(Tuple & tuple)
{
	std::string record = "";
	record += '\1';
	for (int i = 0; i < (int)(tuple.size()); i++)
		record += tuple[i];
	return record;
}

