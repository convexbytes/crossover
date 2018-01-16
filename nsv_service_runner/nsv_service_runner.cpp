#include "../nsv_service/nsv_service.h"

#include <boost\asio.hpp>
#include <boost\program_options.hpp>

#include "..\lib\string.h"

boost::asio::io_context io;
std::unique_ptr<crossover::nsv_service> nsv_service;
std::string service_address;

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
		bpo::variables_map opt_var_map;
		bpo::store (bpo::parse_command_line(argc, argv, opt_desc), opt_var_map);
		bpo::notify(opt_var_map);
		if (opt_var_map.count("address"))
			service_address = opt_var_map["address"].as<std::string>();
		else
			service_address = "http://127.0.0.1:34567";



		// Setup Ctrl-C signal
		boost::asio::signal_set signals{ io, SIGINT };
		signals.async_wait(signal_handler);

		// Start service
		std::cout << "Starting service" << std::endl;
		nsv_service = std::make_unique<crossover::nsv_service>(crossover::string::to_utility_string(service_address));
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
