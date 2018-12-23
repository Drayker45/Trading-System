#include<iostream>
#include<fstream>
#include<random>
#include<string>
#include<ctime>
//#include<sys\timeb.h>
#include<chrono>
#include "pricingservice.hpp"
#include "guiservice.hpp"
#include "streamingAlgoservice.hpp"
#include "historicaldataservice.hpp"
#include "executionAlgoservice.hpp"

using namespace std;

vector<string> bond_type = { "2Y","3Y","5Y","7Y","10Y","30Y" };

void create_prices(int n)
{
	cout << "Creating price data......" << endl;
	ofstream f;
	f.open("price.txt");
	//f << "type, mid, spread" << endl;
	for (int i = 0; i < 6; i++)
	{
		cout << i << endl;
		for (int j = 0; j < n; j++)
		{
			int x1 = 99 + rand() % 2;
			int x2 = rand() % 32;
			int x3 = rand() % 8;
			string s1 = to_string(x1) + '-';
			string s2, s3;
			if (x2 < 10)
				s2 = '0' + to_string(x2);
			else
				s2 = to_string(x2);
			if (x3 == 4)
				s3 = "+";
			else
				s3 = to_string(x3);
			string mid = s1 + s2 + s3;
			int x4 = 2 + rand() % 3;
			string spread;
			if (x4 == 4)
				spread = "0-00+";
			else
				spread = "0-00" + to_string(x4);
			f << bond_type[i] << "," << mid << "," << spread << endl;

		}
	}
	cout << "Creating data done!" << endl;
}

string timestamp()
{
	using namespace chrono;
	auto now = system_clock::now();
	auto sec = time_point_cast<seconds>(now);
	auto millisec = duration_cast<milliseconds>(now - sec);
	string m = to_string(millisec.count());
	if (millisec.count() > 100);
	else if (millisec.count() > 10)
		m = "0" + m;
	else
		m = "00" + m;
	
	time_t tv = system_clock::to_time_t(now);
	tm *local;
	local = localtime(&tv);
	/*char temp[50];
	strftime(temp, 50, "%F %T", localtime(&tv));*/

	string timestamp = "2018 "+string(asctime(local)).substr(4,15) + "." + m;
	

	return timestamp;
}

void create_trades()
{
	cout << "Creating trade data......" << endl;
	ofstream f;
	f.open("trades.txt");
	//f << "type, mid, spread" << endl;
	for (int i = 0; i < 6; i++)
	{
		cout << i << endl;
		for (int j = 0; j < 10; j++)
		{
			string id = to_string(i * 10 + j + 1);
			int x1 = 99 + rand() % 2;
			int x2 = rand() % 32;
			int x3 = rand() % 8;
			string s1 = to_string(x1) + '-';
			string s2, s3;
			if (x2 < 10)
				s2 = '0' + to_string(x2);
			else
				s2 = to_string(x2);
			if (x3 == 4)
				s3 = "+";
			else
				s3 = to_string(x3);
			string price = s1 + s2 + s3;
			int r = rand() % 3;
			string book;
			if (r == 0)
				book = "TRSY1";
			else if (r == 1)
				book = "TRSY2";
			else
				book = "TRSY3";
			string side;
			if (j % 2 == 0)
				side = "BUY";
			else
				side = "SELL";
			int q = 1000000 * (j % 5 + 1);
			f << bond_type[i] << "," << id << "," << price << "," << to_string(q) << "," << book << "," << side << endl;

		}
	}
	cout << "Creating trade done!" << endl;




}
void create_market(int n)
{
	cout << "Creating market data......" << endl;
	ofstream f;
	f.open("market.txt");
	//f << "type, mid, spread" << endl;
	for (int i = 0; i < 6; i++)
	{
		cout << i << endl;
		for (int j = 0; j < n; j++)
		{
			int x1 = 99 + rand() % 2;
			int x2 = rand() % 32;
			int x3 = rand() % 8;
			string s1 = to_string(x1) + '-';
			string s2, s3;
			if (x2 < 10)
				s2 = '0' + to_string(x2);
			else
				s2 = to_string(x2);
			if (x3 == 4)
				s3 = "+";
			else
				s3 = to_string(x3);
			string mid = s1 + s2 + s3;
			int x4 = 8 - abs(j % 6 - 3) * 2;
			string spread;
			
			if (x4 == 4)
				spread = "0-00+";
			else
				spread = "0-00" + to_string(x4);

			long position = 1000000;
			position = position * (j % 5 + 1);
			f << bond_type[i] << "," << mid << "," << spread <<"," <<to_string(position)<<endl;

		}
	}
	cout << "Creating data done!" << endl;
}

