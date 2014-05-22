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
#include <Wt/WServer>
#include <Wt/WTabWidget>
#include <Wt/WAnimation>
#include <Wt/WScrollArea>
#include <Wt/Chart/WCartesianChart>
#include <Wt/Chart/WDataSeries>
#include <Wt/WDateTime>
#include <Wt/WStandardItem>
#include <Wt/WStandardItemModel>
#include <Wt/WVBoxLayout>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>
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
	pollRunning = false;
}

NodeDetails::~NodeDetails() {
	//delete contents;
}

void NodeDetails::handleTabChange(int index){
	if (index == tabW->indexOf(currentValuesContainer)){
		pollRunning = true;
		boost::thread pollThread = boost::thread(&NodeDetails::pollCurrentValues, this);
	} else {
		pollRunning = false;
	}
}

void NodeDetails::pollCurrentValues(){
	boost::posix_time::seconds delay(2);
	boost::posix_time::milliseconds shortDelay(500);

	Wt::Http::Client *client = new Wt::Http::Client(Wt::WApplication::instance());
	client->setTimeout(15);
	client->setMaximumResponseSize(10 * 1024);
	client->done().connect(boost::bind(&NodeDetails::pollResponseArrived, this, _1, _2));

	while (pollRunning){
		std::cout <<"runnn" << std::endl;
		boost::this_thread::sleep(delay);
		for (int i=0; i<4; i++){
			std::stringstream url;
			url << "http://" << manager->getGatewayAddress() << "/measuresensordata/by-address/" << address << "/" <<i;
			client->get(url.str());
			boost::this_thread::sleep(shortDelay);
		}
	}
}

void NodeDetails::postCurrentValue(int type, float value){
	std::stringstream val;
	val << value;

	switch(type){
	case 0:
		tempValueText->setText(val.str().c_str());
		break;
	case 1:
		lightValueText->setText(val.str().c_str());
		break;
	case 2:
		voltageValueText->setText(val.str().c_str());
		break;
	case 3:
		currentValueText->setText(val.str().c_str());
		break;

	}
}

void NodeDetails::pollResponseArrived(boost::system::error_code err, const Wt::Http::Message& response){
	boost::property_tree::ptree result;
	std::istringstream is(response.body());
	read_json(is, result);

	int type = result.get<int>("type");
	float value = result.get<float>("value");
	Wt::WServer::instance()->post(manager->sessionId(), boost::bind(&NodeDetails::postCurrentValue, this, type, value));

	addWidget(contents);
}

void NodeDetails::responseArrived(boost::system::error_code err, const Wt::Http::Message& response) {
	boost::property_tree::ptree result;
	std::istringstream is(response.body());
	read_json(is, result);

	tabW = new Wt::WTabWidget(this);

	currentValuesContainer = new Wt::WContainerWidget();


	Wt::WTable *currentValuesTable= new Wt::WTable();
	currentValuesTable->addStyleClass("table");
	currentValuesTable->setHeaderCount(1);
	currentValuesTable->elementAt(0, 0)->addWidget(new Wt::WText("Temperature"));
	currentValuesTable->elementAt(0, 1)->addWidget(new Wt::WText("Light"));
	currentValuesTable->elementAt(0, 2)->addWidget(new Wt::WText("Voltage"));
	currentValuesTable->elementAt(0, 3)->addWidget(new Wt::WText("Current"));

	tempValueText = new Wt::WText("-");
	lightValueText = new Wt::WText("-");
	voltageValueText = new Wt::WText("-");
	currentValueText = new Wt::WText("-");

	currentValuesTable->elementAt(1, 0)->addWidget(tempValueText);
	currentValuesTable->elementAt(1, 1)->addWidget(lightValueText);
	currentValuesTable->elementAt(1, 2)->addWidget(voltageValueText);
	currentValuesTable->elementAt(1, 3)->addWidget(currentValueText);
	currentValuesContainer->addWidget(currentValuesTable);

	Wt::WScrollArea *tableScrollArea = new Wt::WScrollArea();
	Wt::WScrollArea *chartScrollArea = new Wt::WScrollArea();
	chartScrollArea->setHeight(600);
	chartScrollArea->setWidth(960);
	chartScrollArea->addStyleClass("bordered");
	tableScrollArea->setHeight(600);
	tableScrollArea->setWidth(960);
	tableScrollArea->addStyleClass("bordered");

	Wt::WTable *tableContainerTable = new Wt::WTable();
	tableContainerTable->addStyleClass("table");
	tableContainerTable->setHeaderCount(0);
	tableContainerTable->setWidth(930);

	Wt::WTable *chartContainerTable = new Wt::WTable();
	chartContainerTable->addStyleClass("table");
	chartContainerTable->setHeaderCount(0);
	chartContainerTable->setWidth(930);


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

		tablePanel->setCollapsible(true);
		chartPanel->setCollapsible(true);

		tablePanel->setWidth(450);
		chartPanel->setWidth(450);

		Wt::WAnimation tableAnimation(Wt::WAnimation::SlideInFromTop,Wt::WAnimation::EaseOut, 333);
		Wt::WAnimation chartAnimation(Wt::WAnimation::SlideInFromTop,Wt::WAnimation::EaseOut, 333);

		tablePanel->setAnimation(tableAnimation);
		chartPanel->setAnimation(chartAnimation);

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

		table->toggleStyleClass("table-hover", true);
		table->toggleStyleClass("table-striped", true);

		for (unsigned i = 0; i < sdt.values.size(); ++i) {
		    int row = i + 1;

		    table->elementAt(row, 0)->addWidget(new Wt::WText(Wt::WString(to_simple_string(sdt.timestamps.at(i)))));
		    table->elementAt(row, 1)->addWidget(new Wt::WText(Wt::WString::fromUTF8("{1}").arg(sdt.values.at(i))));
		}



		tablePanel->setCentralWidget(table);
		tableContainerTable->elementAt(typeNum/2, typeNum%2)->addWidget(tablePanel);

		Wt::WStandardItemModel *model = new Wt::WStandardItemModel(sdt.values.size(),2,this);
		for (unsigned i = 0; i < sdt.values.size(); i++) {
			model->setData(i,0, boost::any(sdt.timestamps.at(i)));
			model->setData(i,1, boost::any(sdt.values.at(i)));
		}

		Wt::Chart::WCartesianChart *chart = new Wt::Chart::WCartesianChart();
		chart->setBackground(Wt::WColor(220, 220, 220));
		chart->setModel(model);
		chart->setXSeriesColumn(0);
		chart->setLegendEnabled(false);
		chart->setType(Wt::Chart::ScatterPlot);

		chart->axis(Wt::Chart::XAxis).setScale(Wt::Chart::DateTimeScale );

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

	tableScrollArea->setWidget(tableContainerTable);
	chartScrollArea->setWidget(chartContainerTable);
	tableScrollArea->setVerticalScrollBarPolicy( Wt::WScrollArea::ScrollBarAlwaysOn);
	chartScrollArea->setVerticalScrollBarPolicy( Wt::WScrollArea::ScrollBarAlwaysOn);

	tabW->addTab(tableScrollArea, "Tables");
	tabW->addTab(chartScrollArea, "Charts");
	tabW->addTab(currentValuesContainer, "Current values");
	tabW->currentChanged().connect(boost::bind(&NodeDetails::handleTabChange, this, _1));
	addWidget(tabW);
	Wt::WApplication::instance()->triggerUpdate();
}
void NodeDetails::showRequestErrorMessage(std::string msg) {
	addWidget(new Wt::WText(msg.c_str()));
}

} /* namespace RFSNMAN */
