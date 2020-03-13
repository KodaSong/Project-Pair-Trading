//
//  marketdata.hpp
//

#ifndef marketdata_hpp
#define marketdata_hpp

#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <iomanip>

#include "database.hpp"
#include "curl_easy.h"
#include "curl_form.h"
#include "curl_ios.h"
#include "curl_exception.h"

#include "PairTrading.h"

using namespace std;

int RetrieveMarketData(string url_request, string& read_buffer);
int PopulatePairsTable(sqlite3* db, const map<int, pair<string, string>>& symbols);
int PopulatePricesTable(sqlite3* db, const map<string, Stock>& StockMap);
//int PopulatePairPriceRatioTable(sqlite3* db);
//int PopulatePairPricesTable(sqlite3* db);
//int PopulateTradeTable(sqlite3* db);	// Backtesting Table
int PopulateTestPricesTable(sqlite3* db, const map<string, Stock>& StockMap);

#endif