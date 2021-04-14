#pragma once
#include "sql_info.h"
#include "header.h"

#include <mysql/mysql.h>
#include <vector>
#include <iomanip>


#define SQL_TRANS 0
#define SQL_MIN1 1
#define SQL_MIN10 2
#define SQL_MIN30 3
#define SQL_HOUR1 4
#define SQL_HOUR4 5
#define SQL_DAY 6
#define SQL_WEEK 7
#define SQL_MONTH 8

#define QUERY_SIZE 512


// MySQL function set
class SQL {
private:
	MYSQL con;
	MYSQL* conPtr;
	MYSQL_RES* res;
	MYSQL_ROW row;
	int stat;
	char query[QUERY_SIZE];

public:
	// select update list from 'SQL_LIST' table
	std::vector<char*> getCoinNameList();

	// check existence of schema and if not exist create schema
	int checkDB();

	// check existence of tables and if not exist create table
	int checkTable(char* coinName);


	// select 'Trans' 'num' in order of decreasing date
	// return var need to delete after use
	std::vector<Trans*> selectTransLast(char* coinName, int num = 1);

	// select 'Trans' all between data 'from' and 'to'
	// return var need to delete after use
	std::vector<Trans*> selectTrans(char* coinName, char* from, char* to);

	// select 'Candle' 'num' in order of decreasing date
	// return var need to delete after use
	std::vector<Candle*> selectCandleLast(char* coinName, int type, int num = 1);

	// select 'Candle' all between data 'from' and 'to'
	// return var need to delete after use
	std::vector<Candle*> selectCandle(char* coinName, int type, char* from, char* to);

	// insert 'Trans' data to 'SQL_DB' schema table
	int insert(char* coinName, Trans* trans);

	// insert 'Candle' data to 'SQL_DB' schema table
	int insert(char* coinName, int type, Candle* candle);

	// insert 'CandleNum' data to 'SQL_DB' schema table
	int insert(char* coinName, int type, CandleNum* candleNum);

	// copy to 'SQL_SAVE' schema table all before 'date' in 'SQL_DB' schema table
	int save(char* coinName, int type, char* date);

	// remove all before 'date' schema table in 'SQL_DB' schema table
	int remove(char* coinName, int type, char* date);
};

