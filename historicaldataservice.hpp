/**
 * historicaldataservice.hpp
 * historicaldataservice.hpp
 *
 * @author Breman Thuraisingham
 * Defines the data types and Service for historical data.
 *
 * @author Breman Thuraisingham
 */
#ifndef HISTORICAL_DATA_SERVICE_HPP
#define HISTORICAL_DATA_SERVICE_HPP

/**
 * Service for processing and persisting historical data to a persistent store.
 * Keyed on some persistent key.
 * Type T is the data type to persist.
 */
#include"streamingAlgoservice.hpp"
#include "soa.hpp"
#include "positionservice.hpp"
#include "riskservice.hpp"
#include "executionservice.hpp"
#include "inquiryservice.hpp"

//----------------------------------------------------To Streaming---------------------------------------
template<typename T>
class HisToStreamingListener;

template<typename T>
class HisToStreamingConnector;

template<typename T>
class HistoricalDataServiceStream : Service<string,PriceStream<T>>
{
private:
	HisToStreamingListener<T>* listener;
	HisToStreamingConnector<T>* connector;

public:
	HistoricalDataServiceStream() {
		listener = new HisToStreamingListener<T>(this);
		connector = new HisToStreamingConnector<T>();
	}
	~HistoricalDataServiceStream() {
		delete listener;
		delete connector;
	}


	// Get data on our service given a key
	virtual PriceStream<T>& GetData(string key) 
	{
		PriceStream<T> x= PriceStream<T>();
		return x;
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(PriceStream<T> &data)
	{}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<PriceStream<T>> *listener)
	{
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<PriceStream<T>>* >& GetListeners()const
	{
		return vector<ServiceListener<PriceStream<T>>*>();
	}

	HisToStreamingListener<T>* GetListener() {
		return listener;
	}

  // Persist data to a store
	void PersistData(string persistKey,  PriceStream<T>& data) {
		
		connector->Publish(data);
  }

};


template<typename T>
class HisToStreamingListener :public ServiceListener<PriceStream<T>> {
private:
	HistoricalDataServiceStream<T>* service;
public:
	HisToStreamingListener(HistoricalDataServiceStream<T>* s) {
		service = s;
	}
	~HisToStreamingListener(){}
	// Listener callback to process an add event to the Service
	virtual void ProcessAdd(PriceStream<T> &data) {
		service->PersistData("", data);
	}

	// Listener callback to process a remove event to the Service
	virtual void ProcessRemove(PriceStream<T> &data) {}

	// Listener callback to process an update event to the Service
	virtual void ProcessUpdate(PriceStream<T> &data) {}
};
template<typename T>
class HisToStreamingConnector :public Connector<PriceStream<T>>
{
public:
	HisToStreamingConnector() {}
	~HisToStreamingConnector() {}
	// Publish data to the Connector
	void Publish(PriceStream<T> &data)
	{
		
		ofstream file;
		file.open("streaming.txt", ios::app);
		file << timestamp() << ",";
		file << data.stream_to_string_bid() << endl;
		file <<timestamp() <<","<<data.stream_to_string_offer() << endl;
	}

	virtual void Subscribe(ifstream& file) {}


};

//---------------------------------------------To Position-----------------------------------------------------------


template<typename T>
class HisToPositionListener;

template<typename T>
class HisToPositionConnector;

template<typename T>
class HistoricalDataServicePosition : Service<string, Position<T>>
{
private:
	HisToPositionListener<T>* listener;
	HisToPositionConnector<T>* connector;

public:
	HistoricalDataServicePosition() {
		listener = new HisToPositionListener<T>(this);
		connector = new HisToPositionConnector<T>();
	}
	~HistoricalDataServicePosition() {
		delete listener;
		delete connector;
	}


	// Get data on our service given a key
	virtual Position<T>& GetData(string key)
	{
		Position<T> x = Position<T>();
		return x;
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(Position<T> &data)
	{}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<Position<T>> *listener)
	{
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<Position<T>>* >& GetListeners()const
	{
		return vector<ServiceListener<Position<T>>*>();
	}

	HisToPositionListener<T>* GetListener() {
		return listener;
	}

	// Persist data to a store
	void PersistData(string persistKey, Position<T>& data) {

		connector->Publish(data);
	}

};


template<typename T>
class HisToPositionListener :public ServiceListener<Position<T>> {
private:
	HistoricalDataServicePosition<T>* service;
public:
	HisToPositionListener(HistoricalDataServicePosition<T>* s) {
		service = s;
	}
	~HisToPositionListener() {}
	// Listener callback to process an add event to the Service
	virtual void ProcessAdd(Position<T> &data) {
		service->PersistData("", data);
	}

	// Listener callback to process a remove event to the Service
	virtual void ProcessRemove(Position<T> &data) {}

