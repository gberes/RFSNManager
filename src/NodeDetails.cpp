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
#include <Wt/WTabWidget>

#include <Wt/Chart/WCartesianChart>
#include <Wt/Chart/WDataSeries>
//#include <Wt/WAbstractItemModel>
//#include <Wt/WAbstractItemView>
//#include <Wt/WApplication>
//#include <Wt/WContainerWidget>
#include <Wt/WDateTime>
//#include <Wt/WEnvironment>
//#include <Wt/WPaintedWidget>
#include <Wt/Chart/WAxis>
#include <Wt/WShadow>
#include <Wt/WStandardItem>
#include <Wt/WStandardItemModel>
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

	Wt::WTabWidget *tabW = new Wt::WTabWidget(this);

	Wt::WTable *tableContainerTable = new Wt::WTable();
	tableContainerTable->addStyleClass("table");
	tableContainerTable->setHeaderCount(0);

	Wt::WTable *chartContainerTable = new Wt::WTable();
	chartContainerTable->addStyleClass("table");
	chartContainerTable->setHeaderCount(0);

	struct SensorDataTable {
		int type;
		std::vector<boost::posix_time::ptime> timestamps;
		std::vector<float> values;
	};

	int typeNum = 0;

	BOOST_FOREACH(boost::property_tree::ptree::value_type &v,result.get_child("sensordatas")) {
		SensorDataTable sdt;
		sdt.type = v.second.get<int>("type");

		Wt::WPanel *tablePanel = new Wt::WPanel();
		Wt::WPanel *chartPanel = new Wt::WPanel();
		tablePanel->addStyleClass("centered-example");
		chartPanel->addStyleClass("centered-example");
		switch (sdt.type){
		case 0:
			tablePanel->setTitle("Temperature");
			chartPanel->setTitle("Temperature");
			break;
		case 1:
			tablePanel->setTitle("Light");
			chartPanel->setTitle("Light");
			break;
		case 2:
			tablePanel->setTitle("Voltage");
			chartPanel->setTitle("Voltage");
			break;
		case 3:
			tablePanel->setTitle("Current");
			chartPanel->setTitle("Current");
			break;

		}


	    BOOST_FOREACH(boost::property_tree::ptree::value_type &v2, v.second.get_child("values."))
	    {
	    	sdt.values.push_back(v2.second.get<float>("value"));
	    	sdt.timestamps.push_back(v2.second.get<boost::posix_time::ptime>("timestamp"));
	    }

		Wt::WTable *table = new Wt::WTable();
		table->addStyleClass("table");
		table->setHeaderCount(1);
		table->elementAt(0, 0)->addWidget(new Wt::WText("Timestamp"));
		table->elementAt(0, 1)->addWidget(new Wt::WText("Value"));

		for (unsigned i = 0; i < sdt.values.size(); ++i) {
		    int row = i + 1;

		    table->elementAt(row, 0)->addWidget(new Wt::WText(Wt::WString(to_simple_string(sdt.timestamps.at(i)))));
		    table->elementAt(row, 1)->addWidget(new Wt::WText(Wt::WString::fromUTF8("{1}").arg(sdt.values.at(i))));
		}

		tablePanel->setCentralWidget(table);
		tableContainerTable->elementAt(typeNum/2, typeNum%2)->addWidget(tablePanel);

		Wt::WStandardItemModel *model = new Wt::WStandardItemModel(sdt.values.size(),2,this);
		for (unsigned i = 0; i < sdt.values.size(); i++) {
			Wt::WStandardItem *item1 = new Wt::WStandardItem();
			item1->setData(Wt::WDateTime(sdt.timestamps.at(i)));
			Wt::WStandardItem *item2 = new Wt::WStandardItem();
			item2->setData(sdt.values.at(i));
			model->setItem(i,0, item1);
			model->setItem(i,1, item2);
			std::cout<< " Adding to model: " << sdt.values.at(i) << std::endl;
		}

		Wt::Chart::WCartesianChart *chart = new Wt::Chart::WCartesianChart();
		chart->setBackground(Wt::WColor(220, 220, 220));
		chart->setModel(model);
		chart->setXSeriesColumn(0);
		chart->setLegendEnabled(false);
		chart->setType(Wt::Chart::ScatterPlot);

		Wt::Chart::WAxis& x_axis  = chart->axis(Wt::Chart::Axis::XAxis);
		Wt::Chart::WAxis& y1_axis = chart->axis(Wt::Chart::Axis::Y1Axis);
		x_axis.setScale(Wt::Chart::DateTimeScale );

		chart->setPlotAreaPadding(40, Wt::Left | Wt::Top | Wt::Bottom);
		chart->setPlotAreaPadding(40, Wt::Right);


		Wt::Chart::WDataSeries s(1, Wt::Chart::LineSeries);
		s.setShadow(Wt::WShadow(3, 3, Wt::WColor(0, 0, 0, 127), 3));
		chart->addSeries(s);

		chart->resize(400, 200);
		chart->setMargin(Wt::WLength::Auto, Wt::Left | Wt::Right);

		chartPanel->setCentralWidget(chart);
		chartContainerTable->elementAt(typeNum/2, typeNum%2)->addWidget(chartPanel);

		typeNum++;
	}

	tabW->addTab(tableContainerTable, "Tables", Wt::WTabWidget::PreLoading);
	tabW->addTab(chartContainerTable, "Charts", Wt::WTabWidget::PreLoading);
//	Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
//	hbox->addWidget(containerTable);
//	setLayout(hbox);
	addWidget(tabW);
	Wt::WApplication::instance()->triggerUpdate();
}
void NodeDetails::showRequestErrorMessage(std::string msg) {
	addWidget(new Wt::WText(msg.c_str()));
}

} /* namespace RFSNMAN */
