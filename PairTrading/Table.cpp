#include "Table.h"

int Retrieve_PairPrices_Table(sqlite3* db, vector<Trade>& Trade_History, map<int, pair<string, string> >& pairMap)
{
    int rc = 0;
    char* error = NULL;
    cout << "Retrieving values from PairPrices table ..." << endl;
    char** results = NULL;
    int rows, columns;
    const char* sql_select = "SELECT * from PairPrices;";
    sqlite3_get_table(db, sql_select, &results, &rows, &columns, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
        return -1;
    }
    for (int rowCtr = 0; rowCtr <= rows; ++rowCtr)
    {
        if (rowCtr == 0) continue;
        
        int cellPosition = (rowCtr * columns);// +colCtr;
        int id;
        string ticker1 = results[cellPosition];
        string ticker2 = results[cellPosition + 1];
        float open1 = stof(results[cellPosition + 2]);
        float close1 = stof(results[cellPosition + 3]);
        float open2 = stof(results[cellPosition + 4]);
        float close2 = stof(results[cellPosition + 5]);
        string date = results[cellPosition + 6];
        for (auto it = pairMap.begin(); it != pairMap.end(); it++)
        {
            if (ticker1 == it->second.first && ticker2 == it->second.second)
            {
                id = it->first;
                break;
            }
        }
        Trade trade(id, ticker1, ticker2, open1, close1, open2, close2, date, 0.0);
        Trade_History.push_back(trade);
    }
        
    cout << "Retrieve values into the table." << endl;
    return 0;
}

int Retrieve_Trade_Table(sqlite3* db, vector<Trade>& Trade_Recent, map<int, pair<string, string> >& pairMap)
{
    int rc = 0;
    char* error = NULL;
    cout << "Retrieving values from Trade table ..." << endl;
    char** results = NULL;
    int rows, columns;
    const char* sql_select = "SELECT * from Trade;";
    sqlite3_get_table(db, sql_select, &results, &rows, &columns, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
        return -1;
    }
    for (int rowCtr = 0; rowCtr <= rows; ++rowCtr)
    {
        if (rowCtr == 0) continue;

        int cellPosition = (rowCtr * columns);// +colCtr;
        int id;
        string ticker1 = results[cellPosition];
        string ticker2 = results[cellPosition + 1];
        float open1 = stof(results[cellPosition + 2]);
        float close1 = stof(results[cellPosition + 3]);
        float open2 = stof(results[cellPosition + 4]);
        float close2 = stof(results[cellPosition + 5]);
        string date = results[cellPosition + 6];
        for (auto it = pairMap.begin(); it != pairMap.end(); it++)
        {
            if (ticker1 == it->second.first && ticker2 == it->second.second)
            {
                id = it->first;
                break;
            }
        }
        Trade trade(id, ticker1, ticker2, open1, close1, open2, close2, date, 0.0);
        Trade_Recent.push_back(trade);
    }

    cout << "Retrieve values into the table." << endl;
    return 0;
}

int Retrieve_PriceRatio_Table(sqlite3* db, vector<float>& PriceRatio)
{
    int rc = 0;
    char* error = NULL;
    cout << "Retrieving values from PairPriceRatio table ..." << endl;
    char** results = NULL;
    int rows, columns;
    const char* sql_select = "SELECT PriceRatio from PairPriceRatio;";
    sqlite3_get_table(db, sql_select, &results, &rows, &columns, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
        return -1;
    }
    for (int rowCtr = 0; rowCtr <= rows; ++rowCtr)
    {
        if (rowCtr == 0) continue;
        float sigma = sqrt(stof(results[rowCtr]));
        PriceRatio.push_back(sigma);    // the ith Pair's PriceRatio is vector[i-1]
    }
    cout << "Retrieve values into the table." << endl;
    return 0;
}

void CreatePairsTable(sqlite3* & db, map<int, pair<string, string> >& pairMap)
{
    CreateTable("CREATE TABLE IF NOT EXISTS PAIRS (Id integer, Symbol1 text, Symbol2 text)", db);
    PopulatePairsTable(db, pairMap);

    CreateTable("CREATE TABLE IF NOT EXISTS PAIRS_Test (Id integer, Symbol1 text, Symbol2 text)", db);
    InsertTable("INSERT INTO PAIRS_Test SELECT * FROM PAIRS; ALTER TABLE PAIRS_Test Add TotalProfit float", db);	// Copy PAIRS into PAIRS_Test

}

void CreatePricesTable(sqlite3* & db, map<string, Stock>& StockMap, set<string>& pairSymbols, vector<Stock>& StockList, string StartDate, string EndDate)
{
    RetrieveData(pairSymbols, StockList, StartDate, EndDate);   // Retrieve Data online based on pairSymbols' tickers, and store data in StockList
    Gen_StockMap(StockList, StockMap);		// Store StockList's data into StockMap
    CreateTable("CREATE TABLE IF NOT EXISTS Prices (Symbol text, Date text, Open float, High float, Low float, Close float, Adjusted_Close float, Volume integer)", db);
    PopulatePricesTable(db, StockMap);

    CreateTable("CREATE TABLE IF NOT EXISTS Prices1 (Symbol text, Date text, Open float, High float, Low float, Close float, Adjusted_Close float, Volume integer)", db);
    InsertTable("INSERT INTO Prices1 SELECT * FROM Prices", db);	// Copy Prices into Prices1
}

