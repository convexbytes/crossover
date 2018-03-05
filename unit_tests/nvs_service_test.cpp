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

/// 
TEST(UTF16_UTF8_Conversion, UTF8_To_UTF16)
{
	std::string good_utf8 = u8" Ich möchte einen kaltes Bier a á é e í i ó U ú U itsuka 日本 ni ikitainda!";
	std::wstring good_utf16 = L" Ich möchte einen kaltes Bier a á é e í i ó U ú U itsuka 日本 ni ikitainda!";
	std::wstring converted_utf16 = crossover::string::utf8_to_wtring(good_utf8);
	ASSERT_EQ(good_utf16, converted_utf16);
}

TEST(UTF16_UTF8_Conversion, UTF16_To_UTF8)
{

	std::string good_utf8 = u8" Ich möchte einen kaltes Bier a á é e í i ó U ú U itsuka 日本 ni ikitainda!";
	std::wstring good_utf16 = L" Ich möchte einen kaltes Bier a á é e í i ó U ú U itsuka 日本 ni ikitainda!";
	std::string converted_utf8 = crossover::string::utf16_to_utf8(good_utf16);
	ASSERT_EQ(good_utf8, converted_utf8);
}


/// Service must start and stop correctly
TEST(ServiceStartStopTest, StartStop)
{
	std::string addr{ "http://127.0.0.1:60000" };
	std::string mongo_uri{ "mongodb://localhost:27017/?minPoolSize=1&maxPoolSize=100" };
	crossover::nsv::nsv_service service {addr, mongo_uri};
	service.start().wait();
	service.stop().wait();
}

/// Starts service and performs a Get request to it
/// 
/// MONGOD must be running at address localhost:27017
TEST(ServiceGetTest, GetShouldSucceed) 
{
	
	// Start service
	std::string addr{ "http://127.0.0.1:60001" };
	std::string mongo_uri{ "mongodb://localhost:27017/?minPoolSize=1&maxPoolSize=100" };
	crossover::nsv::nsv_service service(addr, mongo_uri);
	service.start().wait();	

	// Create client
	utility::string_t uaddr{ _XPLATSTR("http://127.0.0.1:60001") };
	web::http::client::http_client client(uaddr);

	// Setup query
	web::uri_builder uri_builder(_XPLATSTR(""));
	uri_builder.append_query(_XPLATSTR("Vehicle_Registration"), _XPLATSTR("OI8CBS9"));

	// Request GET
	auto req = client.request(web::http::methods::GET, uri_builder.to_string()).then(
		[=](web::http::http_response response){
		auto task = response.extract_json(true).then([=](web::json::value v) {
			// Helpful when debugging
			ucout << _XPLATSTR("Matches:") << v[_XPLATSTR("matches")].as_integer() << std::endl;
		}).wait();
	}).wait();

	service.stop().wait();
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
