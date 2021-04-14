#include "converter.h"


int cmpDate(char* date1, char* date2) {
	for (int i = 0; i < 20; i++) {
		if (date1[i] > date2[i])
			return 1;
		else if (date1[i] < date2[i])
			return -1;
	}

	return 0;
}

char* getStrDate(tm tm, bool isTP) {
	char* ret = new char[20]();

	int tmp;
	tmp = isTP ? tm.tm_year + 1900 : tm.tm_year;
	ret[3] = (char)((tmp % 10) + 48);
	tmp /= 10;
	ret[2] = (char)((tmp % 10) + 48);
	tmp /= 10;
	ret[1] = (char)((tmp % 10) + 48);
	tmp /= 10;
	ret[0] = (char)((tmp % 10) + 48);
	ret[4] = '-';

	tmp = isTP ? tm.tm_mon + 1 : tm.tm_mon;
	ret[6] = (char)((tmp % 10) + 48);
	tmp /= 10;
	ret[5] = (char)((tmp % 10) + 48);
	ret[7] = '-';

	tmp = tm.tm_mday;
	ret[9] = (char)((tmp % 10) + 48);
	tmp /= 10;
	ret[8] = (char)((tmp % 10) + 48);
	ret[10] = ' ';

	tmp = tm.tm_hour;
	ret[12] = (char)((tmp % 10) + 48);
	tmp /= 10;
	ret[11] = (char)((tmp % 10) + 48);
	ret[13] = ':';

	tmp = tm.tm_min;
	ret[15] = (char)((tmp % 10) + 48);
	tmp /= 10;
	ret[14] = (char)((tmp % 10) + 48);
	ret[16] = ':';

	tmp = tm.tm_sec;
	ret[18] = (char)((tmp % 10) + 48);
	tmp /= 10;
	ret[17] = (char)((tmp % 10) + 48);

	return ret;
}
char* getStrDate(std::chrono::system_clock::time_point time) {
	time_t tt = std::chrono::system_clock::to_time_t(time);
	return getStrDate(*localtime(&tt), true);
}

std::vector<Trans*> apiData2Trans(char* apiData) {
	std::vector<Trans*> retVec;
	Trans* transPtr;

	if (strlen(apiData) < 128)
		return retVec;
	if (strncmp(apiData + 11, "0000", 4) != 0)
		return retVec;

	size_t startPoint = 0;
	int save = 0;

	for (size_t i = 0; i < strlen(apiData); i++) {
		if (apiData[i] == '[') {
			startPoint = i;
			continue;
		}
	}
	for (size_t i = startPoint + 1; i < strlen(apiData); i++) {
		switch (save) {
		case 0:
			if (strncmp(apiData + i, "trans", 5) == 0) {
				save++;
				i += 19;
				transPtr = new Trans;

				transPtr->date = new char[20]();
				for (int j = 0; j < 19; j++)
					transPtr->date[j] = apiData[i + j];

				i += 19;
			}
			break;
		case 1:
			if (strncmp(apiData + i, "type", 4) == 0) {
				save++;
				i += 7;

				transPtr->isBid = new char[2]();
				if (strncmp(apiData + i, "ask", 3) == 0)
					transPtr->isBid[0] = '0';
				else
					transPtr->isBid[0] = '1';

				i += 1;
			}
			break;
		case 2:
			if (strncmp(apiData + i, "units", 5) == 0) {
				save++;
				i += 15;

				int length = 1;
				while (apiData[i + length - 1] != '"') length++;
				transPtr->unit = new char[length]();
				for (int j = 0; j < length - 1; j++)
					transPtr->unit[j] = apiData[i + j];

				i += length;
			}
			break;
		case 3:
			if (strncmp(apiData + i, "price", 5) == 0) {
				save++;
				i += 8;

				int length = 1;
				while (apiData[i + length - 1] != '"') length++;
				transPtr->price = new char[length]();
				for (int j = 0; j < length - 1; j++)
					transPtr->price[j] = apiData[i + j];

				i += length;
			}
			break;
		case 4:
			if (strncmp(apiData + i, "total", 5) == 0) {
				save = 0;
				i += 8;

				int length = 1;
				while (apiData[i + length - 1] != '"') length++;
				transPtr->total = new char[length]();
				for (int j = 0; j < length - 1; j++)
					transPtr->total[j] = apiData[i + j];

				i += length;

				retVec.push_back(transPtr);
			}
			break;
		}
	}

	return retVec;
}
CandleNum* arrangeCandle(std::vector<Trans*> vec, tm tm) {
	if (vec.size() < 1)
		return NULL;

	bool isFisrt = true;
	CandleNum* candleNum = new CandleNum();

	candleNum->date = tm;
	for (size_t i = 0; i < vec.size(); i++) {
		if (isFisrt) {
			isFisrt = false;
			candleNum->open = std::stof(vec[i]->price);
		}
		candleNum->close = std::stof(vec[i]->price);
		if (candleNum->max < std::stof(vec[i]->price))
			candleNum->max = std::stof(vec[i]->price);
		if (candleNum->min > std::stof(vec[i]->price))
			candleNum->min = std::stof(vec[i]->price);
		candleNum->total += std::stof(vec[i]->total);
		candleNum->volume += std::stof(vec[i]->unit);
	}

	return candleNum;
}
CandleNum* arrangeCandle(std::vector<Candle*> vec, tm tm) {
	if (vec.size() < 1)
		return NULL;

	bool isFisrt = true;
	CandleNum* candleNum = new CandleNum();

	candleNum->date = tm;
	for (size_t i = 0; i < vec.size(); i++) {
		if (isFisrt) {
			isFisrt = false;
			candleNum->open = std::stof(vec[i]->open);
		}
		candleNum->close = std::stof(vec[i]->close);
		if (candleNum->max < std::stof(vec[i]->max))
			candleNum->max = std::stof(vec[i]->max);
		if (candleNum->min > std::stof(vec[i]->min))
			candleNum->min = std::stof(vec[i]->min);
		candleNum->total += std::stof(vec[i]->total);
		candleNum->volume += std::stof(vec[i]->volume);
	}

	return candleNum;
}
