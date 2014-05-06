/*
 * NodeList.h
 *
 *  Created on: May 3, 2014
 *      Author: gberes
 */

#ifndef NODELIST_H_
#define NODELIST_H_

#include <Wt/WContainerWidget>
#include <Wt/WStackedWidget>
#include <Wt/WMenu>
#include <boost/system/error_code.hpp>

namespace Wt{
namespace Http{
class Message;
}
}

namespace RFSNMAN {

class RFSNManager;

class NodeList : public Wt::WContainerWidget {
public:
	NodeList(RFSNManager* man);
	virtual ~NodeList();

protected:
	Wt::WStackedWidget contents;
	Wt::WMenu* menu;
	RFSNManager* manager;

	void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);
};

} /* namespace RFSNMAN */

#endif /* NODELIST_H_ */
