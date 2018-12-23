#ifndef STREAMING_ALGO_SERVICE_HPP
#define STREAMING_ALGO_SERVICE_HPP



#include "streamingservice.hpp"
#include "pricingservice.hpp"

using namespace std;

template<typename T>
class StreamingAlgoListener;



template<typename T>
class StreamingAlgoService : public Service<string, PriceStream <T> >
{
private:
	map<string, PriceStream<T>> streammap;
	vector<ServiceListener<PriceStream<T>>*> listeners;
	StreamingAlgoListener<T>* listener;
public:
	StreamingAlgoService() {
		listener = new StreamingAlgoListener<T>(this);
		listeners = vector<ServiceListener<PriceStream<T>>*>();
		streammap = map<string, PriceStream<T>>();
	}
	~StreamingAlgoService() {
		delete listener;
	}
	 

	// Get data on our service given a key
	virtual PriceStream<T>& GetData(string key)
	{
		return streammap[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(PriceStream<T> &data)
	{
		streammap[data.GetProduct().GetProductId()] = data;
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<PriceStream<T>> *listener)
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<PriceStream<T>>* >& GetListeners()const
	{
		return listeners;
	}

	virtual void PublishPrice( PriceStream<T>& priceStream) {
		for (auto& l : listeners)
			l->ProcessAdd(priceStream);
	}
	StreamingAlgoListener<T>* GetListener() {
		return listener;
	}

};

template<typename T>
class StreamingAlgoListener :public ServiceListener<Price<T>> {
private:
	StreamingAlgoService<T>* streamalgo;
public:
	StreamingAlgoListener(StreamingAlgoService<T>* s) {
		streamalgo = s;
	}
	~StreamingAlgoListener(){}
	void ProcessAdd(Price<T> &data)
	{
		PriceStreamOrder bid(data.GetMid() - data.GetBidOfferSpread() / 2, 1000000, 2000000, BID);
		PriceStreamOrder ask(data.GetMid() + data.GetBidOfferSpread() / 2, 1000000, 2000000, OFFER);
		PriceStream<T> P(data.GetProduct(), bid, ask);
		streamalgo->PublishPrice(P);
	}

	// Listener callback to process a remove event to the Service
	virtual void ProcessRemove(Price<T> &data) {}

	// Listener callback to process an update event to the Service
	virtual void ProcessUpdate(Price<T> &data) {}


};
#endif