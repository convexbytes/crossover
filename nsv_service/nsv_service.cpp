/// bson and mongo must be included BEFORE cpprestsdk
/// cpprestsdk defines U, which causes an issue with boost type_traits.hpp,
/// type_traits.hpp uses U as a template typename: "... template <class U> ..."

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/helpers.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx\client.hpp>
#include <mongocxx\pool.hpp>
#include <mongocxx\stdx.hpp>
#include <mongocxx\uri.hpp>
#include <mongocxx\instance.hpp>

#include <cpprest\http_client.h>
#include <cpprest\filestream.h>
#include <cpprest\http_listener.h>

#include <locale>
#include <codecvt>
#include <iostream>

#include "nsv_service.h"


namespace crossover
{
	mongocxx::instance mongodb_instance{};

	// TODO: construct the pool uri somewhere else like command line args or conf file
	mongocxx::uri mongodb_pool_uri{ "mongodb://localhost:27017/?minPoolSize=100&maxPoolSize=1" };

	/// Unique mongo db pool for the entire web app
	mongocxx::pool mongodb_pool{ mongodb_pool_uri};
	

	nsv_service::nsv_service(utility::string_t address) : m_address(address)
	{
		// Create listener
		m_http_listener = std::make_unique<http_listener>(web::uri_builder{ m_address }.to_uri().to_string());

		// Add GET support
		m_http_listener->support(web::http::methods::GET, std::bind(&nsv_service::handle_get, this, std::placeholders::_1));
	}

	nsv_service::~nsv_service()
	{
	}

	inline std::string utf16_to_utf8(std::string const & s)
	{
		return s;
	}

	
	inline std::string utf16_to_utf8(std::wstring const & s)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8converter{};
		return utf8converter.to_bytes(s);
	}


	bsoncxx::builder::basic::document
	create_query_document(std::map<utility::string_t, utility::string_t> filters)
	{
		bsoncxx::builder::basic::document doc{};
		std::list<utility::string_t> filters_names
			{U("Vehicle_Registration"), U("Vehicle_Make"), U("Vehicle_Model"), U("Vehicle_Owner")};
		for (auto && filter_name : filters_names)
			if (filters.find(filter_name) != filters.end())
			{
				std::string value = utf16_to_utf8(filters[filter_name]);
				std::string name = utf16_to_utf8(filter_name);
				doc.append(bsoncxx::builder::basic::kvp(name, 
					bsoncxx::types::b_regex{".*" + value + ".*", "i"}));
			}
		return doc;
	}

	void log_request(mongocxx::database & db, web::http::http_request const & request)
	{
		using bsoncxx::builder::basic::kvp;
		using bsoncxx::builder::basic::document;
		// To convert query to utf8 strings, which is required by BSON 
		std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8converter{};
		// utf8 decoded URI
		std::string utf8_uri = utf8converter.to_bytes(web::uri::decode(request.relative_uri().to_string()).c_str());
		document basic_builder{};
		basic_builder.append(kvp ("Time", bsoncxx::types::b_date{ std::chrono::system_clock::now() }));
		basic_builder.append(kvp("URI", utf8_uri));
		mongocxx::collection log_coll = db["Service_Search_Log"];
		log_coll.insert_one(basic_builder.view());
	}

	void nsv_service::handle_get(web::http::http_request request)
	{
		// TODO: tear down request for a more complex scenario than
		//       a root URI

		using bsoncxx::builder::basic::kvp;
		using bsoncxx::builder::basic::document;
		
		// Debugging
		ucout << request.to_string() << std::endl;

		try
		{
			// // Debugging. Wait 
			// using namespace std::chrono_literals;
			// std::this_thread::sleep_for(10s);

			// Access the database and collection
			mongocxx::pool::entry client = mongodb_pool.acquire();
			mongocxx::database db = (*client)["nsvd"];

			// To convert query to utf8 strings, which is required by BSON 
			std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8converter{};

			// Log the request
			log_request(db, request);

			// Decoded and split query
			std::map<utility::string_t, utility::string_t> split_query =
				web::uri::split_query(web::uri::decode(request.relative_uri().query()));

			// Using basic builder
			auto basic_builder = bsoncxx::builder::basic::document{};
			
			// Create the filter document
			auto query_doc = create_query_document(split_query);

			// Debugging
			// std::cout << bsoncxx::to_json(query_doc) << std::endl;
			
			// Limit the query to one thousand results
			mongocxx::options::find find_options{};
			find_options.limit(1000); // Only one thousand per query

			mongocxx::collection vehicles_coll = db["Stolen_Vehicles"];
			mongocxx::cursor result_cursor = vehicles_coll.find(query_doc.view(), find_options);
		
			// Construct result
			bsoncxx::builder::basic::array arr;
			for (auto doc : result_cursor)
				arr.append(doc);
			bsoncxx::builder::basic::document result;
			result.append(bsoncxx::builder::basic::kvp("data", arr));
			request.reply(web::http::status_codes::OK, bsoncxx::to_json(result)).wait();
		}
		// Todo: handle different types of exceptions
		catch (std::exception & e)
		{
			request.reply(web::http::status_codes::InternalError, e.what()).wait();
		}

		return;
	}

	concurrency::task<void> nsv_service::start()
	{
		return m_http_listener->open();	
	}

	concurrency::task<void> nsv_service::stop()
	{
		return m_http_listener->close();
	}

}