int CreatePriceRatioTable(sqlite3* & db)
{
    char pair_query_table[1000];
    sprintf(pair_query_table, " CREATE TABLE IF NOT EXISTS PairPriceRatio (Id integer, Symbol1 text, Symbol2 text, PriceRatio float); \
								INSERT INTO PairPriceRatio (Id, Symbol1, Symbol2, PriceRatio) \
								SELECT PAIRS.Id, PAIRS.Symbol1, PAIRS.Symbol2, \
							    ROUND( AVG(Prices1.Adjusted_Close * Prices1.Adjusted_Close / Prices.Adjusted_Close / Prices.Adjusted_Close) - AVG(Prices1.Adjusted_Close / Prices.Adjusted_Close) * AVG(Prices1.Adjusted_Close / Prices.Adjusted_Close), 2) AS PriceRatio \
								FROM Prices1, Prices, PAIRS \
								WHERE Prices1.Symbol = PAIRS.Symbol1 AND Prices.Symbol = PAIRS.Symbol2 AND Prices1.Date = Prices.Date \
								GROUP BY PAIRS.Id, PAIRS.Symbol1, PAIRS.Symbol2 \
								ORDER BY PAIRS.Id, PAIRS.Symbol1, PAIRS.Symbol2; ALTER TABLE PairPriceRatio ADD TotalProfit float;");
    if (QueryTable(pair_query_table, db) == -1)
    {
        return -1;
    }

    return 0;
}

int CreatePairPricesTable(sqlite3*& db)
{
    char pair_query_table[1000];
    sprintf(pair_query_table, " CREATE TABLE IF NOT EXISTS PairPrices (Symbol1 text, Symbol2 text, Symbol1Open float, Symbol1Close float, Symbol2Open float, Symbol2Close float, PriceDate text); \
								INSERT INTO PairPrices (Symbol1, Symbol2, Symbol1Open, Symbol1Close, Symbol2Open, Symbol2Close, PriceDate) \
								SELECT PAIRS.Symbol1 AS Symbol1, PAIRS.Symbol2 AS Symbol2, \
							    Prices1.Open AS Symbol1Open, Prices1.Close AS Symbol1Close, \
								Prices.Open AS Symbol2Open, Prices.Close AS Symbol2Close, \
								Prices.Date AS PriceDate \
								FROM Prices1, Prices, PAIRS \
								WHERE Prices1.Symbol = PAIRS.Symbol1 AND Prices.Symbol = PAIRS.Symbol2 AND Prices1.Date = Prices.Date \
								ORDER BY PAIRS.Symbol1, PAIRS.Symbol2, Prices1.Date; ALTER TABLE PairPrices ADD Profit float");
    if (QueryTable(pair_query_table, db) == -1)
    {
        return -1;
    }

    return 0;
}

void CreateTestPricesTable(sqlite3*& db, map<string, Stock>& StockMap, set<string>& pairSymbols, vector<Stock>& StockList, string StartDate, string EndDate)
{
    RetrieveData(pairSymbols, StockList, StartDate, EndDate);   // Retrieve Data online based on pairSymbols' tickers, and store data in StockList
    Gen_StockMap(StockList, StockMap);		// Store StockList's data into StockMap
    CreateTable("CREATE TABLE IF NOT EXISTS TestPrices (Symbol text, Date text, Open float, High float, Low float, Close float, Adjusted_Close float, Volume integer)", db);
    PopulateTestPricesTable(db, StockMap);

    CreateTable("CREATE TABLE IF NOT EXISTS TestPrices1 (Symbol text, Date text, Open float, High float, Low float, Close float, Adjusted_Close float, Volume integer)", db);
    InsertTable("INSERT INTO TestPrices1 SELECT * FROM TestPrices", db);	// Copy Prices into Prices1
}

int CreateTradeTable(sqlite3*& db)
{
    char pair_query_table[1000];
    sprintf(pair_query_table, " CREATE TABLE IF NOT EXISTS Trade (Symbol1 text, Symbol2 text, Symbol1Open float, Symbol1Close float, Symbol2Open float, Symbol2Close float, PriceDate text); \
								INSERT INTO Trade (Symbol1, Symbol2, Symbol1Open, Symbol1Close, Symbol2Open, Symbol2Close, PriceDate) \
								SELECT PAIRS.Symbol1 AS Symbol1, PAIRS.Symbol2 AS Symbol2, \
							    TestPrices1.Open AS Symbol1Open, TestPrices1.Close AS Symbol1Close, \
								TestPrices.Open AS Symbol2Open, TestPrices.Close AS Symbol2Close, \
								TestPrices.Date AS PriceDate \
								FROM TestPrices1, TestPrices, PAIRS \
								WHERE TestPrices1.Symbol = PAIRS.Symbol1 AND TestPrices.Symbol = PAIRS.Symbol2 AND TestPrices1.Date = TestPrices.Date \
								ORDER BY PAIRS.Symbol1, PAIRS.Symbol2, TestPrices1.Date; ALTER TABLE Trade ADD Profit float;");
    if (QueryTable(pair_query_table, db) == -1)
    {
        return -1;
    }
}