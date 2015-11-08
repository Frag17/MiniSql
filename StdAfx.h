#pragma once

#include<iostream>
#include<string>
#include<list>
#include<map>
#include<vector>
#include<sstream>
#include<fstream>
using namespace std;

#include"BufferManager.h"
#include"RecordManager.h"
#include"CatalogManager.h"
#include"IndexManager.h"
#include"api.h"


BufferManager Buf;
RecordManager Rec;
CatalogManager Cat;
IndexManager Ind;