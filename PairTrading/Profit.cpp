#include "Profit.h"

void Calculate_Profit(vector<float>& PriceRatio, vector<Trade>& Trade_History, int k, sqlite3* db)
{
	// Add a column "Profit" into PairPrices Table and Update 
	char pair_update_table[512];

	map<int, float> Profit;
	for (auto it = Trade_History.begin(); it != Trade_History.end() - 1; it++)
	{
		int id1 = it->iGetPairID(), id2 = (it + 1)->iGetPairID();
		if (id1 == id2)
		{
			float sigma = k * PriceRatio[id1 - 1];
			float N1 = 10000.0, N2;
			float profit = 0.0;
			float Ratio = abs(it->fGetTicker1Close() / it->fGetTicker2Close() - (it + 1)->fGetTicker1Open() / (it + 1)->fGetTicker2Open());
			if (Ratio > sigma)
			{
				// Short the pair	
				N2 = N1 * (it + 1)->fGetTicker1Open() / (it + 1)->fGetTicker2Open();
				profit = N1 * ((it + 1)->fGetTicker1Open() - (it + 1)->fGetTicker1Close()) - N2 * ((it + 1)->fGetTicker1Open() - (it + 1)->fGetTicker1Close());
			}
			else if (Ratio < sigma)
			{
				// Long the pair
				N2 = N1 * (it + 1)->fGetTicker1Open() / (it + 1)->fGetTicker2Open();
				profit = -N1 * ((it + 1)->fGetTicker1Open() - (it + 1)->fGetTicker1Close()) + N2 * ((it + 1)->fGetTicker1Open() - (it + 1)->fGetTicker1Close());
			}
			cout << " " << profit << endl;
			it->SetProfit(profit);

			Profit[id1] += profit;
		}
		sprintf(pair_update_table, "UPDATE PairPrices SET Profit = %.2f WHERE Symbol1 = \"%s\" AND Symbol2 = \"%s\" AND PriceDate = \"%s\";", it->fGetProfit(), it->sGetTicker1().c_str(), it->sGetTicker2().c_str(), it->sGetDate().c_str());
		UpdateTable(pair_update_table, db);
	}

	// We also need to update the data of last row
	auto it = Trade_History.end() - 1;
	sprintf(pair_update_table, "UPDATE PairPrices SET Profit = %.2f WHERE Symbol1 = \"%s\" AND Symbol2 = \"%s\" AND PriceDate = \"%s\";", it->fGetProfit(), it->sGetTicker1().c_str(), it->sGetTicker2().c_str(), it->sGetDate().c_str());
	UpdateTable(pair_update_table, db);

	// Add a column "TotalProfit" into PairPriceRatio Table and Update 
	for (int i = 1; i < 20; i++)
	{
		sprintf(pair_update_table, "UPDATE PairPriceRatio SET TotalProfit = %.2f WHERE Id = %d;", Profit[i], i);
		UpdateTable(pair_update_table, db);
	}
}

void BackTest(vector<float>& PriceRatio, vector<Trade>& Trade_Recent,
	map<int, int>& Profit_Trades, map<int, int>& Loss_Trades, map<int, float>& ProfitRatio, map<int, float>& Profit, int k, sqlite3* db)
{
	// Add a column "Profit" into Trade Table and Update 
	char pair_update_table[512];

	for (auto it = Trade_Recent.begin(); it != Trade_Recent.end() - 1; it++)
	{
		int id1 = it->iGetPairID(), id2 = (it + 1)->iGetPairID();
		if (id1 == id2)
		{
			float sigma = k * PriceRatio[id1 - 1];
			float N1 = 10000.0, N2;	
			float profit = 0.0;
			float Ratio = abs(it->fGetTicker1Close() / it->fGetTicker2Close() - (it + 1)->fGetTicker1Open() / (it + 1)->fGetTicker2Open());
			if (Ratio > sigma)
			{
				// Short the pair	
				N2 = N1 * (it + 1)->fGetTicker1Open() / (it + 1)->fGetTicker2Open();
				profit = N1 * ((it + 1)->fGetTicker1Open() - (it + 1)->fGetTicker1Close()) - N2 * ((it + 1)->fGetTicker1Open() - (it + 1)->fGetTicker1Close());
			}
			else if (Ratio < sigma)
			{
				// Long the pair
				N2 = N1 * (it + 1)->fGetTicker1Open() / (it + 1)->fGetTicker2Open();
				profit = -N1 * ((it + 1)->fGetTicker1Open() - (it + 1)->fGetTicker1Close()) + N2 * ((it + 1)->fGetTicker1Open() - (it + 1)->fGetTicker1Close());
			}
			it->SetProfit(profit);

			// Count the Num of Profit Trades and Loss Trades
			if (profit > 0.0)	Profit_Trades[id1]++;
			else if (profit < 0.0)	Loss_Trades[id1]++;
			Profit[id1] += profit;
		}

		sprintf(pair_update_table, "UPDATE Trade SET Profit = %.2f WHERE Symbol1 = \"%s\" AND Symbol2 = \"%s\" AND PriceDate = \"%s\";", it->fGetProfit(), it->sGetTicker1().c_str(), it->sGetTicker2().c_str(), it->sGetDate().c_str());
		UpdateTable(pair_update_table, db);
	}

	// We also need to update the data of last row
	auto it = Trade_Recent.end()-1;
	sprintf(pair_update_table, "UPDATE Trade SET Profit = %.2f WHERE Symbol1 = \"%s\" AND Symbol2 = \"%s\" AND PriceDate = \"%s\";", it->fGetProfit(), it->sGetTicker1().c_str(), it->sGetTicker2().c_str(), it->sGetDate().c_str());
	UpdateTable(pair_update_table, db);

	// Add a column "TotalProfit" into PAIRS_Test Table and Update 
	for (int i = 1; i < 20; i++)
	{
		sprintf(pair_update_table, "UPDATE PAIRS_Test SET TotalProfit = %.2f WHERE Id = %d;", Profit[i], i);
		UpdateTable(pair_update_table, db);
	}
}