void create_inquiry()
{
	cout << "Creating inquiry data......" << endl;
	ofstream f;
	f.open("inquiry.txt");
	//f << "type, mid, spread" << endl;
	for (int i = 0; i < 6; i++)
	{
		cout << i << endl;
		for (int j = 0; j < 5; j++)
		{
			int x1 = 99 + rand() % 2;
			int x2 = rand() % 32;
			int x3 = rand() % 8;
			string s1 = to_string(x1) + '-';
			string s2, s3;
			if (x2 < 10)
				s2 = '0' + to_string(x2);
			else
				s2 = to_string(x2);
			if (x3 == 4)
				s3 = "+";
			else
				s3 = to_string(x3);
			string mid = s1 + s2 + s3;
			int x4 = 8 - abs(j % 6 - 3) * 2;
			string spread;

			if (x4 == 4)
				spread = "0-00+";
			else
				spread = "0-00" + to_string(x4);
			string buy = "BUY";
			string sell = "SELL";
			long position = 1000000;
			position = position * (j % 5 + 1);
			f << bond_type[i] << "," << buy << "," << to_string(position)<<"," << mid << "," << spread << "," << "RECEIVED" << endl;
			f << bond_type[i] << "," << sell << "," << to_string(position) <<","<< mid << "," << spread << "," << "RECEIVED" << endl;

		}
	}
	cout << "Creating data done!" << endl;
}






int main()
{
	create_prices(101);
	create_trades();
	create_market(100);
	create_inquiry();
	PricingService<Bond> pricingservice;
	GuiService<Bond> guiservice;
	StreamingAlgoService<Bond> streamingalgoservice;
	StreamingService<Bond> streamingservice;


	TradeBookingService<Bond> tradebookingservice;
	PositionService<Bond> positionservice;
	RiskService<Bond> riskservice;


	MarketDataService<Bond> marketdataservice;
	ExecutionService<Bond> executionservice;
	ExecutionAlgoService<Bond> executionalgoservice;


	InquiryService<Bond> inquiryservice;


	HistoricalDataServiceStream<Bond> historicaldataservicestream;
	HistoricalDataServicePosition<Bond> historicaldataserviceposition;
	HistoricalDataServiceRisk<Bond> historicaldataservicerisk;
	HistoricalDataServiceExecution<Bond> historicaldataserviceexecution;
	HistoricalDataServiceInquiry<Bond> historicaldataserviceinquiry;

	
	pricingservice.AddListener(guiservice.GetListener());
	pricingservice.AddListener(streamingalgoservice.GetListener());
	streamingalgoservice.AddListener(streamingservice.GetListener());
	streamingservice.AddListener(historicaldataservicestream.GetListener());

	ifstream pricefile("price.txt");
	ifstream tradefile("trades.txt");
	ifstream marketfile("market.txt");
	ifstream inquiryfile("inquiry.txt");

	tradebookingservice.AddListener(positionservice.GetListener());
	positionservice.AddListener(historicaldataserviceposition.GetListener());
	positionservice.AddListener(riskservice.GetListener());
	
	riskservice.AddListener(historicaldataservicerisk.GetListener());
	riskservice.AddListenerB(historicaldataservicerisk.GetListenerB());

	marketdataservice.AddListener(executionalgoservice.GetListener());
	executionalgoservice.AddListener(executionservice.GetListener());
	executionservice.AddListener(historicaldataserviceexecution.GetListener());
	executionservice.AddListener(tradebookingservice.GetListener());

	inquiryservice.AddListener(historicaldataserviceinquiry.GetListener());


	pricingservice.GetConnector()->Subscribe(pricefile);
	tradebookingservice.GetConnector()->Subscribe(tradefile);
	marketdataservice.GetConnector()->Subscribe(marketfile);
	inquiryservice.GetConnector()->Subscribe(inquiryfile);
	cout << "price gui done" << endl;
	
	
	
	
	system("pause");


	return 0;
}


