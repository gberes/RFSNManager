/*
 * RequestBase.cpp
 *
 *  Created on: May 6, 2014
 *      Author: gberes
 */

#include "RequestBase.h"
#include <Wt/Http/Client>
#include <Wt/WApplication>
#include <Wt/Http/Client>

namespace RFSNMAN {

RequestBase::RequestBase() {
	// TODO Auto-generated constructor stub

}

RequestBase::~RequestBase() {
	// TODO Auto-generated destructor stub
}

void RequestBase::getRequest(std::string url){
	std::cout << "RequestBase::getRequest/" << __LINE__ << ": " << std::endl;
	Wt::Http::Client *client = new Wt::Http::Client(Wt::WApplication::instance());
	client->setTimeout(15);
	client->setMaximumResponseSize(10 * 1024);
	client->done().connect(boost::bind(&RequestBase::handleHttpResponse, this, _1, _2));

	if (client->get(url.c_str())){
		std::cout << "RequestBase::getRequest/" << __LINE__ << ": get ok" << std::endl;
	} else {
		std::cout << "RequestBase::getRequest/" << __LINE__ << ": get fail" << std::endl;
	}
}

void RequestBase::handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response) {
	std::cout << "RequestBase::handleHttpResponse/" << __LINE__ << ": " << std::endl;
	if (response.status() == 200){
		responseArrived(err,response);
	} else {
		std::stringstream ss;
		ss << "Unable to connect to gateway: " << response.status() << " (" << err.message() << ")";
		if (response.status() == -1){
			ss << " (Is gateway running?)";
		}

		showRequestErrorMessage(ss.str());
	}

}

} /* namespace RFSNMAN */
