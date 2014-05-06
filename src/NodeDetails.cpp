/*
 * NodeDetails.cpp
 *
 *  Created on: May 4, 2014
 *      Author: gberes
 */

#include "NodeDetails.h"
#include <Wt/Http/Client>
#include <Wt/WApplication>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <sstream>
#include <iostream>

namespace RFSNMAN {

NodeDetails::NodeDetails(std::string address) {
//	Wt::Http::Client *client = new Wt::Http::Client(Wt::WApplication::instance());
//	client->setTimeout(15);
//	client->setMaximumResponseSize(10 * 1024);
//	client->done().connect(boost::bind(&NodeList::handleHttpResponse, this, _1, _2));
//
//	client->get("http://127.0.0.1:4231/getsensordata/by-address/" + address);

}

void NodeDetails::handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response) {
	boost::property_tree::ptree result;
	std::istringstream is(response.body());
	read_json(is, result);

//	BOOST_FOREACH(boost::property_tree::ptree::value_type &v,result.get_child("nodes")){
//		Wt::WMenuItem* mi = menu->addItem(v.second.get<std::string>("address"));
//		mi->clicked().connect(boost::bind(&RFSNManager::showNode, manager, v.second.get<std::string>("name")));
//	}
//
//	addWidget(&contents);

}

NodeDetails::~NodeDetails() {
}

} /* namespace RFSNMAN */
