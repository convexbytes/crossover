#pragma once

/// bson and mongo must be included BEFORE cpprestsdk
/// cpprestsdk defines U, which causes an issue with boost type_traits.hpp,
/// type_traits.hpp uses U as a template typename: "... template <class U> ..."
/// or alternatively define the macro _TURN_OFF_PLATFORM_STRING
#define _TURN_OFF_PLATFORM_STRING 1

#include <mongocxx/pool.hpp>
#include <cpprest/http_headers.h>
#include <cpprest/http_listener.h>

#include <memory>

using web::http::experimental::listener::http_listener;

namespace crossover {
	namespace nsv {
	///
	///
	///
	class nsv_service
	{
	public:
		nsv_service(std::string service_address,
			std::string mongo_uri);
		~nsv_service();
		concurrency::task<void> start();
		concurrency::task<void> stop();

	private:
		/// GET request handler
		void handle_get(web::http::http_request request);

		/// HTTP listener
		std::shared_ptr<http_listener> m_http_listener;

		/// Service startup address
		utility::string_t m_service_address;

		/// Location of mongo db
		std::string m_mongodb_uri; 

		std::unique_ptr<mongocxx::pool> m_mongodb_pool;
	};

	}
}