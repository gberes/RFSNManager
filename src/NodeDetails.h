/*
 * NodeDetails.h
 *
 *  Created on: May 4, 2014
 *      Author: gberes
 */

#ifndef NODEDETAILS_H_
#define NODEDETAILS_H_

#include <boost/system/error_code.hpp>
#include <Wt/WContainerWidget>
#include <Wt/WPanel>
#include <string>
#include "RequestBase.h"

namespace Wt{
namespace Http{
class Message;
}
class WStackedWidget;
class WPanel;
}

namespace RFSNMAN {

class RFSNManager;

class NodeDetails : public Wt::WContainerWidget, public RequestBase {
public:
	NodeDetails(std::string address, RFSNManager* manager);
	virtual ~NodeDetails();

	void responseArrived(boost::system::error_code err, const Wt::Http::Message& response);
	void showRequestErrorMessage(std::string msg);

protected:
	RFSNManager* manager;
	Wt::WStackedWidget* contents;
	Wt::WPanel* panel;
	std::string address;
};

} /* namespace RFSNMAN */

#endif /* NODEDETAILS_H_ */
