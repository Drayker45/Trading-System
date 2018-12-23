/**
 * streamingservice.hpp
 * Defines the data types and Service for price streams.
 *
 * @author Breman Thuraisingham
 */
#ifndef STREAMING_SERVICE_HPP
#define STREAMING_SERVICE_HPP

#include "soa.hpp"
#include "marketdataservice.hpp"

/**
 * A price stream order with price and quantity (visible and hidden)
 */
class PriceStreamOrder
{

public:

  // ctor for an order
  PriceStreamOrder(double _price, long _visibleQuantity, long _hiddenQuantity, PricingSide _side);
  PriceStreamOrder() = default;
  // The side on this order
  PricingSide GetSide() const {
	  return side;
  }

  // Get the price on this order
  double GetPrice() const;

  // Get the visible quantity on this order
  long GetVisibleQuantity() const;

  // Get the hidden quantity on this order
  long GetHiddenQuantity() const;

private:
  double price;
  long visibleQuantity;
  long hiddenQuantity;
  PricingSide side;

};

/**
 * Price Stream with a two-way market.
 * Type T is the product type.
 */
template<typename T>
class PriceStream
{

public:

  // ctor
  PriceStream(const T &_product, const PriceStreamOrder &_bidOrder, const PriceStreamOrder &_offerOrder);
  PriceStream() = default;
  // Get the product
  const T& GetProduct() const;

  // Get the bid order
  const PriceStreamOrder& GetBidOrder() const;

  // Get the offer order
  const PriceStreamOrder& GetOfferOrder() const;

  string stream_to_string_bid() {
	  string s1 = product.GetProductId();
	  string s2 = to_string(bidOrder.GetPrice());
	  string s3 = to_string(bidOrder.GetVisibleQuantity());
	  string s4 = to_string(bidOrder.GetHiddenQuantity());
	  string s5 = "BID";
	  return s1 + "," + s2 + "," + s3 + "," + s4 + "," + s5;
	}
  string stream_to_string_offer() {
	  string s1 = product.GetProductId();
	  string s2 = to_string(offerOrder.GetPrice());
	  string s3 = to_string(offerOrder.GetVisibleQuantity());
	  string s4 = to_string(offerOrder.GetHiddenQuantity());
	  string s5 = "OFFER";
	  return s1 + "," + s2 + "," + s3 + "," + s4 + "," + s5;
  }


private:
  T product;
  PriceStreamOrder bidOrder;
  PriceStreamOrder offerOrder;

};

/**
 * Streaming service to publish two-way prices.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class StreamingListener;

template<typename T>
class StreamingConnector;




template<typename T>
class StreamingService : public Service<string,PriceStream <T> >
{

private:
	map<string, PriceStream<T>> streammap;
	vector<ServiceListener<PriceStream<T>>*> listeners;
	StreamingListener<T>* listener;
	StreamingConnector<T>* connector;
public:
	StreamingService() {
		listener = new StreamingListener<T>(this);
		listeners = vector<ServiceListener<PriceStream<T>>*>();
		streammap = map<string, PriceStream<T>>();
		connector = new StreamingConnector<T>(this);
	}
	~StreamingService() {
		delete listener;
		delete connector;
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

	virtual void PublishPrice(PriceStream<T>& priceStream) {
		for (auto& l : listeners)
			l->ProcessAdd(priceStream);
	}
	StreamingListener<T>* GetListener() {
		return listener;
	}

};

PriceStreamOrder::PriceStreamOrder(double _price, long _visibleQuantity, long _hiddenQuantity, PricingSide _side)
{
  price = _price;
  visibleQuantity = _visibleQuantity;
  hiddenQuantity = _hiddenQuantity;
  side = _side;
}

double PriceStreamOrder::GetPrice() const
{
  return price;
}

long PriceStreamOrder::GetVisibleQuantity() const
{
  return visibleQuantity;
}

long PriceStreamOrder::GetHiddenQuantity() const
{
  return hiddenQuantity;
}

template<typename T>
PriceStream<T>::PriceStream(const T &_product, const PriceStreamOrder &_bidOrder, const PriceStreamOrder &_offerOrder) :
  product(_product), bidOrder(_bidOrder), offerOrder(_offerOrder)
{
}


template<typename T>
const T& PriceStream<T>::GetProduct() const
{
  return product;
}

template<typename T>
const PriceStreamOrder& PriceStream<T>::GetBidOrder() const
{
  return bidOrder;
}

template<typename T>
const PriceStreamOrder& PriceStream<T>::GetOfferOrder() const
{
  return offerOrder;
}

template<typename T>
class StreamingConnector :public Connector<PriceStream<T>> {
private:
	StreamingService<T>* service;
public:
	StreamingConnector(StreamingService<T>* service) :service(service) {}
	~StreamingConnector() {}
	void Subscribe(ifstream& file) {}
	void Publish(PriceStream<T> &data) {}
};


template<typename T>
class StreamingListener :public ServiceListener<PriceStream<T>> {
private:
	StreamingService<T>* stream;
public:
	StreamingListener(StreamingService<T>* s) {
		stream = s;
	}
	~StreamingListener(){}
	void ProcessAdd(PriceStream<T> &data)
	{
		stream->PublishPrice(data);
	}

	// Listener callback to process a remove event to the Service
	virtual void ProcessRemove(PriceStream<T> &data) {}

	// Listener callback to process an update event to the Service
	virtual void ProcessUpdate(PriceStream<T> &data) {}








};









#endif
