
/**
 * positionservice.hpp
 * Defines the data types and Service for positions.
 *
 * @author Breman Thuraisingham
 */
#ifndef POSITION_SERVICE_HPP
#define POSITION_SERVICE_HPP

#include <string>
#include <map>
#include "soa.hpp"
#include "tradebookingservice.hpp"
#include "products.hpp"

using namespace std;

/**
 * Position class in a particular book.
 * Type T is the product type.
 */
template<typename T>
class Position
{

public:

  // ctor for a position
  Position(const T &_product);
  Position() = default;
  Position(const T &_product, long q1, long q2, long q3) :
	  product(_product) 
  {
	  positions = map<string, long>();
	  positions["TRSY1"] = q1;
	  positions["TRSY2"] = q2;
	  positions["TRSY3"] = q3;
  }
  string Position_to_String() {
	  string s1 = product.GetProductId();
	  string s2 = to_string(positions["TRSY1"]);
	  string s3 = to_string(positions["TRSY2"]);
	  string s4 = to_string(positions["TRSY3"]);
	  string s5 = to_string(GetAggregatePosition());
	  return s1 + "," + s2 + "," + s3 + "," + s4 + "," + s5;

  }


  // Get the product
  const T& GetProduct() const;

  // Get the position quantity
  long GetPosition(string &book);

  // Get the aggregate position
  long GetAggregatePosition();

private:
  T product;
  map<string,long> positions;

};
template<typename T>
class PositionService;


template<typename T>
class PositionToBookingListener :public ServiceListener<Trade<T>> {

private:
	PositionService<T>* service;

public:
	PositionToBookingListener(PositionService<T>* s) {
		service = s;
	}
	~PositionToBookingListener(){}

	// Listener callback to process an add event to the Service
	void ProcessAdd(Trade<T> &data)
	{
		service->AddTrade(data);
	}

	// Listener callback to process a remove event to the Service
	virtual void ProcessRemove(Trade<T> &data) {}

	// Listener callback to process an update event to the Service
	virtual void ProcessUpdate(Trade<T> &data) {}
};


/**
 * Position Service to manage positions across multiple books and secruties.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class PositionService : public Service<string,Position <T> >
{
private:
	map<string, Position<T>> position;
	vector<ServiceListener<Position<T>>*> listeners;
	PositionToBookingListener<T>* listener;

public:
	PositionService() {
		position = map<string, Position<T>>();
		listeners = vector<ServiceListener<Position<T>>*>();
		listener =new PositionToBookingListener<T>(this);
		date d(2018, Nov, 25);
		T bond1("2Y", CUSIP, "T", 0, d);
		position["2Y"] = Position<T>(bond1);
		T bond2("3Y", CUSIP, "T", 0, d);
		position["3Y"] = Position<T>(bond2);
		T bond3("5Y", CUSIP, "T", 0, d);
		position["5Y"] = Position<T>(bond3);
		T bond4("7Y", CUSIP, "T", 0, d);
		position["7Y"] = Position<T>(bond4);
		T bond5("10Y", CUSIP, "T", 0, d);
		position["10Y"] = Position<T>(bond5);
		T bond6("30Y", CUSIP, "T", 0, d);
		position["30Y"] = Position<T>(bond6);


  }
	~PositionService() { delete listener; }
  // Add a trade to the service
	virtual void AddTrade(const Trade<T> &trade) {
		string book = trade.GetBook();
		T product = trade.GetProduct();
		string bond = product.GetProductId();
		long quantity = trade.GetQuantity();
		Side side = trade.GetSide();
		if (side == SELL)
			quantity = -quantity;
		vector<long> q(3, 0);
		if (book == "TRSY1")
			q[0] += quantity;
		else if (book == "TRSY2")
			q[1] += quantity;
		else
			q[2] += quantity;
		string s1("TRSY1");
		string s2("TRSY2");
		string s3("TRSY3");

		Position<T> p(product, q[0]+position[bond].GetPosition(s1), q[1]+position[bond].GetPosition(s2), q[2]+position[bond].GetPosition(s3));
		position[bond] = p;
		for (auto& l : listeners)
			l->ProcessAdd(p);
  }
	// Get data on our service given a key
	Position<T>& GetData(string key) {
		return position[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(Position<T> &data){}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<Position<T>> *listener) {
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<Position<T>>* >& GetListeners() const {
		return listeners;
	}
	ServiceListener<Trade<T>>* GetListener() {
		return listener;
	}

};








template<typename T>
Position<T>::Position(const T &_product) :
  product(_product)
{
	positions = map<string, long>();
	positions["TRSY1"] = 0;
	positions["TRSY2"] = 0;
	positions["TRSY3"] = 0;
}

template<typename T>
const T& Position<T>::GetProduct() const
{
  return product;
}

template<typename T>
long Position<T>::GetPosition(string &book)
{
  return positions[book];
}

template<typename T>
long Position<T>::GetAggregatePosition()
{
  // No-op implementation - should be filled out for implementations
	return positions["TRSY1"] + positions["TRSY2"] + positions["TRSY3"];
}

#endif
