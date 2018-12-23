#ifndef EXECUTION_ALGO_SERVICE_HPP
#define EXECUTION_ALGO_SERVICE_HPP



#include "executionservice.hpp"
#include "marketdataservice.hpp"
#include<map>

using namespace std;

template<typename T>
class ExecutionAlgoListener;



template<typename T>
class ExecutionAlgoService : public Service<string,ExecutionOrder<T> >
{
private:
	map<string, ExecutionOrder<T>> ordermap;
	vector<ServiceListener<ExecutionOrder<T>>*> listeners;
	ExecutionAlgoListener<T>* listener;
public:
	ExecutionAlgoService() {
		listener = new ExecutionAlgoListener<T>(this);
		listeners = vector<ServiceListener<ExecutionOrder<T>>*>();
		ordermap = map<string, ExecutionOrder<T>>();
	}
	~ExecutionAlgoService() {
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

	virtual void ExecuteOrder(ExecutionOrder<T>& executionorder) {
		for (auto& l : listeners)
			l->ProcessAdd(executionorder);
	}
	ExecutionAlgoListener<T>* GetListener() {
		return listener;
	}

};

template<typename T>
class ExecutionAlgoListener :public ServiceListener<OrderBook<T>> {
private:
	ExecutionAlgoService<T>* exealgo;
public:
	ExecutionAlgoListener(ExecutionAlgoService<T>* s) {
		exealgo = s;
	}
	~ExecutionAlgoListener() {}
	void ProcessAdd(OrderBook<T> &data)
	{
		static int i = 1;
		Order bid=data.GetBidOffer().GetBidOrder();
		Order offer = data.GetBidOffer().GetOfferOrder();
		if (offer.GetPrice() - bid.GetPrice() < 1 / 127)
		{
			ExecutionOrder<T> order1(data.GetProduct(), OFFER, to_string(i), MARKET, bid.GetPrice(), bid.GetQuantity() / 3, bid.GetQuantity() - bid.GetQuantity() / 3, to_string(i), false);
			i++;
			ExecutionOrder<T> order2(data.GetProduct(), BID, to_string(i), MARKET, offer.GetPrice(), offer.GetQuantity() / 3, offer.GetQuantity() - offer.GetQuantity() / 3, to_string(i), false);
			i++;
			exealgo->ExecuteOrder(order1);
			exealgo->ExecuteOrder(order2);
		}

		
	}

	// Listener callback to process a remove event to the Service
	virtual void ProcessRemove(OrderBook<T> &data) {}

	// Listener callback to process an update event to the Service
	virtual void ProcessUpdate(OrderBook<T> &data) {}


};




#endif