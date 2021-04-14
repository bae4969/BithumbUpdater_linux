#include "api.h"
#include "converter.h"
#include "sql.h"

#include <thread>
#include <unistd.h>


// make candle data from one step lower trans or candle data and insert to SQL
// and copy to 'SQL_SAVE' schema and remove except trans data
void insertCandle(char* coinName, int type, std::chrono::system_clock::time_point date) {
	time_t dateTT = std::chrono::system_clock::to_time_t(date);
	tm dateTM = *localtime(&dateTT);
	char* from = getStrDate(date);
	char* to = NULL;
	char* removeDate = NULL;
	std::chrono::seconds sec(1);

	switch (type) {
	case SQL_MIN1: {
		std::chrono::minutes min(1);
		to = getStrDate(date + min - sec);
		removeDate = getStrDate(date + min);
		break;
	}
	case SQL_MIN10: {
		std::chrono::minutes min(10);
		std::chrono::hours removeFlag(72);
		to = getStrDate(date + min - sec);
		removeDate = getStrDate(date - removeFlag);
		break;
	}
	case SQL_MIN30: {
		std::chrono::minutes min(30);
		std::chrono::hours removeFlag(720);
		to = getStrDate(date + min - sec);
		removeDate = getStrDate(date - removeFlag);
		break;
	}
	case SQL_HOUR1: {
		std::chrono::hours hour(1);
		std::chrono::hours removeFlag(2240);
		to = getStrDate(date + hour - sec);
		removeDate = getStrDate(date - removeFlag);
		break;
	}
	case SQL_HOUR4: {
		std::chrono::hours hour(4);
		std::chrono::hours removeFlag(4480);
		to = getStrDate(date + hour - sec);
		removeDate = getStrDate(date - removeFlag);
		break;
	}
	case SQL_DAY: {
		std::chrono::hours hour(24);
		std::chrono::hours removeFlag(17920);
		to = getStrDate(date + hour - sec);
		removeDate = getStrDate(date - removeFlag);
		break;
	}
	case SQL_WEEK: {
		std::chrono::hours hour(168);
		std::chrono::hours removeFlag(125440);
		to = getStrDate(date + hour - sec);
		removeDate = getStrDate(date - removeFlag);
		break;
	}
	case SQL_MONTH: {
		std::chrono::hours hour(5208);
		std::chrono::hours removeFlag(3763200);
		to = getStrDate(date + hour - sec);
		removeDate = getStrDate(date - removeFlag);
		break;
	}}

	SQL sql;
	CandleNum* candleNum = NULL;

	switch (type) {
	case SQL_MIN1: {
		std::vector<Trans*> trans = sql.selectTrans(coinName, from, to);
		candleNum = arrangeCandle(trans, dateTM);
		for (size_t i = 0; i < trans.size(); i++) {
			delete[] trans[i]->date;
			delete[] trans[i]->isBid;
			delete[] trans[i]->unit;
			delete[] trans[i]->price;
			delete[] trans[i]->total;
			delete trans[i];
		}
		break;
	}
	case SQL_MIN10:
	case SQL_MIN30:
	case SQL_HOUR1:
	case SQL_HOUR4:
	case SQL_DAY:
	case SQL_WEEK:
	case SQL_MONTH: {
		std::vector<Candle*> candle = sql.selectCandle(coinName, type - 1, from, to);
		candleNum = arrangeCandle(candle, dateTM);
		for (size_t i = 0; i < candle.size(); i++) {
			delete[] candle[i]->date;
			delete[] candle[i]->open;
			delete[] candle[i]->close;
			delete[] candle[i]->max;
			delete[] candle[i]->min;
			delete[] candle[i]->total;
			delete[] candle[i]->volume;
			delete candle[i];
		}
		sql.save(coinName, type - 1, removeDate);
		break;
	}}

	sql.remove(coinName, type - 1, removeDate);

	if (candleNum != NULL) {
		sql.insert(coinName, type, candleNum);
		delete candleNum;
	}

	delete[] from;
	delete[] to;
	delete[] removeDate;
}

// get transaction data from api and insert to SQL
void insertTrans(char* coinName, std::chrono::system_clock::time_point curTime) {
	char endpoint[64] = "/public/transaction_history/";
	strcat(endpoint, coinName);
	strcat(endpoint, "_KRW?count=30");

	char* apiData = api_request(endpoint, (char*)"");
	std::vector<Trans*> trans = apiData2Trans(apiData);
	free(apiData);

	char* curDate = getStrDate(curTime);
	SQL sql;
	for (size_t i = 0; i < trans.size(); i++) {
		if (cmpDate(trans[i]->date, curDate) >= 0)
			sql.insert(coinName, trans[i]);
		delete[] trans[i]->date;
		delete[] trans[i]->isBid;
		delete[] trans[i]->unit;
		delete[] trans[i]->price;
		delete[] trans[i]->total;
		delete trans[i];
	}

	delete[] curDate;
	return;
}

