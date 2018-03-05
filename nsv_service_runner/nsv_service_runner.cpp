#define _TURN_OFF_PLATFORM_STRING 1

#include "../nsv_service/nsv_service.h"
#include "../lib/string.h"

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

boost::asio::io_context io;
std::unique_ptr<crossover::nsv::nsv_service> nsv_service;

namespace bpo = boost::program_options;

void signal_handler(const boost::system::error_code& error, int signal_number)
{
	io.stop();
	std::cout << "Termination signal received. Exiting..."  << std::endl;
	nsv_service->stop().wait();
	std::cout << "Service stopped." << std::endl;
}

int main(int argc, char **argv)
{
	try
	{
		// Configure command line options
		bpo::options_description opt_desc{"Options"};
		opt_desc.add_options()
			("address,a", bpo::value<std::string>()->default_value("http://127.0.0.1:34567"), "Service address");
		opt_desc.add_options()
			("mongodb_uri,d", bpo::value<std::string>()->default_value("mongodb://localhost:27017/?minPoolSize=1&maxPoolSize=100"), "MongoDB URI");
		bpo::variables_map opt_var_map;
		bpo::store (bpo::parse_command_line(argc, argv, opt_desc), opt_var_map);
		bpo::notify(opt_var_map);
		
		auto service_address = opt_var_map["address"].as<std::string>();
		auto mongo_uri = opt_var_map["mongodb_uri"].as<std::string>();


		// Setup Ctrl-C signal
		boost::asio::signal_set signals{ io, SIGINT };
		signals.async_wait(signal_handler);

		// Start service
		std::cout << "Starting service" << std::endl;
		std::cout << "Address:" << service_address  << std::endl;
		std::cout << "MongoDB URI:" << mongo_uri << std::endl;
		nsv_service = std::make_unique<crossover::nsv::nsv_service>(service_address,mongo_uri);

		auto task = nsv_service->start();
		task.wait();
		std::cout << "Service listening. Hit Ctrl-C to stop." << std::endl;

		// Wait for termination signal
		io.run();

	}
	catch (std::exception & e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}

	return 0;
}
