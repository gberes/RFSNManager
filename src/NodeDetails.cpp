/*
 * NodeDetails.cpp
 *
 *  Created on: May 4, 2014
 *      Author: gberes
 */

#include "NodeDetails.h"
#include <Wt/Http/Client>
#include <Wt/WApplication>
#include <Wt/WPanel>
#include <Wt/WStackedWidget>
#include <Wt/WText>
#include <Wt/WTable>
#include <Wt/WHBoxLayout>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <sstream>
#include <vector>
#include <iostream>
#include <boost/date_time.hpp>
#include "RFSNManager.h"

namespace RFSNMAN {

NodeDetails::NodeDetails(std::string address, RFSNManager* manager) :
	manager(manager),
	address(address){
//	contents = new Wt::WStackedWidget(this);
	std::stringstream url;
	url << "http://" << manager->getGatewayAddress() << "/getsensordata/by-address/" << address;
	getRequest(url.str());
}

NodeDetails::~NodeDetails() {
	//delete contents;
}

void NodeDetails::responseArrived(boost::system::error_code err, const Wt::Http::Message& response) {
	boost::property_tree::ptree result;
	std::cout << "NodeDetails::responseArrived/" << __LINE__ << ": respone: " << response.body() << std::endl;
	std::istringstream is(response.body());
	read_json(is, result);

	Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
	setLayout(hbox);

	struct SensorDataTable {
		int type;
		std::vector<boost::posix_time::ptime> timestamps;
		std::vector<float> values;
	};

	BOOST_FOREACH(boost::property_tree::ptree::value_type &v,result.get_child("sensordatas")) {
		SensorDataTable sdt;
		sdt.type = v.second.get<int>("type");
	    BOOST_FOREACH(boost::property_tree::ptree::value_type &v2, v.second.get_child("values."))
	    {
	    	sdt.values.push_back(v2.second.get<float>("value"));
	    	sdt.timestamps.push_back(v2.second.get<boost::posix_time::ptime>("timestamp"));
	    }

		Wt::WTable *table = new Wt::WTable();
		table->setHeaderCount(1);
		table->setWidth("20%");
		table->elementAt(0, 0)->addWidget(new Wt::WText("Timestamp"));
		table->elementAt(0, 1)->addWidget(new Wt::WText("Value"));

		for (unsigned i = 0; i < sdt.values.size(); ++i) {
		    int row = i + 1;

		    table->elementAt(row, 0)->addWidget(new Wt::WText(Wt::WString(to_simple_string(sdt.timestamps.at(i)))));
		    table->elementAt(row, 1)->addWidget(new Wt::WText(Wt::WString::fromUTF8("{1}").arg(sdt.values.at(i))));
		}

		hbox->addWidget(table);
	}


	Wt::WApplication::instance()->triggerUpdate();
}
void NodeDetails::showRequestErrorMessage(std::string msg) {
	addWidget(new Wt::WText(msg.c_str()));
}

} /* namespace RFSNMAN */
