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
  root()->addWidget(&nodeList);
}

void RFSNManager::showNode(std::string address){
	std::cout << "RFSNManager::showNode/" << __LINE__ << ": show node " << address << std::endl;
	boost::function<void(NodeDetails*)> addWidgetFunc = root()->addWidget(new NodeDetails(address, this));
	WServer::instance()->post(WApplication::sessionId(), addWidgetFunc);
	//root()->//addWidget(new NodeDetails(address, this));
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
