/**
 * inquiryservice.hpp
 * Defines the data types and Service for customer inquiries.
 *
 * @author Breman Thuraisingham
 */
#ifndef INQUIRY_SERVICE_HPP
#define INQUIRY_SERVICE_HPP

#include "soa.hpp"
#include "tradebookingservice.hpp"
#include<map>

// Various inqyury states
enum InquiryState { RECEIVED, QUOTED, DONE, REJECTED, CUSTOMER_REJECTED };

/**
 * Inquiry object modeling a customer inquiry from a client.
 * Type T is the product type.
 */
template<typename T>
class Inquiry
{

public:

  // ctor for an inquiry
  Inquiry(string _inquiryId, const T &_product, Side _side, long _quantity, double _price, InquiryState _state);
  Inquiry() = default;
  // Get the inquiry ID
  const string& GetInquiryId() const;

  // Get the product
  const T& GetProduct() const;

  // Get the side on the inquiry
  Side GetSide() const;

  // Get the quantity that the client is inquiring for
  long GetQuantity() const;

  // Get the price that we have responded back with
  double GetPrice() const;

  // Get the current state on the inquiry
  InquiryState GetState() const;

  void SetState(InquiryState s) {
	  state = s;
  }
  void SetPrice(double p) {
	  price = p;
  }
  string inquiry_to_string() {
	  string s2 = product.GetProductId();
	  string s3;
	  switch (side)
	  {
	  case BUY:
		  s3 = "BUY";
		  break;
	  case SELL:
		  s3 = "SELL";
		  break;
	  default:
		  break;
	  }
	  string s1 = inquiryId;
	  string s4 = to_string(quantity);
	  string s5 = to_string(price);
	  string s6;
	  switch (state)
	  {
	  case RECEIVED:
		  s6 = "RECEIVED";
		  break;
	  case QUOTED:
		  s6 = "QUOTED";
		  break;
	  case DONE:
		  s6 = "DONE";
		  break;
	  case REJECTED:
		  s6 = "REJECTED";
		  break;
	  case CUSTOMER_REJECTED:
		  s6 = "CUSTOMER_REJECTED";
		  break;
	  default:
		  break;
	  }
	  return s1 + "," + s2 + "," + s3 + "," + s4 + "," + s5 + "," + s6;
  }


private:
  string inquiryId;
  T product;
  Side side;
  long quantity;
  double price;
  InquiryState state;

};

/**
 * Service for customer inquirry objects.
 * Keyed on inquiry identifier (NOTE: this is NOT a product identifier since each inquiry must be unique).
 * Type T is the product type.
 */
template<typename T>
class InquiryListener;
template<typename T>
class InquiryConnector;

template<typename T>
class InquiryService : public Service<string,Inquiry <T> >
{

private:
	map<string, Inquiry<T>> inquirytable;
	InquiryConnector<T>* connector;
	vector<ServiceListener<Inquiry<T>>*> listeners;
public:
	InquiryService() {
		inquirytable = map<string, Inquiry<T>>();
		connector = new InquiryConnector<T>(this);
		listeners = vector<ServiceListener<Inquiry<T>>*>();
	}
	~InquiryService() {
		delete connector;
	}
	InquiryConnector<T>* GetConnector() {
		return connector;
	}


	// Get data on our service given a key
	virtual Inquiry<T>& GetData(string key)
	{
		return inquirytable[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(Inquiry<T> &data)
	{
		T temp = data.GetProduct();
		string stemp = data.GetInquiryId();
		Inquiry<T> ptemp = data;
		inquirytable[stemp] = ptemp;
		for (auto& listener : listeners)
			listener->ProcessAdd(data);
		if (data.GetState() == QUOTED)
		{
			data.SetState(DONE);
			for (auto& listener : listeners)
				listener->ProcessAdd(data);
		}
		else
		{
			data.SetPrice(100);
			connector->Publish(data);
		}
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<Inquiry<T>> *listener)
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<Inquiry<T>>* >& GetListeners()const
	{
		return listeners;
	}

public:

  // Send a quote back to the client
	void SendQuote(const string &inquiryId, double price) {
		Inquiry<T> inq = inquirytable[inquiryId];
		inq.SetPrice(price);
		connector->Publish(inq);
  }

  // Reject an inquiry from the client
	void RejectInquiry(const string &inquiryId) {
		Inquiry<T> inq = inquirytable[inquiryId];
		inq.SetState(REJECTED);
		for (auto& listener : listeners)
			listener->ProcessAdd(inq);
  }

};

template<typename T>
class InquiryConnector :public Connector<Inquiry<T>> {
private:
	InquiryService<T>* service;
	long id;
public:
	InquiryConnector(InquiryService<T>* service) :service(service) { id = 1; }
	~InquiryConnector() {}
	void Subscribe(ifstream& file) {
		string line;
		stringstream linestream;
		cout << "Inquiry data loading......" << endl;
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
			double mid = convert(component[3]);
			double spread = convert(component[4]);
			double price;
			long quantity = stol(component[2]);
			string sidestr = component[5];
			Side side;
			InquiryState state = RECEIVED;
			if (sidestr == "BUY")
			{
				price = mid - spread;
				side = BUY;
			}
			else
			{
				price = mid + spread;
				side = SELL;
			}

			Inquiry<T> data(to_string(id), bond, side, quantity, price, state);
			id++;
			service->OnMessage(data);
		}
		cout << "inquiry data loaded......" << endl;

	}
	void Publish(Inquiry<T> &data) {
		data.SetState(QUOTED);
		service->OnMessage(data);
	}





};

























template<typename T>
Inquiry<T>::Inquiry(string _inquiryId, const T &_product, Side _side, long _quantity, double _price, InquiryState _state) :
  product(_product)
{
  inquiryId = _inquiryId;
  side = _side;
  quantity = _quantity;
  price = _price;
  state = _state;
}

template<typename T>
const string& Inquiry<T>::GetInquiryId() const
{
  return inquiryId;
}

template<typename T>
const T& Inquiry<T>::GetProduct() const
{
  return product;
}

template<typename T>
Side Inquiry<T>::GetSide() const
{
  return side;
}

template<typename T>
long Inquiry<T>::GetQuantity() const
{
  return quantity;
}

template<typename T>
double Inquiry<T>::GetPrice() const
{
  return price;
}

template<typename T>
InquiryState Inquiry<T>::GetState() const
{
  return state;
}

#endif
