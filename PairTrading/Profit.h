#pragma once
#ifndef Profit_h
#define Profit_h

#include "Table.h"
#include <cmath>

// Calculate each Trade's Profit and each Pair's total Profit
// Update Profit Data into PairPrices Table and total profit into PairPricesRatio
void Calculate_Profit(vector<float> &PriceRatio, vector<Trade>& Trade_History, int k, sqlite3* db);

// Calculate each Trade's Profit and each Pair's total Profit
// Collect each pair's number of profit trades and loss trades and Calculate ProfitRatio
// Update Profit Data into Trade Table and total profit into PAIRS_Test
void BackTest(vector<float>& PriceRatio, vector<Trade>& Trade_Recent,
	map<int, int>& Profit_Trades, map<int, int>& Loss_Trades, map<int, float>& ProfitRatio, map<int, float>& Profit, int k, sqlite3* db);
#endif // !Profit_h
