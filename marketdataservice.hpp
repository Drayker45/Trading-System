/**
 * marketdataservice.hpp
 * Defines the data types and Service for order book market data.
 *
 * @author Breman Thuraisingham
 */
#ifndef MARKET_DATA_SERVICE_HPP
#define MARKET_DATA_SERVICE_HPP

#include <string>
#include <vector>
#include "soa.hpp"
#include<unordered_map>


using namespace std;

// Side for market data
enum PricingSide { BID, OFFER };

/**
 * A market data order with price, quantity, and side.
 */
class Order
{

public:

  // ctor for an order
  Order(double _price, long _quantity, PricingSide _side);
  Order() = default;
  // Get the price on the order
  double GetPrice() const;

  // Get the quantity on the order
  long GetQuantity() const;

  // Get the side on the order
  PricingSide GetSide() const;

private:
  double price;
  long quantity;
  PricingSide side;

};

/**
 * Class representing a bid and offer order
 */
class BidOffer
{

public:

  // ctor for bid/offer
  BidOffer(const Order &_bidOrder, const Order &_offerOrder);
  BidOffer() = default;
  // Get the bid order
  const Order& GetBidOrder() const;

  // Get the offer order
  const Order& GetOfferOrder() const;

private:
  Order bidOrder;
  Order offerOrder;

};

/**
 * Order book with a bid and offer stack.
 * Type T is the product type.
 */
template<typename T>
class OrderBook
{

public:

  // ctor for the order book
  OrderBook(const T &_product, const vector<Order> &_bidStack, const vector<Order> &_offerStack);
  OrderBook() = default;
  // Get the product
  const T& GetProduct() const;

  // Get the bid stack
  const vector<Order>& GetBidStack() const;

  // Get the offer stack
  const vector<Order>& GetOfferStack() const;
  const BidOffer& GetBidOffer() const {
	  double priceb = 0;
	  double priceo = 200;
	  Order bid;
	  Order offer;
	  for (auto& b : bidStack)
	  {
		  if (b.GetPrice() > priceb)
		  {
			  bid = b;
			  priceb = b.GetPrice();
		  }
	  }
	  for (auto& o : offerStack)
	  {
		  if (o.GetPrice() < priceo)
		  {
			  offer = o;
			  priceb = o.GetPrice();
		  }
	  }

	  return BidOffer(bid, offer);




  }

private:
  T product;
  vector<Order> bidStack;
  vector<Order> offerStack;

};

/**
 * Market Data Service which distributes market data
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class MarketConnector;

template<typename T>
class MarketDataService : public Service<string,OrderBook <T> >
{
private:
	map<string, OrderBook<T>> markettable;
	MarketConnector<T>* connector;
	vector<ServiceListener<OrderBook<T>>*> listeners;
	int Depth;

public:
	MarketDataService() {
		markettable = map<string, OrderBook<T>>();
		connector = new MarketConnector<T>(this);
		listeners = vector<ServiceListener<OrderBook<T>>*>();
		Depth = 5;
	}
	~MarketDataService() { delete connector; }
	MarketConnector<T>* GetConnector() {
		return connector;
	}


	// Get data on our service given a key
	virtual OrderBook<T>& GetData(string key)
	{
		return markettable[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(OrderBook<T> &data)
	{
		T temp = data.GetProduct();
		string stemp = temp.GetProductId();
		OrderBook<T> ptemp = data;
		markettable[stemp] = ptemp;
		for (auto& listener : listeners)
			listener->ProcessAdd(data);
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<OrderBook<T>> *listener)
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<OrderBook<T>>* >& GetListeners()const
	{
		return listeners;
	}







  // Get the best bid/offer order
	virtual const BidOffer& GetBestBidOffer(const string &productId) {
		return  markettable[productId].GetBidOffer();
  }

  // Aggregate the order book
	virtual const OrderBook<T>& AggregateDepth(const string &productId) {
		T p=markettable[productId].GetProduct();		
		unordered_map<double,long> bidmap;
		unordered_map<double, long> offermap;
		for (auto& bid : markettable[productId].GetBidStack())
		{
			bidmap[bid.GetPrice()] += bid.GetQuantity();
		}
		for (auto& offer : markettable[productId].GetOfferStack())
		{
			offermap[offer.GetPrice()] += offer.GetQuantity();
		}
		vector<Order> bidbook;
		vector<Order> offerbook;
		for (auto& bid : bidmap)
		{
			Order order(bid.first, bid.second, BID);
			bidbook.push_back(order);
		}
		for (auto& offer : offermap)
		{
			Order order(offer.first, offer.second, OFFER);
			offerbook.push_back(order);
		}


		return OrderBook<T>(p, bidbook, offerbook);

  }






};
template<typename T>
class MarketConnector :public Connector<OrderBook<T>> {
private:
	MarketDataService<T>* service;
public:
	MarketConnector(MarketDataService<T>* service) :service(service) {}
	~MarketConnector() {}
	void Subscribe(ifstream& file) {
		string line;
		stringstream linestream;
		cout << "Market data loading......" << endl;
		
		static long number = 0;
		vector<Order> bidstack;
		vector<Order> offerstack;
		while (getline(file, line))
		{
			linestream.clear();
			linestream.str(line);
			string temp;
			vector<string> component;
			while (getline(linestream, temp, ','))
				component.push_back(temp);
			date d(2018, Nov, 25);
			T bond(component[0], CUSIP, "T", 0, d);
			double mid = convert(component[1]);
			double spread =  1.0 / 256.0 * (8 - abs(number % 6 - 3) * 2);
			long quantity = stol(component[3]);
			Order ordero(mid + spread / 2, quantity, OFFER);
			Order orderb(mid - spread / 2, quantity, BID);
			bidstack.push_back(orderb);
			offerstack.push_back(ordero);
			number++;
			
			if (number % 5 == 0) {
				OrderBook<T> book(bond, bidstack, offerstack);

				service->OnMessage(book);
				bidstack.clear();
				offerstack.clear();

			}
		}
		cout << "market data loaded......" << endl;

	}
	void Publish(OrderBook<T> &data) {}





};




























Order::Order(double _price, long _quantity, PricingSide _side)
{
  price = _price;
  quantity = _quantity;
  side = _side;
}

double Order::GetPrice() const
{
  return price;
}
 
long Order::GetQuantity() const
{
  return quantity;
}
 
PricingSide Order::GetSide() const
{
  return side;
}

BidOffer::BidOffer(const Order &_bidOrder, const Order &_offerOrder) :
  bidOrder(_bidOrder), offerOrder(_offerOrder)
{
}

const Order& BidOffer::GetBidOrder() const
{
  return bidOrder;
}

const Order& BidOffer::GetOfferOrder() const
{
  return offerOrder;
}

template<typename T>
OrderBook<T>::OrderBook(const T &_product, const vector<Order> &_bidStack, const vector<Order> &_offerStack) :
  product(_product), bidStack(_bidStack), offerStack(_offerStack)
{
}

template<typename T>
const T& OrderBook<T>::GetProduct() const
{
  return product;
}

template<typename T>
const vector<Order>& OrderBook<T>::GetBidStack() const
{
  return bidStack;
}

template<typename T>
const vector<Order>& OrderBook<T>::GetOfferStack() const
{
  return offerStack;
}

#endif
