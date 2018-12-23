/**
 * tradebookingservice.hpp
 * Defines the data types and Service for trade booking.
 *
 * @author Breman Thuraisingham
 */
#ifndef TRADE_BOOKING_SERVICE_HPP
#define TRADE_BOOKING_SERVICE_HPP

#include <string>
#include <vector>
#include "soa.hpp"
#include "executionservice.hpp"

// Trade sides
enum Side { BUY, SELL };

/**
 * Trade object with a price, side, and quantity on a particular book.
 * Type T is the product type.
 */










template<typename T>
class Trade
{

public:

  // ctor for a trade
  Trade(const T &_product, string _tradeId, double _price, string _book, long _quantity, Side _side);
  Trade() = default;
  // Get the product
  const T& GetProduct() const;

  // Get the trade ID
  const string& GetTradeId() const;

  // Get the mid price
  double GetPrice() const;

  // Get the book
  const string& GetBook() const;

  // Get the quantity
  long GetQuantity() const;

  // Get the side
  Side GetSide() const;

private:
  T product;
  string tradeId;
  double price;
  string book;
  long quantity;
  Side side;

};
template<typename T>
class TradeBookingService;

template<typename T>
class TradeBookingConnector :public Connector<Trade<T>> {
private:
	TradeBookingService<T>* service;
public:
	TradeBookingConnector(TradeBookingService<T>* s) {
		service = s;
	}
	~TradeBookingConnector(){}
	virtual void Publish(Trade<T> &data) {}
	virtual void Subscribe(ifstream& file) {
		string line;
		stringstream linestream;
		cout << "trades data loading......" << endl;
		while (getline(file, line))
		{
			linestream.clear();
			linestream.str(line);
			string temp;
			vector<string> component;
			while (getline(linestream, temp, ','))
				component.push_back(temp);
			
			
			double price = convert(component[2]);
			string book = component[4];
			Side side;
			if (component[5] == "BUY")
				side = BUY;
			else
				side = SELL;
			date d(2018, Nov, 25);
			T bond(component[0], CUSIP, "T", 0, d);
			Trade<T> data(bond, component[1], price, book, stol(component[3]), side);
			service->BookTrade(data);
		}
		cout << "trades data loaded!" << endl;



	}





};

template<typename T>
class TradeToOrderListener;



/**
 * Trade Booking Service to book trades to a particular book.
 * Keyed on trade id.
 * Type T is the product type.
 */
template<typename T>
class TradeBookingService : public Service<string,Trade <T> >
{
private:
	map<string, Trade<T>> book;
	TradeBookingConnector<T>* connector;
	vector<ServiceListener<Trade<T>>*> listeners;
	TradeToOrderListener<T>* listener;

public:
	TradeBookingService() {
		book = map<string, Trade<T>>();
		connector = new TradeBookingConnector<T>(this);
		listeners = vector<ServiceListener<Trade<T>>*>();
		listener = new TradeToOrderListener<T>(this);
	}
	~TradeBookingService() {
		delete connector;
	}
	TradeBookingConnector<T>* GetConnector() {
		return connector;
	}
	void OnMessage(Trade<T> &data) {}

	// Get data on our service given a key
	virtual Trade<T>& GetData(string key)
	{
		return book[key];
	}

  // Book the trade
	void BookTrade( Trade<T>& trade) {
		book[trade.GetProduct().GetProductId()] = trade;
		for (auto& l : listeners)
			l->ProcessAdd(trade);
  }
	virtual void AddListener(ServiceListener<Trade<T>> *listener)
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<Trade<T>>* >& GetListeners()const
	{
		return listeners;
	}
	TradeToOrderListener<T>* GetListener() {
		return listener;
	}

};

template<typename T>
class TradeToOrderListener :public ServiceListener<ExecutionOrder<T>> {
private:
	TradeBookingService<T>* book;
public:
	TradeToOrderListener(TradeBookingService<T>* s) {
		book = s;
	}
	~TradeToOrderListener() {}
	void ProcessAdd(ExecutionOrder<T> &data)
	{
		static int i = 1; 
		Side side;
		if (data.GetSide() == BID)
			side = BUY;
		else
			side = SELL;
		string s;
		if (i % 3 == 0)
			s = "TSRY1";
		else if (i % 3 == 1)
			s = "TSRY2";
		else
			s = "TSRY3";
		Trade<T> trade(data.GetProduct(), "M"+to_string(i), data.GetPrice(), s, data.GetHiddenQuantity() + data.GetVisibleQuantity(), side);

		book->BookTrade(trade);
	}


	// Listener callback to process a remove event to the Service
	virtual void ProcessRemove(ExecutionOrder<T> &data) {}

	// Listener callback to process an update event to the Service
	virtual void ProcessUpdate(ExecutionOrder<T> &data) {}





};























template<typename T>
Trade<T>::Trade(const T &_product, string _tradeId, double _price, string _book, long _quantity, Side _side) :
  product(_product)
{
  tradeId = _tradeId;
  price = _price;
  book = _book;
  quantity = _quantity;
  side = _side;
}

template<typename T>
const T& Trade<T>::GetProduct() const
{
  return product;
}

template<typename T>
const string& Trade<T>::GetTradeId() const
{
  return tradeId;
}

template<typename T>
double Trade<T>::GetPrice() const
{
  return price;
}

template<typename T>
const string& Trade<T>::GetBook() const
{
  return book;
}

template<typename T>
long Trade<T>::GetQuantity() const
{
  return quantity;
}

template<typename T>
Side Trade<T>::GetSide() const
{
  return side;
}


#endif
