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
	class WBorderLayout;
}

namespace RFSNMAN {

class RFSNManager : public Wt::WApplication {
public:
	RFSNManager(const Wt::WEnvironment& env, std::string gwaddress);
	virtual ~RFSNManager();
	void showNode(std::string address);
	const std::string& getGatewayAddress() const;

private:
	std::string gatewayAddress;
	NodeList nodeList;
	 Wt::WBorderLayout *layout;

};

} /* namespace RFSNMAN */

#endif /* RFSNMANAGER_H_ */
