/*
 * RequestBase.h
 *
 *  Created on: May 6, 2014
 *      Author: gberes
 */

#ifndef REQUESTBASE_H_
#define REQUESTBASE_H_

#include <string>
#include <boost/system/error_code.hpp>

namespace Wt{
namespace Http{
class Message;
}
}

namespace RFSNMAN {

class RequestBase {
public:
	RequestBase() ;
	virtual ~RequestBase();

	void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);
	void getRequest(std::string url);

	virtual void responseArrived(boost::system::error_code err, const Wt::Http::Message& response) = 0;
	virtual void showRequestErrorMessage(std::string msg) = 0;
};

} /* namespace RFSNMAN */

#endif /* REQUESTBASE_H_ */
