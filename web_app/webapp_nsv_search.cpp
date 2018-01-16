#include "webapp_nsv_search.h"

#include <mongocxx\pool.hpp>
#include "..\lib\string.h"

namespace crossover
{
namespace nvsdweb
{

webapp_nsv_search::webapp_nsv_search(const Wt::WEnvironment& env) : Wt::WApplication(env)
{
	// Title
	// TODO: use resources for texts?
	this->setTitle("National Stolen Vehicle DataBase - Search");
	
	m_wt_theme = std::make_shared<Wt::WBootstrapTheme>();
	m_wt_theme->setVersion(Wt::WBootstrapTheme::Version::v3);
	this->setTheme(m_wt_theme);

	m_form_container = root()->addWidget(std::make_unique<Wt::WContainerWidget>());

	// Add vehicle search fields and labels
	m_form_container->addWidget(std::make_unique<Wt::WText>("Registration"));
	m_vehicle_registration_edit = m_form_container->addWidget(std::make_unique<Wt::WLineEdit>());
	m_form_container->addWidget(std::make_unique<Wt::WText>("Make"));
	m_vehicle_make_edit = m_form_container->addWidget(std::make_unique<Wt::WLineEdit>());
	m_form_container->addWidget(std::make_unique<Wt::WText>("Model"));
	m_vehicle_model_edit = m_form_container->addWidget(std::make_unique<Wt::WLineEdit>());
	m_form_container->addWidget(std::make_unique<Wt::WText>("Owner"));
	m_vehicle_owner_edit = m_form_container->addWidget(std::make_unique<Wt::WLineEdit>());

	// Add the search button
	Wt::WPushButton * search_button = root()->addWidget(std::make_unique<Wt::WPushButton>("search"));
	root()->addWidget(std::make_unique<Wt::WBreak>());

	// Add the search result to the app
	m_search_result = root()->addWidget(std::make_unique<Wt::WText>());

	// Add the results table

	m_results = root()->addWidget(std::make_unique<Wt::WTable>());
	m_results->addStyleClass("table-condensed");
	m_results->setHeaderCount(1);

	// Assign a handler for the search button
	search_button->clicked().connect(this, &webapp_nsv_search::on_search_button_click);
	
}

webapp_nsv_search::~webapp_nsv_search()
{
}


web::uri webapp_nsv_search::create_uri_from_input()
{
	web::uri_builder uri_builder;

	if (!m_vehicle_registration_edit->text().empty())
		uri_builder.append_query(utility::string_t(U("Vehicle_Registration")), m_vehicle_registration_edit->text());

	if (!m_vehicle_make_edit->text().empty())
		uri_builder.append_query(utility::string_t(U("Vehicle_Make")), m_vehicle_make_edit->text());

	if (!m_vehicle_model_edit->text().empty())
		uri_builder.append_query(utility::string_t(U("Vehicle_Model")), m_vehicle_model_edit->text());

	if (!m_vehicle_owner_edit->text().empty())
		uri_builder.append_query(utility::string_t(U("Vehicle_Owner")), m_vehicle_owner_edit->text());

	return uri_builder.to_uri();	
}

std::optional<std::string> webapp_nsv_search::validate_input()
{
	return {};
}

void webapp_nsv_search::on_search_error(std::string & e)
{
	std::stringstream display_text{};
	display_text
	 	<< "Something went wrong, please try again in a few minutes.\n"
	 	<< e;
	m_search_result->setText(display_text.str());	
}

void webapp_nsv_search::on_search_http_error(web::http::http_response & response)
{
	std::stringstream failure_text{};
	failure_text
		<< "Something went wrong, try again in a few minutes.\n"
		<< "Status code:" << response.status_code();
	m_search_result->setText(failure_text.str());
}


void webapp_nsv_search::on_search_button_click()
{
	web::uri uri = create_uri_from_input();

	// TODO: remove hardcoded service address
	utility::string_t nsv_service_address{ U("http://127.0.0.1:34567") };
	
	// http client to connect to nsv service
	web::http::client::http_client http_client{ nsv_service_address };

	// When request finishes, update the result section
	auto task = http_client.request(web::http::methods::GET, uri.to_string());
	try {
		task.wait();
	}
	catch (std::exception & e)
	{
		// Report the failure
		on_search_error(std::string{ e.what() });
		return;
	}
	web::http::http_response response = task.get();
		
	// Verify the request was successful
	if (response.status_code() == web::http::status_codes::OK) {
		// Extract the content as json
		// Send true flag to ignore content type for now, the service  always returns json when successful
		auto task = response.extract_json(true); 
		// Need to wait until the task finishes. Updating the UI from a new thread is not possible
		// by default. See: https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WApplication.html#ad9631ca64e68d30d40cb49c90e55223d
		task.wait();
		// Fills result table
		update_result(task.get());
	}
	else {
		// Report the failure
		on_search_http_error(response);
	}
}

void webapp_nsv_search::update_result(web::json::value & result)
{
	m_results->clear();

	// Add headers
	m_results->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Registration"));
	m_results->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Make"));
	m_results->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Model"));
	m_results->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Owner"));

	auto data = result[U("data")];
	unsigned row = 1; // header is row 0
	for (auto && vehicle : data.as_array())
	{
		auto reg = vehicle[U("Vehicle_Registration")].as_string();
		auto make = vehicle[U("Vehicle_Make")].as_string();
		auto model = vehicle[U("Vehicle_Model")].as_string();
		auto owner = vehicle[U("Vehicle_Owner")].as_string();
		
		m_results->elementAt(row, 0)->addWidget(std::make_unique<Wt::WText>(string::utf16_to_utf8(reg)));
		m_results->elementAt(row, 1)->addWidget(std::make_unique<Wt::WText>(string::utf16_to_utf8(make)));
		m_results->elementAt(row, 2)->addWidget(std::make_unique<Wt::WText>(string::utf16_to_utf8(model)));
		m_results->elementAt(row, 3)->addWidget(std::make_unique<Wt::WText>(string::utf16_to_utf8(owner)));
		++row;
	}
}

void webapp_nsv_search::layout()
{

}



} // End of namespace nvsdweb
} // End of namespace crossover


int main(int argc, char **argv)
{
	return Wt::WRun(argc, argv, [](const Wt::WEnvironment& env) {
		return std::make_unique<crossover::nvsdweb::webapp_nsv_search>(env);
	});
}