//
//  marketdata.cpp
//

#include "marketdata.hpp"
#include "database.hpp"

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

int RetrieveMarketData(string url_request, string & read_buffer)
{
	curl_global_init(CURL_GLOBAL_ALL);
	CURL * myHandle;
	CURLcode result;
	myHandle = curl_easy_init();

	curl_easy_setopt(myHandle, CURLOPT_URL, url_request.c_str());

	//adding a user agent
	curl_easy_setopt(myHandle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows; U; Windows NT 6.1; rv:2.2) Gecko/20110201");
	curl_easy_setopt(myHandle, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(myHandle, CURLOPT_SSL_VERIFYHOST, 0);
	//curl_easy_setopt(myHandle, CURLOPT_VERBOSE, 1);

	// send all data to this function  
	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);

	// we pass our 'chunk' struct to the callback function 
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &read_buffer);

	//perform a blocking file transfer
	result = curl_easy_perform(myHandle);

	// check for errors 
	if (result != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
		return -1;
	}
	curl_easy_cleanup(myHandle);
	return 0;
}

int PopulatePairsTable(sqlite3 *db, const map<int, pair<string, string>> & pairs)
{
    for (map<int, pair<string, string>>::const_iterator itr = pairs.begin(); itr != pairs.end(); itr++)
    {
        char pair_insert_table[512];
        sprintf(pair_insert_table, "INSERT INTO PAIRS (id, symbol1, symbol2) VALUES(%d, \"%s\", \"%s\")", itr->first, itr->second.first.c_str(), itr->second.second.c_str());
        if (InsertTable(pair_insert_table, db) == -1)
            return -1;
    }
    return 0;
}

int PopulatePricesTable(sqlite3 *db, const map<string, Stock> & StockMap)
{
	for (auto stock_itr = StockMap.begin(); stock_itr != StockMap.end(); ++stock_itr)
	{
		vector<TradeData> trades = stock_itr->second.getTrades();
		for (auto trade_itr = trades.begin(); trade_itr != trades.end(); ++trade_itr)
		{
			char pair_insert_table[512];
			sprintf(pair_insert_table, "Insert into Prices (symbol, date, open, high, low, close, adjusted_close, volume) \
					VALUES(\"%s\", \"%s\", %.2f, %.2f, %.2f, %.2f, %.2f, %ld)",
				stock_itr->first.c_str(), trade_itr->getDate().c_str(), trade_itr->getOpen(), trade_itr->getHigh(), trade_itr->getLow(),
				trade_itr->getClose(), trade_itr->getAdjClose(), trade_itr->getVolume());
			if (InsertTable(pair_insert_table, db) == -1)
				return -1;
		}
	}
	return 0;
}

//int PopulatePairPriceRatioTable(sqlite3* db)
//{
//	char pair_query_table[1000];
//	sprintf(pair_query_table, " CREATE TABLE IF NOT EXISTS PairPriceRatio (Id integer primary key, Symbol1 text, Symbol2 text, PriceRatio float); \
//								INSERT INTO PairPriceRatio (Id, Symbol1, Symbol2, PriceRatio) \
//								SELECT PAIRS.Id, PAIRS.Symbol1, PAIRS.Symbol2, \
//							    ROUND( AVG(Prices1.Adjusted_Close * Prices1.Adjusted_Close / Prices.Adjusted_Close / Prices.Adjusted_Close) - AVG(Prices1.Adjusted_Close / Prices.Adjusted_Close) * AVG(Prices1.Adjusted_Close / Prices.Adjusted_Close), 2) AS PriceRatio \
//								FROM Prices1, Prices, PAIRS \
//								WHERE Prices1.Symbol = PAIRS.Symbol1 AND Prices.Symbol = PAIRS.Symbol2 AND Prices1.Date = Prices.Date \
//								GROUP BY PAIRS.Id, PAIRS.Symbol1, PAIRS.Symbol2 \
//								ORDER BY PAIRS.Id, PAIRS.Symbol1, PAIRS.Symbol2;");
//	if (QueryTable(pair_query_table, db) == -1)
//	{
//		//cout << "Fail" << endl;
//		return -1;
//	}
//
//
//
//	return 0;
//}

