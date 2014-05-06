/*
 * NodeList.cpp
 *
 *  Created on: May 3, 2014
 *      Author: gberes
 */

#include "NodeList.h"
#include <Wt/Http/Client>
#include <Wt/WText>
#include <Wt/WStackedWidget>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <sstream>
#include <iostream>
#include "RFSNManager.h"

namespace RFSNMAN {

NodeList::NodeList(RFSNManager* man) :
		manager(man) {
	contents = new Wt::WStackedWidget();
	menu = new Wt::WMenu(contents, Wt::Vertical, this);
	menu->setStyleClass("nav nav-pills nav-stacked");
	menu->setWidth(150);

	std::stringstream url;
	url << "http://" << manager->getGatewayAddress() << "/listnodes";
	getRequest(url.str().c_str());
}

void NodeList::responseArrived(boost::system::error_code err, const Wt::Http::Message& response) {
	boost::property_tree::ptree result;
	std::istringstream is(response.body());
	read_json(is, result);

	BOOST_FOREACH(boost::property_tree::ptree::value_type &v,result.get_child("nodes")) {
		Wt::WMenuItem* mi = menu->addItem(v.second.get<std::string>("name"));
		mi->clicked().connect(boost::bind(&RFSNManager::showNode, manager, v.second.get<std::string>("address")));
	}

	addWidget(contents);
}
void NodeList::showRequestErrorMessage(std::string msg) {
	addWidget(new Wt::WText(msg.c_str()));
}

NodeList::~NodeList() {
	delete menu;
	delete contents;
}

} /* namespace RFSNMAN */
