#include "RetrieveData.h"

void Gen_Pair(string filename, set<string>& pairSymbols, map<int, pair<string, string> >& pairMap)
{

	ifstream inFile(filename, ios::in);
	string line;
	int Id = 1;
	while (getline(inFile, line))
	{
		// First, insert Symbols into set
		string ticker1 = line.substr(0, line.find_first_of(','));
		string ticker2 = line.substr(line.find_first_of(',') + 1);
		pairSymbols.insert(ticker1);
		pairSymbols.insert(ticker2);

		// Second, insert pairs into pairMap -> in order to create table
		pairMap[Id] = make_pair(ticker1, ticker2);
		Id++;
	}

}

void Gen_StockMap(vector<Stock>& StockList, map<string, Stock>& StockMap)
{
	for (vector<Stock>::iterator itr = StockList.begin(); itr != StockList.end(); itr++)
		StockMap[itr->getSymbol()] = *itr;
}

int RetrieveData(set<string>& pairSymbols, vector<Stock>& StockList, string StartDate, string EndDate)
{
	for (set<string>::const_iterator itr = pairSymbols.begin(); itr != pairSymbols.end(); itr++)
	{
		cout << *itr << endl;
		string readBuffer;
		string stock_url_common = "https://eodhistoricaldata.com/api/eod/";
		string stock_start_date = StartDate;
		string stock_end_date = EndDate;
		string api_token = "5ba84ea974ab42.45160048";
		string pair_stock_retrieve_url = stock_url_common + *itr + ".US?" +
			"from=" + stock_start_date + "&to=" + stock_end_date + "&api_token=" + api_token + "&period=d";
		if (RetrieveMarketData(pair_stock_retrieve_url, readBuffer) == -1)
			return -1;

		vector<TradeData> trade_vec;
		Split_Buffer(readBuffer, trade_vec);

		Stock stock(*itr, trade_vec);
		StockList.push_back(stock);
	}
	return 0;
}

//void  Split_Buffer(string readBuffer, vector<TradeData>& trade_vec)
//{
//	//vector<TradeData> trade_vec;	// Store each line (each day) data
//	string Date; long Volume;
//	double Open; double High; double Low; double Close; double Adj_Close;
//	string line;
//	line = readBuffer.erase(0, readBuffer.find_first_of('\n') + 1);		// erase the header
//	while (true)
//	{
//		Date = line.substr(0, line.find_first_of(','));		line.erase(0, line.find_first_of(',') + 1);
//		int index1 = line.find_first_of(',');
//		if (index1 == 0)		// We found there are many missing data ",,,,,,,,"
//		{
//			int index2 = line.find_first_of('\n');
//			line.erase(0, index2);
//			continue;
//		}
//		Open = stod(line.substr(0, index1));		line.erase(0, line.find_first_of(',') + 1);
//		High = stod(line.substr(0, line.find_first_of(',')));		line.erase(0, line.find_first_of(',') + 1);
//		Low = stod(line.substr(0, line.find_first_of(',')));		line.erase(0, line.find_first_of(',') + 1);
//		Close = stod(line.substr(0, line.find_first_of(',')));		line.erase(0, line.find_first_of(',') + 1);
//		Adj_Close = stod(line.substr(0, line.find_first_of(',')));		line.erase(0, line.find_first_of(',') + 1);
//		Volume = stol(line.substr(0, line.find_first_of(',')));		line.erase(0, line.find_first_of('\n') + 1);	// Next line
//		if (Volume == 0)
//			continue;
//
//		TradeData data(Date, Open, High, Low, Close, Adj_Close, Volume);
//		trade_vec.push_back(data);
//
//		if (line.length() < 10)	// Last line, break out while loop
//			break;
//	}
//}

void  Split_Buffer(string readBuffer, vector<TradeData>& trade_vec)
{
	//vector<TradeData> trade_vec;    // Store each line (each day) data
	string Date, word, line;
	long Volume;
	vector<string> row;
	double Open, High, Low, Close, AdjClose;

	stringstream ss(readBuffer);
	getline(ss, line, '\n');	// erase the header by finding '\n'

	while (true)
	{
		getline(ss, line);	// Store in line by finding '\n' -> default 
		if (line.length() <= 10) break;	// Last line is a integer, which is the point we break the loop
		if (ss.fail()) break;
		row.clear();	// Clear the vector
		stringstream sss(line);		// Store information of line into sss
		while (getline(sss, word, ','))	// Split sss by ',' and store in word, each word will be pushe back to vector "row"
			row.push_back(word);			

		if (row[0] != "" && row[1] != "" && row[2] != "" && row[3] != "" && row[4] != "" && row[5] != "" && row[6] != "")
		{
			Date = row[0]; Open = stod(row[1]); High = stod(row[2]); Low = stod(row[3]);
			Close = stod(row[4]); AdjClose = stod(row[5]); Volume = stol(row[6]);
			if (Volume == 0)
				continue;
			TradeData data(Date, Open, High, Low, Close, AdjClose, Volume);
			trade_vec.push_back(data);
		}
	}
}