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
#include <Wt/WAnimation>
#include <Wt/WScrollArea>
#include <Wt/Chart/WCartesianChart>
#include <Wt/Chart/WDataSeries>
//#include <Wt/WAbstractItemModel>
//#include <Wt/WAbstractItemView>
//#include <Wt/WApplication>
//#include <Wt/WContainerWidget>
#include <Wt/WDateTime>
//#include <Wt/WEnvironment>
//#include <Wt/WPaintedWidget>
//#include <Wt/Chart/WAxis>
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
	std::istringstream is(response.body());
	read_json(is, result);

	Wt::WTabWidget *tabW = new Wt::WTabWidget(this);

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

		//tablePanel->setAnimation(tableAnimation);
		//chartPanel->setAnimation(chartAnimation);

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

	tabW->addTab(tableScrollArea, "Tables", Wt::WTabWidget::PreLoading);
	tabW->addTab(chartScrollArea, "Charts", Wt::WTabWidget::PreLoading);
	addWidget(tabW);
	Wt::WApplication::instance()->triggerUpdate();
}
void NodeDetails::showRequestErrorMessage(std::string msg) {
	addWidget(new Wt::WText(msg.c_str()));
}

} /* namespace RFSNMAN */