	// Listener callback to process an update event to the Service
	virtual void ProcessUpdate(Position<T> &data) {}
};
template<typename T>
class HisToPositionConnector :public Connector<Position<T>>
{
public:
	HisToPositionConnector() {}
	~HisToPositionConnector() {}
	// Publish data to the Connector
	void Publish(Position<T> &data)
	{

		ofstream file;
		file.open("position.txt", ios::app);
		file << timestamp() << ",";
		file << data.Position_to_String() << endl;
		
	}

	virtual void Subscribe(ifstream& file) {}


};
//--------------------------------------------------------------------------To Risk--------------------------

template<typename T>
class HisToRiskListener;

template<typename T>
class HisToRiskConnector;
template<typename T>
class HisToRiskListenerB;

template<typename T>
class HisToRiskConnectorB;

template<typename T>
class HistoricalDataServiceRisk : Service<string, PV01<T>>
{
private:
	HisToRiskListener<T>* listener;
	HisToRiskConnector<T>* connector;
	HisToRiskListenerB<T>* listenerb;
	HisToRiskConnectorB<T>* connectorb;
public:
	HistoricalDataServiceRisk() {
		listener = new HisToRiskListener<T>(this);
		connector = new HisToRiskConnector<T>();
		listenerb = new HisToRiskListenerB<T>(this);
		connectorb = new HisToRiskConnectorB<T>();
	}
	~HistoricalDataServiceRisk() {
		delete listener;
		delete connector;
		delete listenerb;
		delete connectorb;
	}


	// Get data on our service given a key
	virtual PV01<T>& GetData(string key)
	{
		PV01<T> x = PV01<T>();
		return x;
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(PV01<T> &data)
	{}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<PV01<T>> *listener)
	{
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<PV01<T>>* >& GetListeners()const
	{
		return vector<ServiceListener<PV01<T>>*>();
	}

	HisToRiskListener<T>* GetListener() {
		return listener;
	}
	HisToRiskListenerB<T>* GetListenerB() {
		return listenerb;
	}

	// Persist data to a store
	void PersistData(string persistKey, PV01<T>& data) {

		connector->Publish(data);
	}
	void PersistDataB(string persistKey, PV01<BucketedSector<T>>& data) {

		connectorb->Publish(data);
	}

};


template<typename T>
class HisToRiskListener :public ServiceListener<PV01<T>> {
private:
	HistoricalDataServiceRisk<T>* service;
public:
	HisToRiskListener(HistoricalDataServiceRisk<T>* s) {
		service = s;
	}
	~HisToRiskListener() {}
	// Listener callback to process an add event to the Service
	virtual void ProcessAdd(PV01<T> &data) {
		service->PersistData("", data);
	}

	// Listener callback to process a remove event to the Service
	virtual void ProcessRemove(PV01<T> &data) {}

	// Listener callback to process an update event to the Service
	virtual void ProcessUpdate(PV01<T> &data) {}
};
template<typename T>
class HisToRiskConnector :public Connector<PV01<T>>
{
public:
	HisToRiskConnector() {}
	~HisToRiskConnector() {}
	// Publish data to the Connector
	void Publish(PV01<T> &data)
	{
		string s1 = data.GetProduct().GetProductId();
		string s2 = to_string(data.GetPV01());
		string s3 = to_string(data.GetQuantity());
		ofstream file;
		file.open("risk.txt", ios::app);
		file << timestamp() << ",";
		file << s1<<","<<s2<<","<<s3 << endl;
		
	}

	virtual void Subscribe(ifstream& file) {}


};
template<typename T>
class HisToRiskListenerB:public ServiceListener<PV01<BucketedSector<T>>> {
private:
	HistoricalDataServiceRisk<T>* service;
public:
	HisToRiskListenerB(HistoricalDataServiceRisk<T>* s) {
		service = s;
	}
	~HisToRiskListenerB() {}
	// Listener callback to process an add event to the Service
	virtual void ProcessAdd(PV01<BucketedSector<T>> &data) {
		service->PersistDataB("", data);
	}

	// Listener callback to process a remove event to the Service
	virtual void ProcessRemove(PV01<BucketedSector<T>> &data) {}

	// Listener callback to process an update event to the Service
	virtual void ProcessUpdate(PV01<BucketedSector<T>> &data) {}
};
template<typename T>
class HisToRiskConnectorB :public Connector<PV01<BucketedSector<T>>>
{
public:
	HisToRiskConnectorB() {}
	~HisToRiskConnectorB() {}
	// Publish data to the Connector
	void Publish(PV01<BucketedSector<T>> &data)
	{
		string s1 = data.GetProduct().GetName();
		string s2 = to_string(data.GetPV01());
		string s3 = to_string(data.GetQuantity());
		ofstream file;
		file.open("risk.txt", ios::app);
		file << timestamp() << ",";
		file << s1 << "," << s2 << "," << s3 << endl;
	}

