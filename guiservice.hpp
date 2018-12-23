#ifndef GUI_SERVICE_HPP
#define GUI_SERVICE_HPP



#include <string>
#include "soa.hpp"
#include<map>
#include<sstream>
#include "products.hpp"
#include "pricingservice.hpp"
#include<sys\timeb.h>




template<typename T>
class GuiConnector;

template<typename T>
class GuiService;


template<typename T>
class GuiListener:public ServiceListener<Price<T>>
{
private:
	GuiService<T>* gui;

public:
	GuiListener(GuiService<T>* g) {
		gui = g;
	}

	// Listener callback to process an add event to the Service
	void ProcessAdd( Price<T> &data)
	{
		gui->OnMessage(data);
	}

	// Listener callback to process a remove event to the Service
	virtual void ProcessRemove( Price<T> &data){}

	// Listener callback to process an update event to the Service
	virtual void ProcessUpdate(Price<T> &data) {}

};


template<typename T>
class GuiService:public Service<string,Price<T>>
{
private:
	map<string, Price<T>> pricetable;
	GuiConnector<T>* connector;
	GuiListener<T>* listener;
	long long throttle;

public:
	GuiService(long long _throttle=300)
	{
		
		pricetable = map<string, Price<T>>();
		connector = new GuiConnector<T>();
		listener = new GuiListener<T>(this);
		throttle = _throttle;
	}
	~GuiService() {
		delete connector;
		delete listener;
	}


	// Get data on our service given a key
	virtual Price<T>& GetData(string key) {
		return pricetable[key];
	 }

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(Price<T> &data) {
		static int countshow = 0;
		if (countshow == 100)
		{
			cout << "First 100 updates shown!" << endl; 
			countshow++;
				return;
		}
		else if (countshow > 100)
			return;
		clock_t start = clock();
		while (clock() - start < throttle);
		connector->Publish(data);
		cout << countshow << " ";
		countshow++;
	}

	GuiListener<T>* GetListener() {
		return listener;
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<Price<T>> *listener){}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<Price<T>>* >& GetListeners()const {
		return vector<ServiceListener<Price<T>>*>();
	}

};


string timestamp();


template<typename T>
class GuiConnector :public Connector<Price<T>>
{


public:
	GuiConnector() {}
	~GuiConnector() {}
	// Publish data to the Connector
	void Publish(Price<T> &data)
	{
		ofstream file;
		file.open("gui.txt", ios::app);
		file << timestamp() << ",";
		file << data.Price_to_string() << endl;
	}




	virtual void Subscribe(ifstream& file) {}


};













#endif