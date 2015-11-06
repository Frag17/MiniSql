#include"CatalogManager.h"
#include<string>
#include<fstream>

CatalogManager::CatalogManager()
{
	readCatalog();
}
CatalogManager::~CatalogManager()
{
	writeCatalog();

	std::vector<Table*>::iterator it_table;
	for (it_table = tables.begin(); it_table != tables.end();it_table++)
		delete *it_table;

	std::vector<Index*>::iterator it_index;
	for (it_index = indexs.begin(); it_index != indexs.end(); it_index++)
		delete *it_index;
}

std::vector<Table*>::iterator CatalogManager::findTable(std::string& tableName)
{
	std::vector<Table*>::iterator it;
	for (it = tables.begin(); it != tables.end(); it++)
	{
		if ((*it)->tableName == tableName)
			return it;
	}
}
std::vector<Index*>::iterator CatalogManager::findIndex(std::string& indexName)
{
	std::vector<Index*>::iterator it;
	for (it = indexs.begin(); it != indexs.end(); it++)
	{
		if ((*it)->tableName == indexName)
			return it;
	}
}

void CatalogManager::readCatalog()
{
	std::string filename = "table.catlog";
	std::fstream  fileIn(filename, std::ios::in);
	int tableNum;
	fileIn >> tableNum;
	for (int i = 0; i < tableNum; i++)
	{
		//table
		Table *tablePtr = new Table();
		fileIn >> tablePtr->tableName;
		int attributeNum;
		fileIn >>attributeNum;
		for (int j = 0; j < attributeNum; j++)
		{
			Attribute attribute;
			fileIn >>  attribute.name;
			fileIn >> attribute.type;
			fileIn >> attribute.length;
			fileIn >> attribute.isUnique;
			fileIn >> attribute.isPrimaryKey;
			tablePtr->attributes.push_back(attribute);
		}
		fileIn >> tablePtr->blockNum;
		fileIn >> tablePtr->recordSize;
		int emptyRecordOffsetNum;
		fileIn >> emptyRecordOffsetNum;
		for (int j = 0; j < emptyRecordOffsetNum; j++)
		{
			int recordOffset;
			fileIn >> recordOffset;
			tablePtr->emptyRecordOffset.push_back(recordOffset);
		}
		tables.push_back(tablePtr);
	}
	fileIn.close();
	//index
	filename = "index.catlog";
	fileIn.open(filename, std::ios::in);
	int indexNum;
	fileIn >> indexNum;
	for (int i = 0; i < indexNum; i++)
	{
		Index *indexPtr = new Index();
		fileIn >> indexPtr->indexName;
		fileIn >> indexPtr->tableName;
		fileIn >> indexPtr->attribute;
		fileIn >> indexPtr->blockNum;
		int emptyBlockOffsetNum;
		fileIn >> emptyBlockOffsetNum;
		for (int j = 0; j < emptyBlockOffsetNum; j++)
		{
			int blockOffset;
			fileIn >> blockOffset;
			indexPtr->emptyBlockOffset.push_back(blockOffset);
		}
		indexs.push_back(indexPtr);
	}
	fileIn.close();
}
void CatalogManager::writeCatalog()
{
	//table
	std::string filename = "table.catlog";
	std::fstream  fileOut(filename, std::ios::out);
	fileOut << tables.size();
	std::vector<Table*>::iterator it_table;
	for (it_table = tables.begin(); it_table != tables.end();it_table++)
	{
		Table *tablePtr = *it_table;
		fileOut << tablePtr->tableName;
		fileOut << tablePtr->attributes.size();
		std::vector<Attribute>::iterator it_attribute;
		for (it_attribute = tablePtr->attributes.begin(); it_attribute != tablePtr->attributes.end();it_attribute++)
		{
			Attribute attribute = *it_attribute;
			fileOut << attribute.name;
			fileOut << attribute.type;
			fileOut << attribute.length;
			fileOut << attribute.isUnique;
			fileOut << attribute.isPrimaryKey;
		}
		fileOut << tablePtr->blockNum;
		fileOut << tablePtr->recordSize;
		fileOut << tablePtr->emptyRecordOffset.size();
		std::vector<int>::iterator it_emptyRecordOffset;
		for (it_emptyRecordOffset = tablePtr->emptyRecordOffset.begin(); 
			it_emptyRecordOffset != tablePtr->emptyRecordOffset.end();it_emptyRecordOffset++)
		{
			int recordOffset = *it_emptyRecordOffset;
			fileOut << recordOffset;
		}
	}
	fileOut.close();

	//index
	filename = "index.catlog";
	fileOut.open(filename, std::ios::out);
	fileOut << indexs.size();
	std::vector<Index*>::iterator it_index;
	for (it_index = indexs.begin(); it_index != indexs.end();it_index++)
	{
		Index *indexPtr = *it_index;
		fileOut << indexPtr->indexName;
		fileOut << indexPtr->tableName;
		fileOut << indexPtr->attribute;
		fileOut << indexPtr->blockNum;
		fileOut <<  indexPtr->emptyBlockOffset.size();
		std::vector<int>::iterator it_emptyBlockOffset;
		for (it_emptyBlockOffset = indexPtr->emptyBlockOffset.begin();
			it_emptyBlockOffset != indexPtr->emptyBlockOffset.end();it_emptyBlockOffset++)
		{
			int blockOffset = *it_emptyBlockOffset;
			fileOut << blockOffset;
		}
	}
	fileOut.close();
}

