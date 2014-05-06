/*
 * RFSNManager.h
 *
 *  Created on: May 3, 2014
 *      Author: gberes
 */

#ifndef RFSNMANAGER_H_
#define RFSNMANAGER_H_

#include <Wt/WApplication>
#include "NodeList.h"
#include <string>

namespace Wt{
	class WLineEdit;
	class WText;
	class WEnvironment;
}

namespace RFSNMAN {

class RFSNManager : public Wt::WApplication {
public:
	RFSNManager(const Wt::WEnvironment& env);
	virtual ~RFSNManager();
	void showNode(std::string address);

private:
	NodeList nodeList;
};

} /* namespace RFSNMAN */

#endif /* RFSNMANAGER_H_ */
