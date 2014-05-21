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
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <sstream>
#include <iostream>
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

	BOOST_FOREACH(boost::property_tree::ptree::value_type &v,result.get_child("sensordatas")) {
	    BOOST_FOREACH(boost::property_tree::ptree::value_type &v2, v.second.get_child("values."))
	    {
	    	new Wt::WText(v2.second.get<std::string>("value"), manager->root());
	    }
	}
	Wt::WApplication::triggerUpdate();
}
void NodeDetails::showRequestErrorMessage(std::string msg) {
	addWidget(new Wt::WText(msg.c_str()));
}

} /* namespace RFSNMAN */