// update function for a coin
void executeThread(char* coinName,
	std::chrono::system_clock::time_point curTime,
	std::chrono::system_clock::time_point lastTime)
{
	std::chrono::system_clock::time_point tempTime;
	time_t cur_tt = std::chrono::system_clock::to_time_t(curTime);
	time_t last_tt = std::chrono::system_clock::to_time_t(lastTime);
	tm cur_tm = *localtime(&cur_tt);
	tm last_tm = *localtime(&last_tt);

	{
		tempTime = curTime;
		std::chrono::seconds tempSec(cur_tm.tm_sec);
		tempTime -= tempSec;
		if (cur_tm.tm_sec < last_tm.tm_sec) {
			std::chrono::minutes tempMin(1);
			tempTime -= tempMin;
		}
		insertTrans(coinName, tempTime);
	}

	if (cur_tm.tm_sec < last_tm.tm_sec) {
		tempTime = curTime;
		std::chrono::seconds tempSec(cur_tm.tm_sec);
		std::chrono::minutes tempMin(1);
		tempTime -= tempSec;
		tempTime -= tempMin;
		insertCandle(coinName, SQL_MIN1, tempTime);
	}

	if (cur_tm.tm_min % 10 < last_tm.tm_min % 10) {
		tempTime = curTime;
		std::chrono::seconds tempSec(cur_tm.tm_sec);
		std::chrono::minutes tempMin(10);
		tempTime -= tempSec;
		tempTime -= tempMin;
		insertCandle(coinName, SQL_MIN10, tempTime);
	}

	if (cur_tm.tm_min % 30 < last_tm.tm_min % 30) {
		tempTime = curTime;
		std::chrono::seconds tempSec(cur_tm.tm_sec);
		std::chrono::minutes tempMin(30);
		tempTime -= tempSec;
		tempTime -= tempMin;
		insertCandle(coinName, SQL_MIN30, tempTime);
	}

	if (cur_tm.tm_min < last_tm.tm_min) {
		tempTime = curTime;
		std::chrono::seconds tempSec(cur_tm.tm_sec);
		std::chrono::minutes tempMin(cur_tm.tm_min);
		std::chrono::hours tempHour(1);
		tempTime -= tempSec;
		tempTime -= tempMin;
		tempTime -= tempHour;
		insertCandle(coinName, SQL_HOUR1, tempTime);
	}

	if (cur_tm.tm_hour % 4 < last_tm.tm_hour % 4) {
		tempTime = curTime;
		std::chrono::seconds tempSec(cur_tm.tm_sec);
		std::chrono::minutes tempMin(cur_tm.tm_min);
		std::chrono::hours tempHour(4);
		tempTime -= tempSec;
		tempTime -= tempMin;
		tempTime -= tempHour;
		insertCandle(coinName, SQL_HOUR4, tempTime);
	}

	if (cur_tm.tm_hour < last_tm.tm_hour) {
		tempTime = curTime;
		std::chrono::seconds tempSec(cur_tm.tm_sec);
		std::chrono::minutes tempMin(cur_tm.tm_min);
		std::chrono::hours tempHour(cur_tm.tm_hour + 24);
		tempTime -= tempSec;
		tempTime -= tempMin;
		tempTime -= tempHour;
		insertCandle(coinName, SQL_DAY, tempTime);
	}

	if (cur_tm.tm_wday < last_tm.tm_wday) {
		tempTime = curTime;
		std::chrono::seconds tempSec(cur_tm.tm_sec);
		std::chrono::minutes tempMin(cur_tm.tm_min);
		std::chrono::hours tempHour(cur_tm.tm_hour + 168);
		tempTime -= tempSec;
		tempTime -= tempMin;
		tempTime -= tempHour;
		insertCandle(coinName, SQL_WEEK, tempTime);
	}

	if (cur_tm.tm_mon < last_tm.tm_mon) {
		tempTime = curTime;
		std::chrono::seconds tempSec(cur_tm.tm_sec);
		std::chrono::minutes tempMin(cur_tm.tm_min);
		std::chrono::hours tempHour(cur_tm.tm_hour + cur_tm.tm_mday * 24);
		tempTime -= tempSec;
		tempTime -= tempMin;
		tempTime -= tempHour;
		time_t temp_tt = std::chrono::system_clock::to_time_t(tempTime);
		tm temp_tm = *localtime(&temp_tt);
		std::chrono::hours tempDay((temp_tm.tm_mday - 1) * 24);
		tempTime -= tempDay;
		insertCandle(coinName, SQL_MONTH, tempTime);
	}
}

// make update thread per coin infinitely 
void updateLoop() {
	SQL sql;
	std::vector<char*> nameList;
	std::chrono::system_clock::time_point curTime;
	std::chrono::system_clock::time_point lastTime = std::chrono::system_clock::now();

	while(true) {
		curTime = std::chrono::system_clock::now();

		nameList = sql.getCoinNameList();
		size_t listSize = nameList.size();
		std::thread* th = new std::thread[listSize];

		useconds_t executionTimer = (useconds_t)(3000000 / listSize);
		for (size_t i = 0; i < listSize; i++) {
			th[i] = std::thread(executeThread, nameList[i], curTime, lastTime);
			usleep(executionTimer);
		}

		for (size_t i = 0; i < listSize; i++)
			th[i].join();
		delete[] th;
		for (size_t i = 0; i < listSize; i++)
			delete[] nameList[i];
		nameList.clear();

		lastTime = curTime;
	}
}

// check MySQL status before execute update
int checkDBSetting() {
	SQL sql;
	std::vector<char*> nameList = sql.getCoinNameList();
	if (sql.checkDB()) return -1;
	for (size_t i = 0; i < nameList.size(); i++) {
		if (sql.checkTable(nameList[i]) < 0)
			return -2;
		delete[] nameList[i];
	}
	nameList.clear();

	return 0;
}

// main
int main(int argc, char** argv) {
	int ret = checkDBSetting();
	if (ret < 0) return ret;

	updateLoop();

	return 0;
}