	virtual void Subscribe(ifstream& file) {}


};


//-------------------------------------------------------To Execution----------------------------------------



template<typename T>
class HisToExecutionListener;

template<typename T>
class HisToExecutionConnector;

template<typename T>
class HistoricalDataServiceExecution : Service<string, ExecutionOrder<T>>
{
private:
	HisToExecutionListener<T>* listener;
	HisToExecutionConnector<T>* connector;

public:
	HistoricalDataServiceExecution() {
		listener = new HisToExecutionListener<T>(this);
		connector = new HisToExecutionConnector<T>();
	}
	~HistoricalDataServiceExecution() {
		delete listener;
		delete connector;
	}


	// Get data on our service given a key
	virtual ExecutionOrder<T>& GetData(string key)
	{
		ExecutionOrder<T> x = ExecutionOrder<T>();
		return x;
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(ExecutionOrder<T> &data)
	{}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<ExecutionOrder<T>> *listener)
	{
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<ExecutionOrder<T>>* >& GetListeners()const
	{
		return vector<ServiceListener<ExecutionOrder<T>>*>();
	}

	HisToExecutionListener<T>* GetListener() {
		return listener;
	}

	// Persist data to a store
	void PersistData(string persistKey, ExecutionOrder<T>& data) {

		connector->Publish(data);
	}

};


template<typename T>
class HisToExecutionListener :public ServiceListener<ExecutionOrder<T>> {
private:
	HistoricalDataServiceExecution<T>* service;
public:
	HisToExecutionListener(HistoricalDataServiceExecution<T>* s) {
		service = s;
	}
	~HisToExecutionListener() {}
	// Listener callback to process an add event to the Service
	virtual void ProcessAdd(ExecutionOrder<T> &data) {
		service->PersistData("", data);
	}

	// Listener callback to process a remove event to the Service
	virtual void ProcessRemove(ExecutionOrder<T> &data) {}

	// Listener callback to process an update event to the Service
	virtual void ProcessUpdate(ExecutionOrder<T> &data) {}
};
template<typename T>
class HisToExecutionConnector :public Connector<ExecutionOrder<T>>
{
public:
	HisToExecutionConnector() {}
	~HisToExecutionConnector() {}
	// Publish data to the Connector
	void Publish(ExecutionOrder<T> &data)
	{

		ofstream file;
		file.open("Execution.txt", ios::app);
		file << timestamp() << ",";
		file << data.order_to_string() << endl;

	}

	virtual void Subscribe(ifstream& file) {}


};

//-------------------------------------------------------To Inquiry----------------------------------------



template<typename T>
class HisToInquiryListener;

template<typename T>
class HisToInquiryConnector;

template<typename T>
class HistoricalDataServiceInquiry : Service<string, Inquiry<T>>
{
private:
	HisToInquiryListener<T>* listener;
	HisToInquiryConnector<T>* connector;

public:
	HistoricalDataServiceInquiry() {
		listener = new HisToInquiryListener<T>(this);
		connector = new HisToInquiryConnector<T>();
	}
	~HistoricalDataServiceInquiry() {
		delete listener;
		delete connector;
	}


	// Get data on our service given a key
	virtual Inquiry<T>& GetData(string key)
	{
		Inquiry<T> x = Inquiry<T>();
		return x;
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(Inquiry<T> &data)
	{}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<Inquiry<T>> *listener)
	{
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<Inquiry<T>>* >& GetListeners()const
	{
		return vector<ServiceListener<Inquiry<T>>*>();
	}

	HisToInquiryListener<T>* GetListener() {
		return listener;
	}

	// Persist data to a store
	void PersistData(string persistKey, Inquiry<T>& data) {

		connector->Publish(data);
	}

};


template<typename T>
class HisToInquiryListener :public ServiceListener<Inquiry<T>> {
private:
	HistoricalDataServiceInquiry<T>* service;
public:
	HisToInquiryListener(HistoricalDataServiceInquiry<T>* s) {
		service = s;
	}
	~HisToInquiryListener() {}
	// Listener callback to process an add event to the Service
	virtual void ProcessAdd(Inquiry<T> &data) {
		service->PersistData("", data);
	}

	// Listener callback to process a remove event to the Service
	virtual void ProcessRemove(Inquiry<T> &data) {}

	// Listener callback to process an update event to the Service
	virtual void ProcessUpdate(Inquiry<T> &data) {}
};
template<typename T>
class HisToInquiryConnector :public Connector<Inquiry<T>>
{
public:
	HisToInquiryConnector() {}
	~HisToInquiryConnector() {}
	// Publish data to the Connector
	void Publish(Inquiry<T> &data)
	{

		ofstream file;
		file.open("allinquiries.txt", ios::app);
		file << timestamp() << ",";
		file << data.inquiry_to_string() << endl;

	}

	virtual void Subscribe(ifstream& file) {}


};



#endif
