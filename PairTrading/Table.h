#pragma once
#ifndef Table_h
#define Table_h

#include "RetrieveData.h"
#include <cmath>

int Retrieve_PairPrices_Table(sqlite3* db, vector<Trade>& Trade_History, map<int, pair<string, string> >& pairMap);
int Retrieve_PriceRatio_Table(sqlite3* db, vector<float>& PriceRatio);

void CreatePairsTable(sqlite3* & db, map<int, pair<string, string> >& Pairs);
void CreatePricesTable(sqlite3* & db, map<string, Stock>& StockMap, set<string>& pairSymbols, vector<Stock>& StockList, string StartDate, string EndDate);
int CreatePriceRatioTable(sqlite3* & db);
void CreateTestPricesTable(sqlite3*& db, map<string, Stock>& StockMap, set<string>& pairSymbols, vector<Stock>& StockList, string StartDate, string EndDate);
int CreatePairPricesTable(sqlite3*& db);
int CreateTradeTable(sqlite3*& db);	// Backtesting Table
int Retrieve_Trade_Table(sqlite3* db, vector<Trade>& Trade_Recent, map<int, pair<string, string> >& pairMap);

#endif // !Table_h


