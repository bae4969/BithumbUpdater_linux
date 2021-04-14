#include "sql.h"


std::vector<char*> SQL::getCoinNameList() {
	std::vector<char*> retVal;
	mysql_init(&con);
	conPtr = mysql_real_connect(&con, SQL_ADDR, SQL_ID, SQL_PW, NULL, SQL_PORT, (char*)NULL, 0);
	if (conPtr == NULL)
		return retVal;

	std::fill_n(query, QUERY_SIZE, 0);
	strcat(query, "select (name) from ");
	strcat(query, SQL_LIST);
	stat = mysql_query(conPtr, query);
	if (stat != 0) {
		mysql_close(conPtr);
		return retVal;
	}

	res = mysql_store_result(conPtr);
	while ((row = mysql_fetch_row(res)) != NULL) {
		char* newList = new char[strlen(row[0]) + 1]();
		for (size_t i = 0; i < strlen(row[0]); i++)
			newList[i] = row[0][i];
		retVal.push_back(newList);
	}

	mysql_free_result(res);
	mysql_close(conPtr);

	return retVal;
}
int SQL::checkDB() {
	mysql_init(&con);
	conPtr = mysql_real_connect(&con, SQL_ADDR, SQL_ID, SQL_PW, NULL, SQL_PORT, (char*)NULL, 0);
	if (conPtr == NULL)
		return -1;

	for (int i = 0; i < 2; i++) {
		std::fill_n(query, QUERY_SIZE, 0);
		strcat(query, "SELECT COUNT(*) FROM Information_schema.SCHEMATA WHERE SCHEMA_NAME = '");
		switch (i) {
		case 0:
			strcat(query, SQL_DB);
			break;
		case 1:
			strcat(query, SQL_SAVE);
			break;
		}
		strcat(query, "'");
		stat = mysql_query(conPtr, query);
		if (stat != 0) {
			mysql_close(conPtr);
			return -2;
		}
		res = mysql_store_result(conPtr);
		row = mysql_fetch_row(res);
		if (strncmp(row[0], "0", 1) == 0) {
			mysql_free_result(res);
			std::fill_n(query, QUERY_SIZE, 0);
			strcat(query, "create database ");
			switch (i) {
			case 0:
				strcat(query, SQL_DB);
				break;
			case 1:
				strcat(query, SQL_SAVE);
				break;
			}
			stat = mysql_query(conPtr, query);
			if (stat != 0) {
				mysql_close(conPtr);
				return -3;
			}
			res = mysql_store_result(conPtr);
		}
		mysql_free_result(res);
	}

	mysql_close(conPtr);

	return 0;
}
int SQL::checkTable(char* coinName) {
	mysql_init(&con);
	conPtr = mysql_real_connect(&con, SQL_ADDR, SQL_ID, SQL_PW, NULL, SQL_PORT, (char*)NULL, 0);
	if (conPtr == NULL)
		return -1;

	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < 9; i++) {
			if (i == 0 && j == 1) continue;
			std::fill_n(query, QUERY_SIZE, 0);
			strcat(query, "SELECT COUNT(*) FROM Information_schema.tables WHERE table_schema = '");
			switch (j) {
			case 0:
				strcat(query, SQL_DB);
				break;
			case 1:
				strcat(query, SQL_SAVE);
				break;
			}
			strcat(query, "' AND table_name = '");
			strcat(query, coinName);
			switch (i) {
			case 0:
				strcat(query, "_trans");
				break;
			case 1:
				strcat(query, "_min1");
				break;
			case 2:
				strcat(query, "_min10");
				break;
			case 3:
				strcat(query, "_min30");
				break;
			case 4:
				strcat(query, "_hour1");
				break;
			case 5:
				strcat(query, "_hour4");
				break;
			case 6:
				strcat(query, "_day");
				break;
			case 7:
				strcat(query, "_week");
				break;
			case 8:
				strcat(query, "_month");
				break;
			default:
				return -10;
			}
			strcat(query, "'");
			stat = mysql_query(conPtr, query);
			if (stat != 0) {
				mysql_close(conPtr);
				return -2;
			}
			res = mysql_store_result(conPtr);
			row = mysql_fetch_row(res);

			if (strncmp(row[0], "0", 1) == 0) {
				mysql_free_result(res);
				std::fill_n(query, QUERY_SIZE, 0);
				strcat(query, "create table ");
				switch (j) {
				case 0:
					strcat(query, SQL_DB);
					break;
				case 1:
					strcat(query, SQL_SAVE);
					break;
				}
				strcat(query, ".");
				strcat(query, coinName);
				switch (i) {
				case 0:
					strcat(query, "_trans");
					break;
				case 1:
					strcat(query, "_min1");
					break;
				case 2:
					strcat(query, "_min10");
					break;
				case 3:
					strcat(query, "_min30");
					break;
				case 4:
					strcat(query, "_hour1");
					break;
				case 5:
					strcat(query, "_hour4");
					break;
				case 6:
					strcat(query, "_day");
					break;
				case 7:
					strcat(query, "_week");
					break;
				case 8:
					strcat(query, "_month");
					break;
				default:
					return -10;
				}
				switch (j) {
				case 0:
					if(i == 0)
						strcat(query, " (date datetime NOT NULL, ");
					else
						strcat(query, " (date datetime Primary Key, ");
					break;
				case 1:
					strcat(query, " (date datetime NOT NULL, ");
					break;
				}
				switch (i) {
				case 0:
					strcat(query, "isBid tinyint NOT NULL, unit float NOT NULL,");
					strcat(query, "price float NOT NULL, total float NOT NULL,");
					strcat(query, "primary key (date, isBid, unit, price, total)) ");
					break;
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
					strcat(query, "open float NOT NULL, close float NOT NULL,");
					strcat(query, "max float NOT NULL, min float NOT NULL,");
					strcat(query, "total float NOT NULL, volume float NOT NULL) ");
					break;
				}
				switch (j) {
				case 0:
					strcat(query, "ENGINE = MyISAM");
					break;
				case 1:
					strcat(query, "ENGINE = ARCHIVE");
					break;
				}
				strcat(query, " DEFAULT CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin");
				stat = mysql_query(conPtr, query);
				if (stat != 0) {
					mysql_close(conPtr);
					return -3;
				}
				res = mysql_store_result(conPtr);
			}
			mysql_free_result(res);
		}
	}

	mysql_close(conPtr);
	return 0;
}

