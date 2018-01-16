#include <gtest\gtest.h>


/// bson and mongo must be included BEFORE cpprestsdk
/// cpprestsdk defines U, which causes an issue with boost type_traits.hpp,
/// type_traits.hpp uses U as a template typename "... template <class U> ..."

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

#include <cpprest\http_client.h>
#include <cpprest\filestream.h>
#include <cpprest\http_listener.h>

#include "..\nsv_service\nsv_service.h"
#include "..\lib\string.h"

/// Service must start and stop correctly
TEST(DISABLE_ServiceStartStopTest, StartStop)
{
	crossover::nsv_service service{U("https://127.0.0.1:34567")};
	service.start().wait();
	service.stop().wait();
}

/// Starts service and performs a Get request to it
/// 
/// MONGOD must be running
TEST(DISABLE_ServiceGetTest, GetShouldSucceed) 
{
	
	// Start service
	utility::string_t address = U("http://127.0.0.1:34567");
	crossover::nsv_service service(address);
	service.start().wait();

	// Create URI
	web::uri_builder uri_builder(U(""));

	// Create client
	web::http::client::http_client client(address);

	// Setup query
	uri_builder.append_query(U("Vehicle_Registration"), U("OI8CBS9"));

	// Request GET
	auto req = client.request(web::http::methods::GET, uri_builder.to_string()).then([=](web::http::http_response response)
	{
		auto task = response.extract_json().then([=](web::json::value v) {
			// Helpful when debugging
			ucout << U("Received response:") << v.serialize() << std::endl;
		}).wait();
		

	}).wait();

	service.stop().wait();
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
