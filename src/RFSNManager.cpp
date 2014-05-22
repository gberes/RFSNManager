/*
 * RFSNManager.cpp
 *
 *  Created on: May 3, 2014
 *      Author: gberes
 */

#include "RFSNManager.h"
#include "NodeDetails.h"
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WServer>
#include <Wt/WBorderLayout>
#include <iostream>

namespace RFSNMAN {


RFSNManager::~RFSNManager() {
}



RFSNManager::RFSNManager(const Wt::WEnvironment& env, std::string gwaddress)
  : Wt::WApplication(env),gatewayAddress(gwaddress), nodeList(this)
{
  WApplication::enableUpdates(true);
  setTitle("RF sensor network manager");
  useStyleSheet("/resources/themes/bootstrap/3/bootstrap.css");
  useStyleSheet("/resources/mystyles.css");
  layout = new Wt::WBorderLayout();
  root()->setLayout(layout);
  layout->addWidget(&nodeList, Wt::WBorderLayout::West);
  layout->addWidget(new Wt::WText("<h1>RF sensor network manager</h1>"), Wt::WBorderLayout::North);
  layout->addWidget(new Wt::WText("<h2>Please select a node</h2>"), Wt::WBorderLayout::Center);
}

void RFSNManager::showNode(std::string address){
	Wt::WWidget* currentNode = layout->widgetAt(Wt::WBorderLayout::Center);
	if (currentNode != 0){
		layout->removeWidget(currentNode);
		// todo delete?
	}
	std::cout << "RFSNManager::showNode/" << __LINE__ << ": show node " << address << std::endl;
	Wt::WServer::instance()->post(WApplication::sessionId(), boost::bind(&Wt::WBorderLayout::addWidget, layout, new NodeDetails(address, this), Wt::WBorderLayout::Center));
}

const std::string& RFSNManager::getGatewayAddress() const {
	return gatewayAddress;
}

} /* namespace RFSNMAN */

std::string gwaddr;


Wt::WApplication *createApplication(const Wt::WEnvironment& env)
{
  return new RFSNMAN::RFSNManager(env, gwaddr);
}

int main(int argc, char **argv)
{

  if (argc < 7) {
      std::cerr << "Usage: " << argv[0] << " <gateway address> --docroot=<docroot> --http-address <http address> --http-port <port>" << std::endl;
      return 1;
  } else {
	  gwaddr = std::string(argv[1]);
  }
  return WRun(argc, argv, &createApplication);
}