std::vector<Trans*> SQL::selectTransLast(char* coinName, int num) {
	std::vector<Trans*> retVal;
	mysql_init(&con);
	conPtr = mysql_real_connect(&con, SQL_ADDR, SQL_ID, SQL_PW, NULL, SQL_PORT, (char*)NULL, 0);
	if (conPtr == NULL)
		return retVal;

	std::fill_n(query, QUERY_SIZE, 0);
	strcat(query, "select * from ");
	strcat(query, SQL_DB);
	strcat(query, ".");
	strcat(query, coinName);
	strcat(query, "_trans order by date desc limit ");
	strcat(query, std::to_string(num).c_str());

	stat = mysql_query(conPtr, query);
	if (stat != 0) {
		mysql_close(conPtr);
		return retVal;
	}

	res = mysql_store_result(conPtr);
	while ((row = mysql_fetch_row(res)) != NULL) {
		Trans* trans = new Trans();

		trans->date = new char[strlen(row[0]) + 1]();
		for (size_t i = 0; i < strlen(row[0]); i++)
			trans->date[i] = row[0][i];

		trans->isBid = new char[strlen(row[1]) + 1]();
		for (size_t i = 0; i < strlen(row[1]); i++)
			trans->isBid[i] = row[1][i];

		trans->unit = new char[strlen(row[2]) + 1]();
		for (size_t i = 0; i < strlen(row[2]); i++)
			trans->unit[i] = row[2][i];

		trans->price = new char[strlen(row[3]) + 1]();
		for (size_t i = 0; i < strlen(row[3]); i++)
			trans->price[i] = row[3][i];

		trans->total = new char[strlen(row[4]) + 1]();
		for (size_t i = 0; i < strlen(row[4]); i++)
			trans->total[i] = row[4][i];

		retVal.push_back(trans);
	}

	mysql_free_result(res);
	mysql_close(conPtr);

	return retVal;
}
std::vector<Trans*> SQL::selectTrans(char* coinName, char* from, char* to) {
	std::vector<Trans*> retVal;
	mysql_init(&con);
	conPtr = mysql_real_connect(&con, SQL_ADDR, SQL_ID, SQL_PW, NULL, SQL_PORT, (char*)NULL, 0);
	if (conPtr == NULL)
		return retVal;

	std::fill_n(query, QUERY_SIZE, 0);
	strcat(query, "select * from ");
	strcat(query, SQL_DB);
	strcat(query, ".");
	strcat(query, coinName);
	strcat(query, "_trans where date between '");
	strcat(query, from);
	strcat(query, "' and '");
	strcat(query, to);
	strcat(query, "'");

	stat = mysql_query(conPtr, query);
	if (stat != 0) {
		mysql_close(conPtr);
		return retVal;
	}

	res = mysql_store_result(conPtr);
	while ((row = mysql_fetch_row(res)) != NULL) {
		Trans* trans = new Trans();

		trans->date = new char[strlen(row[0]) + 1]();
		for (size_t i = 0; i < strlen(row[0]); i++)
			trans->date[i] = row[0][i];

		trans->isBid = new char[strlen(row[1]) + 1]();
		for (size_t i = 0; i < strlen(row[1]); i++)
			trans->isBid[i] = row[1][i];

		trans->unit = new char[strlen(row[2]) + 1]();
		for (size_t i = 0; i < strlen(row[2]); i++)
			trans->unit[i] = row[2][i];

		trans->price = new char[strlen(row[3]) + 1]();
		for (size_t i = 0; i < strlen(row[3]); i++)
			trans->price[i] = row[3][i];

		trans->total = new char[strlen(row[4]) + 1]();
		for (size_t i = 0; i < strlen(row[4]); i++)
			trans->total[i] = row[4][i];

		retVal.push_back(trans);
	}

	mysql_free_result(res);
	mysql_close(conPtr);

	return retVal;
}
std::vector<Candle*> SQL::selectCandleLast(char* coinName, int type, int num) {
	std::vector<Candle*> retVal;
	mysql_init(&con);
	conPtr = mysql_real_connect(&con, SQL_ADDR, SQL_ID, SQL_PW, NULL, SQL_PORT, (char*)NULL, 0);
	if (conPtr == NULL)
		return retVal;

	std::fill_n(query, QUERY_SIZE, 0);
	strcat(query, "select * from ");
	strcat(query, SQL_DB);
	strcat(query, ".");
	strcat(query, coinName);
	switch (type) {
	case 1:
		strcat(query, "_min1");
		break;
	case 2:
		strcat(query, "_min10");
		break;
	case 3:
		strcat(query, "_min30");
		break;
	case 4:
		strcat(query, "_hour1");
		break;
	case 5:
		strcat(query, "_hour4");
		break;
	case 6:
		strcat(query, "_day");
		break;
	case 7:
		strcat(query, "_week");
		break;
	case 8:
		strcat(query, "_month");
		break;
	default:
		return retVal;
	}
	strcat(query, " order by date desc limit ");
	strcat(query, std::to_string(num).c_str());

	stat = mysql_query(conPtr, query);
	if (stat != 0) {
		mysql_close(conPtr);
		return retVal;
	}

	res = mysql_store_result(conPtr);
	while ((row = mysql_fetch_row(res)) != NULL) {
		Candle* candle = new Candle();

		candle->date = new char[strlen(row[0]) + 1]();
		for (size_t i = 0; i < strlen(row[0]); i++)
			candle->date[i] = row[0][i];

		candle->open = new char[strlen(row[1]) + 1]();
		for (size_t i = 0; i < strlen(row[1]); i++)
			candle->open[i] = row[1][i];

		candle->close = new char[strlen(row[2]) + 1]();
		for (size_t i = 0; i < strlen(row[2]); i++)
			candle->close[i] = row[2][i];

		candle->max = new char[strlen(row[3]) + 1]();
		for (size_t i = 0; i < strlen(row[3]); i++)
			candle->max[i] = row[3][i];

		candle->min = new char[strlen(row[4]) + 1]();
		for (size_t i = 0; i < strlen(row[4]); i++)
			candle->min[i] = row[4][i];

		candle->total = new char[strlen(row[5]) + 1]();
		for (size_t i = 0; i < strlen(row[5]); i++)
			candle->total[i] = row[5][i];

		candle->volume = new char[strlen(row[6]) + 1]();
		for (size_t i = 0; i < strlen(row[6]); i++)
			candle->volume[i] = row[6][i];

		retVal.push_back(candle);
	}

	mysql_free_result(res);
	mysql_close(conPtr);

	return retVal;
}
std::vector<Candle*> SQL::selectCandle(char* coinName, int type, char* from, char* to) {
	std::vector<Candle*> retVal;
	mysql_init(&con);
	conPtr = mysql_real_connect(&con, SQL_ADDR, SQL_ID, SQL_PW, NULL, SQL_PORT, (char*)NULL, 0);
	if (conPtr == NULL)
		return retVal;

	std::fill_n(query, QUERY_SIZE, 0);
	strcat(query, "select * from ");
	strcat(query, SQL_DB);
	strcat(query, ".");
	strcat(query, coinName);
	switch (type) {
	case 1:
		strcat(query, "_min1");
		break;
	case 2:
		strcat(query, "_min10");
		break;
	case 3:
		strcat(query, "_min30");
		break;
	case 4:
		strcat(query, "_hour1");
		break;
	case 5:
		strcat(query, "_hour4");
		break;
	case 6:
		strcat(query, "_day");
		break;
	case 7:
		strcat(query, "_week");
		break;
	case 8:
		strcat(query, "_month");
		break;
	default:
		return retVal;
	}
	strcat(query, " where date between '");
	strcat(query, from);
	strcat(query, "' and '");
	strcat(query, to);
	strcat(query, "'");

	stat = mysql_query(conPtr, query);
	if (stat != 0) {
		mysql_close(conPtr);
		return retVal;
	}

	res = mysql_store_result(conPtr);
	while ((row = mysql_fetch_row(res)) != NULL) {
		Candle* candle = new Candle();

		candle->date = new char[strlen(row[0]) + 1]();
		for (size_t i = 0; i < strlen(row[0]); i++)
			candle->date[i] = row[0][i];

		candle->open = new char[strlen(row[1]) + 1]();
		for (size_t i = 0; i < strlen(row[1]); i++)
			candle->open[i] = row[1][i];

		candle->close = new char[strlen(row[2]) + 1]();
		for (size_t i = 0; i < strlen(row[2]); i++)
			candle->close[i] = row[2][i];

		candle->max = new char[strlen(row[3]) + 1]();
		for (size_t i = 0; i < strlen(row[3]); i++)
			candle->max[i] = row[3][i];

		candle->min = new char[strlen(row[4]) + 1]();
		for (size_t i = 0; i < strlen(row[4]); i++)
			candle->min[i] = row[4][i];

		candle->total = new char[strlen(row[5]) + 1]();
		for (size_t i = 0; i < strlen(row[5]); i++)
			candle->total[i] = row[5][i];

		candle->volume = new char[strlen(row[6]) + 1]();
		for (size_t i = 0; i < strlen(row[6]); i++)
			candle->volume[i] = row[6][i];

		retVal.push_back(candle);
	}

	mysql_free_result(res);
	mysql_close(conPtr);

	return retVal;
}
int SQL::insert(char* coinName, Trans* trans) {
	mysql_init(&con);
	conPtr = mysql_real_connect(&con, SQL_ADDR, SQL_ID, SQL_PW, NULL, SQL_PORT, (char*)NULL, 0);
	if (conPtr == NULL)
		return -1;

	std::fill_n(query, QUERY_SIZE, 0);
	strcat(query, "insert into ");
	strcat(query, SQL_DB);
	strcat(query, ".");
	strcat(query, coinName);
	strcat(query, "_trans (date, isBid, unit, price, total) values ('");
	strcat(query, trans->date);
	strcat(query, "', ");
	strcat(query, trans->isBid);
	strcat(query, ", ");
	strcat(query, trans->unit);
	strcat(query, ", ");
	strcat(query, trans->price);
	strcat(query, ", ");
	strcat(query, trans->total);
	strcat(query, ")");
	stat = mysql_query(conPtr, query);
	if (stat != 0) {
		mysql_close(conPtr);
		return -2;
	}
	res = mysql_store_result(conPtr);
	mysql_free_result(res);
	mysql_close(conPtr);

	return 0;
}
int SQL::insert(char* coinName, int type, Candle* candle) {
	mysql_init(&con);
	conPtr = mysql_real_connect(&con, SQL_ADDR, SQL_ID, SQL_PW, NULL, SQL_PORT, (char*)NULL, 0);
	if (conPtr == NULL)
		return -1;

	std::fill_n(query, QUERY_SIZE, 0);
	strcat(query, "insert into ");
	strcat(query, SQL_DB);
	strcat(query, ".");
	strcat(query, coinName);
	switch (type) {
	case 1:
		strcat(query, "_min1");
		break;
	case 2:
		strcat(query, "_min10");
		break;
	case 3:
		strcat(query, "_min30");
		break;
	case 4:
		strcat(query, "_hour1");
		break;
	case 5:
		strcat(query, "_hour4");
		break;
	case 6:
		strcat(query, "_day");
		break;
	case 7:
		strcat(query, "_week");
		break;
	case 8:
		strcat(query, "_month");
		break;
	default:
		return -10;
	}
	strcat(query, " (date, open, close, max, min, total, volume) values ('");
	strcat(query, candle->date);
	strcat(query, "', ");
	strcat(query, candle->open);
	strcat(query, ", ");
	strcat(query, candle->close);
	strcat(query, ", ");
	strcat(query, candle->max);
	strcat(query, ", ");
	strcat(query, candle->min);
	strcat(query, ", ");
	strcat(query, candle->total);
	strcat(query, ", ");
	strcat(query, candle->volume);
	strcat(query, ")");
	stat = mysql_query(conPtr, query);
	if (stat != 0) {
		mysql_close(conPtr);
		return -2;
	}
	res = mysql_store_result(conPtr);
	mysql_free_result(res);
	mysql_close(conPtr);

	return 0;
}
int SQL::insert(char* coinName, int type, CandleNum* candleNum) {
	char tmBuf[32] = {};
	strftime(tmBuf, sizeof(tmBuf), "%Y-%m-%d %H:%M:%S", &(candleNum->date));

	mysql_init(&con);
	conPtr = mysql_real_connect(&con, SQL_ADDR, SQL_ID, SQL_PW, NULL, SQL_PORT, (char*)NULL, 0);
	if (conPtr == NULL)
		return -1;

	std::fill_n(query, QUERY_SIZE, 0);
	strcat(query, "insert into ");
	strcat(query, SQL_DB);
	strcat(query, ".");
	strcat(query, coinName);
	switch (type) {
	case 1:
		strcat(query, "_min1");
		break;
	case 2:
		strcat(query, "_min10");
		break;
	case 3:
		strcat(query, "_min30");
		break;
	case 4:
		strcat(query, "_hour1");
		break;
	case 5:
		strcat(query, "_hour4");
		break;
	case 6:
		strcat(query, "_day");
		break;
	case 7:
		strcat(query, "_week");
		break;
	case 8:
		strcat(query, "_month");
		break;
	default:
		return -10;
	}
	strcat(query, " (date, open, close, max, min, total, volume) values ('");
	strcat(query, tmBuf);
	strcat(query, "', ");
	strcat(query, std::to_string(candleNum->open).c_str());
	strcat(query, ", ");
	strcat(query, std::to_string(candleNum->close).c_str());
	strcat(query, ", ");
	strcat(query, std::to_string(candleNum->max).c_str());
	strcat(query, ", ");
	strcat(query, std::to_string(candleNum->min).c_str());
	strcat(query, ", ");
	strcat(query, std::to_string(candleNum->total).c_str());
	strcat(query, ", ");
	strcat(query, std::to_string(candleNum->volume).c_str());
	strcat(query, ")");
	stat = mysql_query(conPtr, query);
	if (stat != 0) {
		mysql_close(conPtr);
		return -2;
	}
	res = mysql_store_result(conPtr);
	mysql_free_result(res);
	mysql_close(conPtr);

	return 0;
}
int SQL::save(char* coinName, int type, char* date) {
	mysql_init(&con);
	conPtr = mysql_real_connect(&con, SQL_ADDR, SQL_ID, SQL_PW, NULL, SQL_PORT, (char*)NULL, 0);
	if (conPtr == NULL)
		return -1;

	std::fill_n(query, QUERY_SIZE, 0);
	strcat(query, "insert into ");
	strcat(query, SQL_SAVE);
	strcat(query, ".");
	strcat(query, coinName);
	switch (type) {
	case 0:
		strcat(query, "_trans");
		break;
	case 1:
		strcat(query, "_min1");
		break;
	case 2:
		strcat(query, "_min10");
		break;
	case 3:
		strcat(query, "_min30");
		break;
	case 4:
		strcat(query, "_hour1");
		break;
	case 5:
		strcat(query, "_hour4");
		break;
	case 6:
		strcat(query, "_day");
		break;
	case 7:
		strcat(query, "_week");
		break;
	case 8:
		strcat(query, "_month");
		break;
	default:
		return -10;
	}
	strcat(query, " select * from ");
	strcat(query, SQL_DB);
	strcat(query, ".");
	strcat(query, coinName);
	switch (type) {
	case 0:
		strcat(query, "_trans");
		break;
	case 1:
		strcat(query, "_min1");
		break;
	case 2:
		strcat(query, "_min10");
		break;
	case 3:
		strcat(query, "_min30");
		break;
	case 4:
		strcat(query, "_hour1");
		break;
	case 5:
		strcat(query, "_hour4");
		break;
	case 6:
		strcat(query, "_day");
		break;
	case 7:
		strcat(query, "_week");
		break;
	case 8:
		strcat(query, "_month");
		break;
	default:
		return -10;
	}
	strcat(query, " where date < '");
	strcat(query, date);
	strcat(query, "'");

	stat = mysql_query(conPtr, query);
	if (stat != 0) {
		mysql_close(conPtr);
		return -2;
	}
	res = mysql_store_result(conPtr);
	mysql_free_result(res);
	mysql_close(conPtr);

	return 0;
}
int SQL::remove(char* coinName, int type, char* date) {
	mysql_init(&con);
	conPtr = mysql_real_connect(&con, SQL_ADDR, SQL_ID, SQL_PW, NULL, SQL_PORT, (char*)NULL, 0);
	if (conPtr == NULL)
		return -1;

	std::fill_n(query, QUERY_SIZE, 0);
	strcat(query, "delete from ");
	strcat(query, SQL_DB);
	strcat(query, ".");
	strcat(query, coinName);
	switch (type) {
	case 0:
		strcat(query, "_trans");
		break;
	case 1:
		strcat(query, "_min1");
		break;
	case 2:
		strcat(query, "_min10");
		break;
	case 3:
		strcat(query, "_min30");
		break;
	case 4:
		strcat(query, "_hour1");
		break;
	case 5:
		strcat(query, "_hour4");
		break;
	case 6:
		strcat(query, "_day");
		break;
	case 7:
		strcat(query, "_week");
		break;
	case 8:
		strcat(query, "_month");
		break;
	default:
		return -10;
	}
	strcat(query, " where date < '");
	strcat(query, date);
	strcat(query, "'");

	stat = mysql_query(conPtr, query);
	if (stat != 0) {
		mysql_close(conPtr);
		return -2;
	}
	res = mysql_store_result(conPtr);
	mysql_free_result(res);
	mysql_close(conPtr);

	return 0;
}
