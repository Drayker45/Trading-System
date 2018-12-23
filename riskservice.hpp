/**
 * riskservice.hpp
 * Defines the data types and Service for fixed income risk.
 *
 * @author Breman Thuraisingham
 */
#ifndef RISK_SERVICE_HPP
#define RISK_SERVICE_HPP

#include "soa.hpp"
#include "positionservice.hpp"

/**
 * PV01 risk.
 * Type T is the product type.
 */



template<typename T>
class PV01
{

public:

  // ctor for a PV01 value
  PV01(const T &_product, double _pv01, long _quantity);
  PV01() = default;

  // Get the product on this PV01 value
  const T& GetProduct() const {
	  return product;
  }

  // Get the PV01 value
  double GetPV01() const {
	  return pv01;
  }

  // Get the quantity that this risk value is associated with
  long GetQuantity() const {
	  return quantity;
  }

private:
  T product;
  double pv01;
  long quantity;

};

/**
 * A bucket sector to bucket a group of securities.
 * We can then aggregate bucketed risk to this bucket.
 * Type T is the product type.
 */
template<typename T>
class BucketedSector
{

public:

  // ctor for a bucket sector
  BucketedSector(const vector<T> &_products, string _name);
  BucketedSector() = default;

  // Get the products associated with this bucket
  const vector<T>& GetProducts() const;

  // Get the name of the bucket
  const string& GetName() const;

private:
  vector<T> products;
  string name;

};

