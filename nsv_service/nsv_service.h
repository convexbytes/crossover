#pragma once

#include <memory>

#include <cstdint>
#include <iostream>

#include <cpprest\http_client.h>
#include <cpprest\http_listener.h>

using web::http::experimental::listener::http_listener;

namespace crossover
{
	///
	///
	///
	class nsv_service
	{
	public:
		nsv_service(utility::string_t address);
		~nsv_service();
		concurrency::task<void> start();
		concurrency::task<void> stop();

	private:
		/// GET request handler
		void handle_get(web::http::http_request request);

		/// HTTP listener
		std::shared_ptr<http_listener> m_http_listener;

		/// @Service startup address
		utility::string_t m_address;
	};


}