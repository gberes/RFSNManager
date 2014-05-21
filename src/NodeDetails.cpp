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

//#include <Wt/Chart/WCartesianChart>
//#include <Wt/Chart/WDataSeries>
//#include <Wt/WAbstractItemModel>
//#include <Wt/WAbstractItemView>
//#include <Wt/WApplication>
//#include <Wt/WContainerWidget>
//#include <Wt/WDate>
//#include <Wt/WEnvironment>
//#include <Wt/WPaintedWidget>
//#include <Wt/WItemDelegate>
//#include <Wt/WShadow>
//#include <Wt/WStandardItem>
//#include <Wt/WStandardItemModel>
//#include <Wt/WTableView>
#include <Wt/WVBoxLayout>

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

	Wt::WTable *containerTable = new Wt::WTable();
	containerTable->setHeaderCount(0);

	struct SensorDataTable {
		int type;
		std::vector<boost::posix_time::ptime> timestamps;
		std::vector<float> values;
	};

	int typeNum = 0;

	BOOST_FOREACH(boost::property_tree::ptree::value_type &v,result.get_child("sensordatas")) {
		SensorDataTable sdt;
		sdt.type = v.second.get<int>("type");

		Wt::WPanel *panel = new Wt::WPanel();
		panel->addStyleClass("centered-example");
		switch (sdt.type){
		case 0:
			panel->setTitle("Temperature");
			break;
		case 1:
			panel->setTitle("Light");
			break;
		case 2:
			panel->setTitle("Voltage");
			break;
		case 3:
			panel->setTitle("Current");
			break;

		}


	    BOOST_FOREACH(boost::property_tree::ptree::value_type &v2, v.second.get_child("values."))
	    {
	    	sdt.values.push_back(v2.second.get<float>("value"));
	    	sdt.timestamps.push_back(v2.second.get<boost::posix_time::ptime>("timestamp"));
	    }

		Wt::WTable *table = new Wt::WTable();
		table->setHeaderCount(1);
		table->elementAt(0, 0)->addWidget(new Wt::WText("Timestamp"));
		table->elementAt(0, 1)->addWidget(new Wt::WText("Value"));

		for (unsigned i = 0; i < sdt.values.size(); ++i) {
		    int row = i + 1;

		    table->elementAt(row, 0)->addWidget(new Wt::WText(Wt::WString(to_simple_string(sdt.timestamps.at(i)))));
		    table->elementAt(row, 1)->addWidget(new Wt::WText(Wt::WString::fromUTF8("{1}").arg(sdt.values.at(i))));
		}

		panel->setCentralWidget(table);

		containerTable->elementAt(typeNum/2, typeNum%2)->addWidget(panel);

//		Wt::WStandardItemModel *model = new Wt::WStandardItemModel(sdt.values.size(),2,this);
//		for (unsigned i = 0; i < sdt.values.size(); ++i) {
//			Wt::WStandardItem *item1 = new Wt::WStandardItem();
//			item1->setData(sdt.timestamps.at(i));
//			Wt::WStandardItem *item2 = new Wt::WStandardItem();
//			item2->setData(sdt.values.at(i));
//			model->setItem(i,0, item1);
//			model->setItem(i,1, item2);
//		}

//		Wt::Chart::WCartesianChart *chart = new Wt::Chart::WCartesianChart();
//		chart->setBackground(Wt::WColor(220, 220, 220));
//		chart->setModel(model);
//		chart->setXSeriesColumn(0);
//		chart->setLegendEnabled(true);
//		chart->setType(Wt::Chart::ScatterPlot);
//		chart->axis(Wt::Chart::XAxis).setScale(Wt::Chart::DateScale);
//		hbox->addWidget(chart);

		typeNum++;
	}

	Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
	hbox->addWidget(containerTable);
	setLayout(hbox);
	Wt::WApplication::instance()->triggerUpdate();
}
void NodeDetails::showRequestErrorMessage(std::string msg) {
	addWidget(new Wt::WText(msg.c_str()));
}

} /* namespace RFSNMAN */