/**
 * Risk Service to vend out risk for a particular security and across a risk bucketed sector.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class RiskToPositionListener;


template<typename T>
class RiskService : public Service<string,PV01 <T> >
{
private:
	map<string, PV01<T>> risk;
	vector<ServiceListener<PV01<T>>*> listeners;
	RiskToPositionListener<T>* listener;
	vector<ServiceListener<PV01<BucketedSector<T>>>*> listenersbucket;
	
public:
	map<string, double> pv01;
	
	RiskService() {
		risk = map<string, PV01<T>>();
		listeners = vector<ServiceListener<PV01<T>>*>();
		listener = new RiskToPositionListener<T>(this);
		listenersbucket = vector<ServiceListener<PV01<BucketedSector<T>>>*>();
		pv01 = map<string, double>();
		date d(2018, Nov, 25);
		T bond1("2Y", CUSIP, "T", 0, d);
		T bond2("3Y", CUSIP, "T", 0, d);
		T bond3("5Y", CUSIP, "T", 0, d);
		T bond4("7Y", CUSIP, "T", 0, d);
		T bond5("10Y", CUSIP, "T", 0, d);
		T bond6("30Y", CUSIP, "T", 0, d);
		
		string s = "2Y";		pv01[s] = 0.019851; risk[s] = PV01<T>(bond1, 0.019851, 0);
		s = "3Y"; pv01[s] = 0.029309; risk[s] = PV01<T>(bond2, 0.029309, 0);
		s = "5Y"; pv01[s] = 0.048643; risk[s] = PV01<T>(bond3, 0.048643, 0);
		s = "7Y"; pv01[s] = 0.065843; risk[s] = PV01<T>(bond4, 0.065843, 0);
		s = "10Y"; pv01[s] = 0.087939; risk[s] = PV01<T>(bond5, 0.087939, 0);
		s = "30Y"; pv01[s] = 0.184698; risk[s] = PV01<T>(bond6, 0.184698, 0);
	}
	~RiskService() { delete listener; }
	// Get data on our service given a key
	PV01<T>& GetData(string key) {
		return risk[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(PV01<T> &data) {}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<PV01<T>> *listener) {
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<PV01<T>>* >& GetListeners() const {
		return listeners;
	}
	ServiceListener<Position<T>>* GetListener() {
		return listener;
	}
  // Add a position that the service will risk
	void AddPosition(Position<T> &position) {
		T bond = position.GetProduct();
		PV01<T> temp(bond, pv01[bond.GetProductId()], position.GetAggregatePosition());
		risk[bond.GetProductId()] = temp;
		for (auto& l : listeners)
			l->ProcessAdd(temp);
  }
	virtual void AddListenerB(ServiceListener<PV01<BucketedSector<T>>> *listener) {
		listenersbucket.push_back(listener);
	}





  // Get the bucketed risk for the bucket sector
  const PV01< BucketedSector<T> > GetBucketedRisk(const BucketedSector<T> &sector)const  {
	  double ret=0;
	  string s = sector.GetName();
	  long q = 0;
	  vector<T> products = sector.GetProducts();
	  for (const T& bond : products)
	  {
		  string a = bond.GetProductId();
		  PV01<T> tempp = risk.at(a);
		  ret += tempp.GetPV01() *(tempp.GetQuantity());
		  q += tempp.GetQuantity();
	  }
	  PV01<BucketedSector<T>> p(sector, ret, q);
	  return p;
  }
  void AddPositionBucket(Position<T> &position)
  {
	  date d(2018, Nov, 25);
	  T bond = position.GetProduct();
	  T bond1("2Y", CUSIP, "T", 0, d);
	  T bond2("3Y", CUSIP, "T", 0, d);
	  T bond3("5Y", CUSIP, "T", 0, d);
	  T bond4("7Y", CUSIP, "T", 0, d);
	  T bond5("10Y", CUSIP, "T", 0, d);
	  T bond6("30Y", CUSIP, "T", 0, d);
	  vector<T> bucket;
	  
	  if (position.GetProduct().GetProductId() == "2Y" || position.GetProduct().GetProductId() == "3Y")
	  {
		  bucket.push_back(bond1);
		  bucket.push_back(bond2);
		  BucketedSector<T> b(bucket, "FrontEnd");
		  GetBucketedRisk(b);
		  PV01< BucketedSector<T> > pv = GetBucketedRisk(b);
		 for (auto& l : listenersbucket)
			 l->ProcessAdd(pv);
	  }
	  else if (position.GetProduct().GetProductId() == "5Y" || position.GetProduct().GetProductId() == "7Y" || position.GetProduct().GetProductId() == "10Y")
	  {
		  bucket.push_back(bond3);
		  bucket.push_back(bond4);
		  bucket.push_back(bond5);
		  BucketedSector<T> b(bucket, "Belly");
		  PV01< BucketedSector<T> > pv = GetBucketedRisk(b);
		  for (auto& l : listenersbucket)
			  l->ProcessAdd(pv);
	  }
	  else
	  {
		  bucket.push_back(bond6);
		  BucketedSector<T> b(bucket, "LongEnd");
		  PV01< BucketedSector<T> > pv = GetBucketedRisk(b);
		  for (auto& l : listenersbucket)
			  l->ProcessAdd(pv);
	  }
	  
  }




};



template<typename T>
class RiskToPositionListener :public ServiceListener<Position<T>> {
private:
	RiskService<T>* service;

public:
	RiskToPositionListener(RiskService<T>* s) {
		service = s;
	}
	~RiskToPositionListener() {}

	// Listener callback to process an add event to the Service
	void ProcessAdd(Position<T> &data)
	{
		service->AddPosition(data);
		service->AddPositionBucket(data);
	}

	// Listener callback to process a remove event to the Service
	virtual void ProcessRemove(Position<T> &data) {}

	// Listener callback to process an update event to the Service
	virtual void ProcessUpdate(Position<T> &data) {}




};



















template<typename T>
PV01<T>::PV01(const T &_product, double _pv01, long _quantity) :
  product(_product)
{
  pv01 = _pv01;
  quantity = _quantity;
}

template<typename T>
BucketedSector<T>::BucketedSector(const vector<T>& _products, string _name) :
  products(_products)
{
  name = _name;
}

template<typename T>
const vector<T>& BucketedSector<T>::GetProducts() const
{
  return products;
}

template<typename T>
const string& BucketedSector<T>::GetName() const
{
  return name;
}

#endif
