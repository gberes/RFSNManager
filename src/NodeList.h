/*
 * NodeList.h
 *
 *  Created on: May 3, 2014
 *      Author: gberes
 */

#ifndef NODELIST_H_
#define NODELIST_H_

#include <Wt/WContainerWidget>
#include <Wt/WMenu>
#include <boost/system/error_code.hpp>
#include "RequestBase.h"

namespace Wt{
namespace Http{
class Message;
}
class WStackedWidget;
}

namespace RFSNMAN {

class RFSNManager;

class NodeList : public Wt::WContainerWidget, public RequestBase {
public:
	NodeList(RFSNManager* man);
	virtual ~NodeList();

protected:
	Wt::WStackedWidget* contents;
	Wt::WMenu* menu;
	RFSNManager* manager;

	void responseArrived(boost::system::error_code err, const Wt::Http::Message& response);
	void showRequestErrorMessage(std::string msg);
	void onElementClicked(std::string address );
};

} /* namespace RFSNMAN */

#endif /* NODELIST_H_ */
