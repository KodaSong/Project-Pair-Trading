#include "Profit.h"
#include <fstream>
using namespace std;

int main()
{
	//// Remove previous db
	//remove("MyDB.db");

	// Initialization
	sqlite3* db = NULL;
	const char* db_name = "MyDB.db";
	string filename;	filename = "PairTrading.txt";	// For test purpose
	set<string> pairSymbols;		
	map<int, pair<string, string> > pairMap;	
	vector<Stock> StockList_History, StockList_Recent;	// One for Calculating PriceRatio, One for Backtesting
	map<string, Stock> StockMap_History, StockMap_Recent;
	
	vector<float> PriceRatio;
	vector<Trade> Trade_History, Trade_Recent;
	map<int, int> Profit_Trades, Loss_Trades;	
	map<int, float> ProfitRatio, Profit;
	int k = 1, flagB = 1, flagC = 1, flagD = 1, flagE = 1;
	string input;

	cout << "============Welcome to Pair Trading Project=============" << endl;

	while (true)
	{
		cout << "=========================Menu===========================" << endl;
		cout << "A - Create and Populate Pair Table" << endl;
		cout << "B - Retrieve and Populate Historical Data for Each Stock" << endl;
		cout << "C - Create PairPrices Table" << endl;
		cout << "D - Calculate Volatility" << endl;
		cout << "E - Retrieve Data From Tables" << endl;
		cout << "F - Back Test" << endl;
		cout << "G - Calculate Profit and Loss for Each Pair" << endl;
		cout << "H - Manual Testing" << endl;
		cout << "X - Exit" << endl;
		cout << "Please input your command: " << endl;

		cin >> input;

		if (input == "A") { // A - Create and Populate Pair Table
			OpenDatabase(db_name, db);
			Gen_Pair(filename, pairSymbols, pairMap);	// Generate a PairMap <Ticker1, Ticker2>
			//CreatePairsTable(db, pairMap);
		}

		else if (input == "B") {  // B - Retrieve and Populate Historical Data for Each Stock
			if (flagB == 1)
			{
				CreatePricesTable(db, StockMap_History, pairSymbols, StockList_History, "2009-01-01", "2019-12-31");	// Create Table "Prices"
				CreateTestPricesTable(db, StockMap_Recent, pairSymbols, StockList_Recent, "2020-01-01", "2020-01-31");	// Create Table "TestPrices"
				flagB = 0;
			}
			else cout << "Prices and TestPrices Table Created Successfully!" << endl << endl;
		}

		else if (input == "C") {  // C - Create PairPrices Table
			if (flagC == 1)
			{
				CreatePairPricesTable(db);	// Match each PAIRS's prices
				CreateTradeTable(db);	// Match each Pair's prices for backtesting
				flagC = 0;
			}
			else cout << "PairPrices and Trade Table Created Successfully!" << endl << endl;
		}

		else if (input == "D") {   // D - Calculate Violatility
			if (flagD == 1)
			{
				CreatePriceRatioTable(db);	
				flagD = 0;
			}
			else cout << "PriceRatio Created Successfully!" << endl << endl;
		}

		else if (input == "E")	// Retrieve Values From Table 
		{
			if (flagE == 1)
			{
				Retrieve_PriceRatio_Table(db, PriceRatio);	// Retrieve PriceRatio and store in vector
				Retrieve_PairPrices_Table(db, Trade_History, pairMap);
				Retrieve_Trade_Table(db, Trade_Recent, pairMap);
				flagE = 0;
			}
			cout << "PairPrices and Trade Table Retrieved successfully!" << endl;
		}

		else if (input == "F") {  // F - Calculate Historical trades' profit
			cout << "Setting k, please input k:" << endl;
			cin >> k;
			//Retrieve_PairPrices_Table(db, Trade_History, pairMap);
			Calculate_Profit(PriceRatio, Trade_History, k, db);
		}

		else if (input == "G") { // G - Calculate Profit and Loss for Each Pair
			cout << "Setting k, please input k:" << endl;
			cin >> k;
			
			BackTest(PriceRatio, Trade_Recent, Profit_Trades,
				Loss_Trades, ProfitRatio, Profit, k, db);

			cout << "Pair No." << " || " << "Number of Profit Trades" << " || " << "Number of Loss Trades" << " || "
				<< "Profitable Ratio" << " || " << "Total P/L" << endl;
			for (int i = 1; i < 20; i++)
				cout << right << setw(4) << i << setw(8) << " || " << setw(10) << Profit_Trades[i] << setw(17)
				<< " || " << setw(10) << Loss_Trades[i] << setw(15) << " || " << setw(10)
				<< fixed << setprecision(2) << Profit_Trades[i] * 1.0 / (Loss_Trades[i] * 1.0) << setw(10) << " || " << Profit[i] << endl;
			cout << endl;
		}

		else if (input == "H") { // H - Manual Testing
			int q1 = 10000, k;
			double pl = 0.0;
			int pairNo;
			double D1C1, D1C2, D2O1, D2O2, D2C1, D2C2;
			cout << "Please input k: " << endl;
			cin >> k;

			cout << "Please select tickers from the following list:" << endl;
			cout << "Pair No." << " || " << "Ticker1" << " || " << "Ticker2" << endl;
			for (auto it = pairMap.begin(); it != pairMap.end(); it++)
			{
				cout << setw(4) << it->first << setw(8) << " || " << setw(6) << it->second.first << setw(5) << " || " << setw(6) << it->second.second << endl;
			}
			cout << "Please input Pair No.: " << endl; //get volatility by using Pair No.
			cin >> pairNo;
			double v = PriceRatio[pairNo - 1];

			cout << "Please input Day1 Close, Day2 Open, Day2 Close for Ticker1:" << endl;
			cin >> D1C1 >> D2O1 >> D2C1;
			cout << "Please input Ticker2, Day1 Close, Day2 Open, Day2 Close for Ticker2:" << endl;
			cin >> D1C2 >> D2O2 >> D2C2;

			int q2 = (int)(q1 * D2O1 / D2O2);
			if (abs(D1C1 / D1C2 - D2O1 / D2O2) > k* v)
				pl = -q1 * (D2C1 - D2O1) + q2 * (D2C2 - D2O2);
			else
				pl = q1 * (D2C1 - D2O1) - q2 * (D2C2 - D2O2);
			cout << "Profit/Loss = " << setprecision(2) << fixed << pl << endl;
			cout << endl;
		}

		else if (input == "X") { // X - Exit
			CloseDatabase(db);
			cout << "Thank you for using!" << endl;
			break;
		}
		else {
			cout << "Warning! Wrong input! Try again." << endl;
		}
	}


	//Gen_Pair(filename, pairSymbols, pairMap);	// Generate a PairMap <Ticker1, Ticker2>
	//OpenDatabase(db_name, db);	// Create Database

	//// Download Data, Create Tables and Insert Values 
	//CreatePairsTable(db, pairMap);	// Create Table "PAIRS" and "PAIRS_Test"
	//CreatePricesTable(db, StockMap_History, pairSymbols, StockList_History, "2019-10-01", "2019-12-31");	// Create Table "Prices"
	//CreateTestPricesTable(db, StockMap_Recent, pairSymbols, StockList_Recent, "2020-01-01", "2020-01-31");	// Create Table "TestPrices"

	//CreatePriceRatioTable(db);	// Calculate PriceRatio
	//CreatePairPricesTable(db);	// Match each PAIRS's prices
	//CreateTradeTable(db);	// Backtesting Table

	//// Retrieve Data from Tables
	//Retrieve_PriceRatio_Table(db, PriceRatio);	// Retrieve PriceRatio and store in vector
	//cout << PriceRatio[0] << endl;

 //	Retrieve_PairPrices_Table(db, Trade_History, pairMap);
	//Calculate_Profit(PriceRatio, Trade_History, k, db);	// Calculate each trade's profit


	//Retrieve_Trade_Table(db, Trade_Recent, pairMap);
	//BackTest(PriceRatio, Trade_Recent, Profit_Trades, Loss_Trades, ProfitRatio, Profit, k, db);
	//
	//for (int i = 1; i < 20; i++)
	//	cout << i << " " << Profit_Trades[i] << " " << Loss_Trades[i] << " " << Profit[i] << endl;

	//CloseDatabase(db);

	return 0;
}