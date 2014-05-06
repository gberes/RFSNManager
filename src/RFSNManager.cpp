/*
 * RFSNManager.cpp
 *
 *  Created on: May 3, 2014
 *      Author: gberes
 */

#include "RFSNManager.h"
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>

namespace RFSNMAN {


RFSNManager::~RFSNManager() {
}



RFSNManager::RFSNManager(const Wt::WEnvironment& env)
  : Wt::WApplication(env), nodeList(this)
{
  setTitle("RF sensor network manager");
  useStyleSheet("/resources/themes/bootstrap/3/bootstrap.css");
  root()->addWidget(&nodeList);
}

void RFSNManager::showNode(std::string address){
	root()->addWidget(new Wt::WText(address));
}

} /* namespace RFSNMAN */


Wt::WApplication *createApplication(const Wt::WEnvironment& env)
{
  return new RFSNMAN::RFSNManager(env);
}

int main(int argc, char **argv)
{
  return WRun(argc, argv, &createApplication);
}
