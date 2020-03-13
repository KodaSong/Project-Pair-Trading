#pragma once
#ifndef RetrieveData_h
#define RetrieveData_h

#include "marketdata.hpp"
#include <set>
#include <string>
using namespace std;

//reading "pairtrading.csv"
//extract useful information and use stl data structure to store
//set<string> Gen_pairSymbols(string filename);
void Gen_Pair(string filename, set<string>& pairSymbols, map<int, pair<string, string> >& pairMap);

// Use StockList to generate a stock map
void Gen_StockMap(vector<Stock>& StockList, map<string, Stock>& StockMap);

// Retrieve Data use libcurl and generate a vector<Stock> to store each stock's data
//int RetrieveData(set<string>& pairSymbols, vector<Stock>& StockList);
int RetrieveData(set<string>& pairSymbols, vector<Stock>& StockList, string StartDate, string EndDate);

// Split readBuffer
void  Split_Buffer(string readBuffer, vector<TradeData>& trade_vec);

#endif // !RetrieveData_h
