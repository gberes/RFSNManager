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
#include <string>

namespace Wt{
namespace Http{
class Message;
}
}

namespace RFSNMAN {

class NodeDetails : public Wt::WContainerWidget {
public:
	NodeDetails(std::string address);
	virtual ~NodeDetails();

	void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);
};

} /* namespace RFSNMAN */

#endif /* NODEDETAILS_H_ */
