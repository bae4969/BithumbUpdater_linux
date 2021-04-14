#pragma once

#include <iostream>
#include <string>
#include <string.h>


// char* type transaction data structure
struct Trans {
	char* date;
	char* isBid;
	char* unit;
	char* price;
	char* total;
};

// char* type candle data structure
struct Candle {
	char* date;
	char* open;
	char* close;
	char* max;
	char* min;
	char* total;
	char* volume;
};


// number type transaction data structure
struct TransNum {
	tm date;
	bool isBid;
	float unit;
	float price;
	float total;
};

// number type candle data structure
struct CandleNum {
	tm date;
	float open = 0.0f;
	float close = 0.0f;
	float max = 0.0f;
	float min = 1000000000000.0f;
	float total = 0.0f;
	float volume = 0.0f;
};
