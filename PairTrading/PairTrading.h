#pragma once
#include <string>
#include <iostream>
#include <vector>

using namespace std;

class TradeData
{
private:
	string sDate;
	double dOpen;
	double dHigh;
	double dLow;
	double dClose;
	double dAdjClose;
	long lVolume;
public:
	TradeData() : sDate(""), dOpen(0), dClose(0), dHigh(0), dLow(0), dAdjClose(0), lVolume(0) {}
	TradeData(string sDate_, double dOpen_, double dHigh_, double dLow_, double dClose_, double dAdjClose_, long lVolume_) :
		sDate(sDate_), dOpen(dOpen_), dHigh(dHigh_), dLow(dLow_), dClose(dClose_), dAdjClose(dAdjClose_), lVolume(lVolume_) {}
	TradeData(const TradeData & TradeData) :sDate(TradeData.sDate), dOpen(TradeData.dOpen),
		dHigh(TradeData.dHigh), dLow(TradeData.dLow), dClose(TradeData.dClose), dAdjClose(TradeData.dAdjClose), lVolume(TradeData.lVolume) {}
	TradeData operator=(const TradeData & TradeData)
	{
		sDate = TradeData.sDate;
		dOpen = TradeData.dOpen;
		dHigh = TradeData.dHigh;
		dLow = TradeData.dLow;
		dClose = TradeData.dClose;
		dAdjClose = TradeData.dAdjClose;
		lVolume = TradeData.lVolume;

		return *this;
	}
	string getDate() { return sDate; }
	double getOpen() { return dOpen; }
	double getHigh() { return dHigh; }
	double getLow() { return dLow; }
	double getClose() { return dClose; }
	double getAdjClose() { return dAdjClose; }
	long getVolume() { return lVolume; }
	friend ostream & operator<<(ostream & ostr, const TradeData & TradeData)
	{
		ostr << TradeData.sDate << " " << TradeData.dOpen << " " << TradeData.dHigh << " " 
			 << TradeData.dLow << " " << TradeData.dClose << " " << TradeData.dAdjClose << " "
			 << TradeData.lVolume;// << endl
		return ostr;
	}
};

class Stock
{
private:
	string sSymbol;
	vector<TradeData> trades;

public:
	Stock() : sSymbol("") {}
	Stock(string sSymbol_, const vector<TradeData> trades_) :sSymbol(sSymbol_), trades(trades_) {}
	Stock(const Stock & stock) :sSymbol(stock.sSymbol), trades(stock.trades) {}
	Stock operator=(const Stock & stock)
	{
		sSymbol = stock.sSymbol;
		trades = stock.trades;

		return *this;
	}

	void addTrade(const TradeData & trade) { trades.push_back(trade); }
	string getSymbol() { return sSymbol; }
	const vector<TradeData> & getTrades() const { return trades; }

	friend ostream & operator<<(ostream & ostr, const Stock & stock)
	{
		ostr << "Symbol: " << stock.sSymbol << endl;
		for (vector<TradeData>::const_iterator itr = stock.trades.begin(); itr != stock.trades.end(); itr++)
			ostr << *itr;
		return ostr;
	}
};

class Trade
{
private:
	int iPairID;
	string sTicker1;
	string sTicker2;
	float fTicker1Open;
	float fTicker1Close;
	float fTicker2Open;
	float fTicker2Close;
	string sDate;
	float fProfit;

public:
	Trade()
	{
		iPairID = 0;	sTicker1 = "";	sTicker2 = "";
		fTicker1Open = 0.0;	fTicker1Close = 0.0;	fTicker2Open = 0.0;	fTicker2Close = 0.0;
		sDate = "";	fProfit = 0.0;
	}
	~Trade(void) {}
	Trade(int id, string pTicker1, string pTicker2, float open1, float close1, float open2, float close2, string pDate, float profit)
	{
		iPairID = id;	sTicker1 = pTicker1;	sTicker2 = pTicker2;
		fTicker1Open = open1;	fTicker1Close = close1;		fTicker2Open = open2;	fTicker2Close = close2;
		sDate = pDate;	fProfit = profit;
	}
	int iGetPairID(void) { return iPairID; }
	void SetPairID(int id) { iPairID = id; }
	string sGetTicker1(void) { return sTicker1; }
	void SetTicker1(string pTicker1) { sTicker1 = pTicker1; }
	string sGetTicker2(void) { return sTicker2; }
	void SetTicker2(string pTicker2) { sTicker2 = pTicker2; }
	string sGetDate(void) { return sDate; }
	void SetDate(string pDate) { sDate = pDate; }
	float fGetTicker1Open(void) { return fTicker1Open; }
	float fGetTicker1Close(void) { return fTicker1Close; }
	float fGetTicker2Open(void) { return fTicker2Open; }
	float fGetTicker2Close(void) { return fTicker2Close; }
	float fGetProfit(void) { return fProfit; }
	void SetProfit(float profit) { fProfit = profit; }
	friend ostream& operator<<(ostream& out, const Trade& aTrade)
	{
		out << aTrade.iPairID << " " << aTrade.sTicker1 << " "
			<< aTrade.sTicker2 << " " << aTrade.fTicker1Open << " "
			<< aTrade.fTicker1Close << " " << aTrade.fTicker2Open << " "
			<< aTrade.fTicker2Close << " " << aTrade.sDate << " "
			<< aTrade.fProfit << endl;
		/*out << aTrade.sTicker1 << " "
			<< aTrade.sTicker2 << " " << aTrade.fTicker1Open << " "
			<< aTrade.fTicker1Close << " " << aTrade.fTicker2Open << " "
			<< aTrade.fTicker2Close << " " << aTrade.sDate << " "
			<< aTrade.fProfit << endl;*/
		return out;
	}
};