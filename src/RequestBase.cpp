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
	Wt::Http::Client *client = new Wt::Http::Client(Wt::WApplication::instance());
	client->setTimeout(15);
	client->setMaximumResponseSize(10 * 1024);
	client->done().connect(boost::bind(&RequestBase::handleHttpResponse, this, _1, _2));

	if (client->get(url.c_str())){
	} else {
	}
}

void RequestBase::handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response) {
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