//int PopulatePairPricesTable(sqlite3* db)
//{
//	char pair_query_table[1000];
//	sprintf(pair_query_table, " CREATE TABLE IF NOT EXISTS PairPrices (Symbol1 text, Symbol2 text, Symbol1Open float, Symbol1Close float, Symbol2Open float, Symbol2Close float, PriceDate text); \
//								INSERT INTO PairPrices (Symbol1, Symbol2, Symbol1Open, Symbol1Close, Symbol2Open, Symbol2Close, PriceDate) \
//								SELECT PAIRS.Symbol1 AS Symbol1, PAIRS.Symbol2 AS Symbol2, \
//							    Prices1.Open AS Symbol1Open, Prices1.Close AS Symbol1Close, \
//								Prices.Open AS Symbol2Open, Prices.Close AS Symbol2Close, \
//								Prices.Date AS PriceDate \
//								FROM Prices1, Prices, PAIRS \
//								WHERE Prices1.Symbol = PAIRS.Symbol1 AND Prices.Symbol = PAIRS.Symbol2 AND Prices1.Date = Prices.Date \
//								ORDER BY PAIRS.Symbol1, PAIRS.Symbol2, Prices1.Date;");
//	if (QueryTable(pair_query_table, db) == -1)
//	{
//		return -1;
//	}
//
//	return 0;
//}

//int PopulateTradeTable(sqlite3* db)
//{
//	char pair_query_table[1000];
//	sprintf(pair_query_table, " CREATE TABLE IF NOT EXISTS Trade (Symbol1 text, Symbol2 text, Symbol1Open float, Symbol1Close float, Symbol2Open float, Symbol2Close float, PriceDate text); \
//								INSERT INTO Trade (Symbol1, Symbol2, Symbol1Open, Symbol1Close, Symbol2Open, Symbol2Close, PriceDate) \
//								SELECT PAIRS.Symbol1 AS Symbol1, PAIRS.Symbol2 AS Symbol2, \
//							    TestPrices1.Open AS Symbol1Open, TestPrices1.Close AS Symbol1Close, \
//								TestPrices.Open AS Symbol2Open, TestPrices.Close AS Symbol2Close, \
//								TestPrices.Date AS PriceDate \
//								FROM TestPrices1, TestPrices, PAIRS \
//								WHERE TestPrices1.Symbol = PAIRS.Symbol1 AND TestPrices.Symbol = PAIRS.Symbol2 AND TestPrices1.Date = TestPrices.Date \
//								ORDER BY PAIRS.Symbol1, PAIRS.Symbol2, TestPrices1.Date;");
//	if (QueryTable(pair_query_table, db) == -1)
//	{
//		return -1;
//	}
//}

int PopulateTestPricesTable(sqlite3* db, const map<string, Stock>& StockMap)
{
	for (auto stock_itr = StockMap.begin(); stock_itr != StockMap.end(); ++stock_itr)
	{
		vector<TradeData> trades = stock_itr->second.getTrades();
		for (auto trade_itr = trades.begin(); trade_itr != trades.end(); ++trade_itr)
		{
			char pair_insert_table[512];
			sprintf(pair_insert_table, "Insert into TestPrices (symbol, date, open, high, low, close, adjusted_close, volume) \
					VALUES(\"%s\", \"%s\", %.2f, %.2f, %.2f, %.2f, %.2f, %ld)",
				stock_itr->first.c_str(), trade_itr->getDate().c_str(), trade_itr->getOpen(), trade_itr->getHigh(), trade_itr->getLow(),
				trade_itr->getClose(), trade_itr->getAdjClose(), trade_itr->getVolume());
			if (InsertTable(pair_insert_table, db) == -1)
				return -1;
		}
	}
	return 0;
}