void CatalogManager::createTable(Table *table)
{
	Table *tablePtr = new Table();
	tablePtr->tableName = table->tableName;
	tablePtr->attributes = table->attributes;
	tablePtr->blockNum = table->blockNum;
	tablePtr->recordSize = table->recordSize;
	tablePtr->emptyRecordOffset = table->emptyRecordOffset;
	tables.push_back(tablePtr);
}
void CatalogManager::createIndex(Index *index)
{
	Index *indexPtr = new Index();
	indexPtr->indexName = index->indexName;
	indexPtr->tableName = index->indexName;
	indexPtr->attribute = index->attribute;
	indexPtr->blockNum = index->blockNum;
	indexPtr->emptyBlockOffset = index->emptyBlockOffset;
	indexs.push_back(indexPtr);
}

void CatalogManager::dropTable(std::string& tableName)
{
	std::vector<Table*>::iterator it_table;
	for (it_table = tables.begin(); it_table != tables.end(); it_table++)
		if ((*it_table)->tableName == tableName)
		{
			delete (*it_table);
			tables.erase(it_table);
			break;
		}

	std::vector<Index*>::iterator it_index;
	for (it_index = indexs.begin(); it_index != indexs.end(); it_index++)
		if ((*it_index)->tableName == tableName)
		{
			delete (*it_index);
			indexs.erase(it_index);
		}
}
void CatalogManager::dropIndex(std::string& indexName)
{
	std::vector<Index*>::iterator it_index;
	for (it_index = indexs.begin(); it_index != indexs.end(); it_index++)
		if ((*it_index)->indexName == indexName)
		{
			delete (*it_index);
			indexs.erase(it_index);
			break;
		}
}

bool CatalogManager::existTable(std::string& tableName)
{
	std::vector<Table*>::iterator it_table;
	for (it_table = tables.begin(); it_table != tables.end(); it_table++)
		if ((*it_table)->tableName == tableName)
			return 1;
	return 0;
}
bool CatalogManager::existIndex(std::string& indexName)
{
	std::vector<Index*>::iterator it_index;
	for (it_index = indexs.begin(); it_index != indexs.end(); it_index++)
		if ((*it_index)->indexName == indexName)
			return 1;
	return 0;
}

Table* CatalogManager::getTablePtr(std::string& tableName)
{
	std::vector<Table*>::iterator it_table;
	for (it_table = tables.begin(); it_table != tables.end(); it_table++)
		if ((*it_table)->tableName == tableName)
			return *it_table;
	return NULL;
}
Index* CatalogManager::getIndexPtr(std::string& indexName)
{
	std::vector<Index*>::iterator it_index;
	for (it_index = indexs.begin(); it_index != indexs.end(); it_index++)
		if ((*it_index)->indexName == indexName)
			return *it_index;
	return NULL;
}

