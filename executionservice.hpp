/**
 * executionservice.hpp
 * Defines the data types and Service for executions.
 *
 * @author Breman Thuraisingham
 */
#ifndef EXECUTION_SERVICE_HPP
#define EXECUTION_SERVICE_HPP

#include <string>
#include "soa.hpp"
#include "marketdataservice.hpp"
//#include "executionAlgoservice.hpp"

using namespace std;


enum OrderType { FOK, IOC, MARKET, LIMIT, STOP };

enum Market { BROKERTEC, ESPEED, CME };

/**
 * An execution order that can be placed on an exchange.
 * Type T is the product type.
 */
template<typename T>
class ExecutionOrder
{

public:

  // ctor for an order
  ExecutionOrder(const T &_product, PricingSide _side, string _orderId, OrderType _orderType, double _price, double _visibleQuantity, double _hiddenQuantity, string _parentOrderId, bool _isChildOrder);
  ExecutionOrder() = default;
  // Get the product
  const T& GetProduct() const;

  const PricingSide GetSide() const {
	  return side;
  }

  string order_to_string() {
	  string s1 = product.GetProductId();
	  string s2;
	  if (side == BID)
		  s2 = "BID";
	  else
		  s2 = "OFFER";
	  return s1 + "," + s2 + "," + orderId + "," + "MARKET" + "," + to_string(price) + "," + to_string(visibleQuantity) + "," + to_string(hiddenQuantity) + "," + parentOrderId + "," + "FALSE";



  }


  // Get the order ID
  const string& GetOrderId() const;

  // Get the order type on this order
  OrderType GetOrderType() const;

  // Get the price on this order
  double GetPrice() const;

  // Get the visible quantity on this order
  long GetVisibleQuantity() const;

  // Get the hidden quantity
  long GetHiddenQuantity() const;

  // Get the parent order ID
  const string& GetParentOrderId() const;

  // Is child order?
  bool IsChildOrder() const;

private:
  T product;
  PricingSide side;
  string orderId;
  OrderType orderType;
  double price;
  double visibleQuantity;
  double hiddenQuantity;
  string parentOrderId;
  bool isChildOrder;

};

/**
 * Service for executing orders on an exchange.
 * Keyed on product identifier.
 * Type T is the product type.
 */

template<typename T>
class ExecutionListener;




template<typename T>
class ExecutionService : public Service<string,ExecutionOrder <T> >
{
private:
	map<string, ExecutionOrder<T>> ordermap;
	vector<ServiceListener<ExecutionOrder<T>>*> listeners;
	ExecutionListener<T>* listener;

public:
	ExecutionService() {
		listener = new ExecutionListener<T>(this);
		listeners = vector<ServiceListener<ExecutionOrder<T>>*>();
		ordermap = map<string, ExecutionOrder<T>>();
		
	}
	~ExecutionService() {
		delete listener;
	}


	// Get data on our service given a key
	virtual ExecutionOrder<T>& GetData(string key)
	{
		return ordermap[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(ExecutionOrder<T> &data)
	{
		ordermap[data.GetProduct().GetProductId()] = data;
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<ExecutionOrder<T>> *listener)
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<ExecutionOrder<T>>* >& GetListeners()const
	{
		return listeners;
	}

	ExecutionListener<T>* GetListener() {
		return listener;
	}

  // Execute an order on a market
	void ExecuteOrder( ExecutionOrder<T>& order, Market market) {
		for (auto& l : listeners)
			l->ProcessAdd(order);

  }

};


template<typename T>
class ExecutionListener :public ServiceListener<ExecutionOrder<T>> {
private:
	ExecutionService<T>* exe;
public:
	ExecutionListener(ExecutionService<T>* s) {
		exe = s;
	}
	~ExecutionListener() {}
	void ProcessAdd(ExecutionOrder<T> &data)
	{
		exe->ExecuteOrder(data,CME);
	}

	// Listener callback to process a remove event to the Service
	virtual void ProcessRemove(ExecutionOrder<T> &data) {}

	// Listener callback to process an update event to the Service
	virtual void ProcessUpdate(ExecutionOrder<T> &data) {}



};













template<typename T>
ExecutionOrder<T>::ExecutionOrder(const T &_product, PricingSide _side, string _orderId, OrderType _orderType, double _price, double _visibleQuantity, double _hiddenQuantity, string _parentOrderId, bool _isChildOrder) :
  product(_product)
{
  side = _side;
  orderId = _orderId;
  orderType = _orderType;
  price = _price;
  visibleQuantity = _visibleQuantity;
  hiddenQuantity = _hiddenQuantity;
  parentOrderId = _parentOrderId;
  isChildOrder = _isChildOrder;
}

template<typename T>
const T& ExecutionOrder<T>::GetProduct() const
{
  return product;
}

template<typename T>
const string& ExecutionOrder<T>::GetOrderId() const
{
  return orderId;
}

template<typename T>
OrderType ExecutionOrder<T>::GetOrderType() const
{
  return orderType;
}

template<typename T>
double ExecutionOrder<T>::GetPrice() const
{
  return price;
}

template<typename T>
long ExecutionOrder<T>::GetVisibleQuantity() const
{
  return visibleQuantity;
}

template<typename T>
long ExecutionOrder<T>::GetHiddenQuantity() const
{
  return hiddenQuantity;
}

template<typename T>
const string& ExecutionOrder<T>::GetParentOrderId() const
{
  return parentOrderId;
}

template<typename T>
bool ExecutionOrder<T>::IsChildOrder() const
{
  return isChildOrder;
}

#endif
