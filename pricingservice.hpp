/**
 * pricingservice.hpp
 * Defines the data types and Service for internal prices.
 *
 * @author Breman Thuraisingham
 */
#ifndef PRICING_SERVICE_HPP
#define PRICING_SERVICE_HPP

#include <string>
#include "soa.hpp"
#include<map>
#include<sstream>
#include "products.hpp"


double convert(string& s) {
	stringstream ss(s);
	string temp;
	vector<string> strings;
	while (getline(ss, temp, '-'))
	{
		strings.push_back(temp);
	}
	string s1 = strings[0];
	string s2 = strings[1].substr(0, 2);
	string s3 = strings[1].substr(2, 1);
	double x1 = stoi(s1);
	double x2 = double(stoi(s2)) / 32.0;
	double x3;
	if (s3 == "+")
		x3 = 4.0 / 256.0;
	else
		x3 = (double(stoi(s3))) / 256.0;

	return x1 + x2 + x3;
}





/**
 * A price object consisting of mid and bid/offer spread.
 * Type T is the product type.
 */
template<typename T>
class Price
{

public:

  // ctor for a price
  Price(const T &_product, double _mid, double _bidOfferSpread);
  Price() = default;
  // Get the product
  const T& GetProduct() const;

  // Get the mid price
  double GetMid() const;

  // Get the bid/offer spread around the mid
  double GetBidOfferSpread() const;
  string Price_to_string() {
	  int x1 = int(mid);
	  int x2 = int((mid-x1)*32);
	  int x3 = int((mid-x1-(double(x2)/32.0))*256);
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
	  string _mid = s1 + s2 + s3;
	  int x4 = int(bidOfferSpread * 256);
	  string _spread;
	  if (x4 == 4)
		  _spread = "0-00+";
	  else
		  _spread = "0-00" + to_string(x4);
	  return GetProduct().GetProductId() + "," + _mid + "," + _spread;
	


  }

private:
  T product;
  double mid;
  double bidOfferSpread;

};




template<typename T>
class PricingConnector;

/**
 * Pricing Service managing mid prices and bid/offers.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class PricingService : public Service<string,Price <T> >
{
private:
	map<string, Price<T>> pricetable;
	PricingConnector<T>* connector;
	vector<ServiceListener<Price<T>>*> listeners;
public:
	PricingService() {
		pricetable = map<string, Price<T>>();
		connector = new PricingConnector<T>(this);
		listeners = vector<ServiceListener<Price<T>>*>();
	}
	~PricingService(){
		delete connector;
	}
	PricingConnector<T>* GetConnector() {
		return connector;
	}


	// Get data on our service given a key
	virtual Price<T>& GetData(string key)
	{
		return pricetable[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(Price<T> &data)
	{
		T temp = data.GetProduct();
		string stemp = temp.GetProductId();
		Price<T> ptemp = data;
		pricetable[stemp] = ptemp;
		for (auto& listener : listeners)
			listener->ProcessAdd(data);
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<Price<T>> *listener)
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<Price<T>>* >& GetListeners()const
	{
		return listeners;
	}









};


template<typename T>
class PricingConnector :public Connector<Price<T>> {
private:
	PricingService<T>* service;
public:
	PricingConnector(PricingService<T>* service) :service(service) {}
	~PricingConnector(){}
	void Subscribe(ifstream& file) {
		string line;
		stringstream linestream;
		cout << "price data loading......" << endl;
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
			double spread = convert(component[2]);
			Price<T> data(bond, mid, spread);
			service->OnMessage(data);
		}
		cout << "price data loaded......" << endl;

	}
	void Publish(Price<T> &data) {}





};















template<typename T>
Price<T>::Price(const T &_product, double _mid, double _bidOfferSpread) :
  product(_product)
{
  mid = _mid;
  bidOfferSpread = _bidOfferSpread;
}

template<typename T>
 const T& Price<T>::GetProduct() const
{
  return product;
}

template<typename T>
double Price<T>::GetMid() const
{
  return mid;
}

template<typename T>
double Price<T>::GetBidOfferSpread() const
{
  return bidOfferSpread;
}

#endif
