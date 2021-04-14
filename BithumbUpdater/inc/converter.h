#pragma once
#include "header.h"

#include <string>
#include <vector>
#include <chrono>


// compare char* type 'date1' and char* type 'date2'
int cmpDate(char* date1, char* date2);


// convert to char* from 'tm'
// return var need to delete after use
char* getStrDate(tm tm, bool isTP = false);

// convert to char* from 'time_point'
// return var need to delete after use
char* getStrDate(std::chrono::system_clock::time_point time);


// convert to 'vector<Trans*>' from apiResult char*
// return var need to delete after use
std::vector<Trans*> apiData2Trans(char* apiData);

// convert to one 'CandleNum' from 'vec' and tm is new 'CandleNum.date' value
// return var need to delete after use
CandleNum* arrangeCandle(std::vector<Trans*> vec, tm tm);

// convert to one 'CandleNum' from 'vec' and tm is new 'CandleNum.date' value
// return var need to delete after use
CandleNum* arrangeCandle(std::vector<Candle*> vec, tm tm);
