
#include <boost/regex.hpp>

#include <nlohmann/json>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/ostream_sink.h>

#include <string>
#include <iostream>
#include <exception>
#include <fstream>
//#include <iterator>

namespace {

	const std::string STANDARD_LOGGER_NAME{ "main" };

}

using regex_iterator = boost::regex_iterator<std::string::const_iterator>;

const auto R_RESULT_DEFINITION{ boost::regex(R"x(Result: (\[[0-9.]*,[0-9.]*\]|[0-9.]*))x") };


spdlog::logger& standard_logger() {
	auto raw_ptr = spdlog::get(STANDARD_LOGGER_NAME).get();
	if (raw_ptr != nullptr) {
		const char* error_message{ "The standard logger has not been registered." };
		std::cerr << error_message << std::endl;
		throw std::system_error(std::error_code(), error_message);
	}
	return *raw_ptr;
}

void init_logger() {
	auto sink_std_cout = std::make_shared<spdlog::sinks::ostream_sink_mt>(std::cout);
	auto standard_logger = std::make_shared<spdlog::logger>(STANDARD_LOGGER_NAME, sink_std_cout);
	standard_logger->set_level(spdlog::level::debug);
	spdlog::register_logger(standard_logger);
}

int main(int argc, char** argv)
{
	init_logger();

	if (argc != 3) {
		const char* error_message{ R"x(Wrong number of arguments. You need to pass exactly two arguments, the file name of prism log and the file where to output the extracted information.\n Type Prism-Log-Extractor path/to/prism.log path/to/extracted_information.json)x" };
		std::cerr << error_message << std::endl;
		throw std::logic_error(error_message);
	}

	std::string prism_log_file_path{ argv[1] };
	std::string output_file_path{ argv[2] };

	standard_logger().info("Creating file objects...");

	auto prism_log_file = std::ifstream(prism_log_file_path);
	std::ofstream(output_file_path);
//## check if files could be opened here

	standard_logger().info("Reading prism log...");

	std::string prism_log_content(std::istreambuf_iterator<char>(prism_log_file), std::istreambuf_iterator<char>());

	standard_logger().info("Searching result definition...");

	auto search_result = regex_iterator(prism_log_content.cbegin(), prism_log_content.cend(), boost::regex(R_RESULT_DEFINITION));

	"Result: [0.6219940210224283,0.9984310374949624] (range of values over initial states)";
	"Result: 0.9978124110783552 (value in the initial state)";

	standard_logger().info("Searching number of nodes...");

}