#pragma once

#include <Wt/WApplication.h>
#include <Wt/WBreak.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WTable.h>
#include <Wt\WBootstrapTheme.h>

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

#include <optional>


/// Default mongo db client pool size if not defined anywhere else
/// It is a small application, 50 sounds reasonable
#ifndef NVSDWEB_DBCLIENT_POOL_SIZE
#define NVSDWEB_DBCLIENT_POOL_SIZE 50
#endif


namespace crossover
{
namespace nvsdweb
{	

	class webapp_nsv_search : public Wt::WApplication
	{
	public:
		webapp_nsv_search(const Wt::WEnvironment& env);
		~webapp_nsv_search();

	private:
		Wt::WContainerWidget * m_form_container;
		Wt::WLineEdit *m_vehicle_registration_edit;
		Wt::WLineEdit *m_vehicle_make_edit;
		Wt::WLineEdit *m_vehicle_model_edit;
		Wt::WLineEdit *m_vehicle_owner_edit;
		Wt::WText *m_search_result;
		Wt::WTable * m_results;

		/// @brief theme
		std::shared_ptr<Wt::WBootstrapTheme> m_wt_theme;

		/// @brief Service address
		utility::string_t m_nsv_service_address;

		///  @brief Sets the layout of the widgets
		/// Modifies UI
		void layout();

		/// @brief handles search button click
		/// Fills result table
		/// Modifies UI
		void on_search_button_click();

		/// @brief Returns an error message if the input is bad
		std::optional<std::string> validate_input();

		/// @brief Creates URI from search input fields
		/// Assuming the input is valid.
		/// @returns Corresponding URI
		web::uri create_uri_from_input();

		/// @brief Handles search errors
		/// Used only when an unkown exception is catched
		/// Modifies UI
		void on_search_error(std::string & e);

		/// @brief Handles search http errors
		/// Modifies UI
		void on_search_http_error(web::http::http_response & response);

		/// @brief Updates the result table
		/// Modifies UI
		void update_result(web::json::value & result);


	};

	/// Encapsulate query building
	/// TODO:
	class uri_query_builder 
	{
	public:
	private:
	};
}
}

