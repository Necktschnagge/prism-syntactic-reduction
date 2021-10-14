
#include <boost/regex.hpp>

#include <nlohmann/json.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/ostream_sink.h>

#include <string>
#include <iostream>
#include <exception>
#include <fstream>
#include <cstdint>
//#define debug_local

namespace {

	const std::string STANDARD_LOGGER_NAME{ "main" };

}

using regex_iterator = boost::regex_iterator<std::string::const_iterator>;

const auto R_NUMBER_OF_STATES{ boost::regex(R"x(States:\s*([0-9]+)\s+\(([0-9]+)\s+initial\)\s*[\r\n]+?)x") };
const auto R_NUMBER_OF_TRANSITIONS{ boost::regex(R"x(Transitions:\s*([0-9]+)\s*[\r\n]+?)x") };
const auto R_RESULT_DEFINITION{ boost::regex(R"x(Result: (\[[0-9.]*,[0-9.]*\]|[0-9.]*))x") };
const auto R_RESULT_RANGE_DEFINITION{ boost::regex(R"x(Result: \[([0-9.]*),([0-9.]*)\])x") };
const auto R_RESULT_VALUE_DEFINITION{ boost::regex(R"x(Result: ([0-9.]*))x") };
const auto R_TRANSITION_MATRIX_INFORMATION{ boost::regex(R"x(Transition matrix:\s+([0-9]+)\s+nodes\s+\(([0-9]+)\s+terminal\),\s+([0-9]+)\s+minterms,\s+vars:[\w\W]*?[\r\n]+?)x") };
/* two formats are:
Result: [0.6219940210224283,0.9984310374949624] (range of values over initial states)
Result: 0.9978124110783552 (value in the initial state)
*/



spdlog::logger& standard_logger() {
	auto raw_ptr = spdlog::get(STANDARD_LOGGER_NAME).get();
	if (raw_ptr == nullptr) {
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

std::tuple<long double, long double> extract_result(const std::string& prism_log_content) {
	long double min{ 0 };
	long double max{ 0 };

	standard_logger().info("Searching result definition...");

	std::list<std::pair<std::string::const_iterator, std::string::const_iterator>> result_locations;

	auto search_result = regex_iterator(prism_log_content.cbegin(), prism_log_content.cend(), boost::regex(R_RESULT_DEFINITION));

	while (search_result != regex_iterator()) {
		result_locations.push_back(std::make_pair(search_result->prefix().end(), search_result->suffix().begin()));
		++search_result;
	}

	standard_logger().info(std::string("Found ") + std::to_string(result_locations.size()) + " result definitions.");
	if (result_locations.size() != 1) {
		auto error_message = std::string("Expected 1 but found ") + std::to_string(result_locations.size()) + " result definitions.";
		throw std::logic_error(error_message);
	}
	standard_logger().info("Reading values...");
	boost::match_results<std::string::const_iterator> m; // boost::smatch

	if (boost::regex_match(result_locations.front().first, result_locations.front().second, m, boost::regex(R_RESULT_VALUE_DEFINITION))) {
		standard_logger().info("Found single value definition.");
		max = min = std::stold(m[1]);
	}
	else if (boost::regex_match(result_locations.front().first, result_locations.front().second, m, boost::regex(R_RESULT_RANGE_DEFINITION))) {
		standard_logger().info("Found range definition.");
		min = std::stold(m[1]);
		max = std::stold(m[2]);
	}
	else {
		throw std::logic_error("Internal software error.");
	}
	standard_logger().info(std::string("Recognized min=") + std::to_string(min) + "  and  max=" + std::to_string(max));

	return std::make_tuple(min, max);
}

uint64_t extract_number_of_nodes(const std::string& prism_log_content) {
	uint64_t nodes;


	standard_logger().info("Searching transition matrix information...");

	std::list<std::pair<std::string::const_iterator, std::string::const_iterator>> result_locations;

	auto search_result = regex_iterator(prism_log_content.cbegin(), prism_log_content.cend(), boost::regex(R_TRANSITION_MATRIX_INFORMATION));

	while (search_result != regex_iterator()) {
		result_locations.push_back(std::make_pair(search_result->prefix().end(), search_result->suffix().begin()));
		++search_result;
	}

	standard_logger().info(std::string("Found ") + std::to_string(result_locations.size()) + " transition matrix information clauses.");
	if (0 < result_locations.size() && result_locations.size() <= 2) {
		auto error_message = std::string("Expected 1 or 2 but found ") + std::to_string(result_locations.size()) + " transition matrix information clauses.";
		throw std::logic_error(error_message);
	}
	standard_logger().info("Reading values...");
	boost::match_results<std::string::const_iterator> m; // boost::smatch

	if (boost::regex_match(result_locations.back().first, result_locations.back().second, m, boost::regex(R_TRANSITION_MATRIX_INFORMATION))) {
		nodes = std::stoull(m[1]);
	}
	else {
		throw std::logic_error("Internal software error.");
	}
	standard_logger().info(std::string("Recognized nodes=") + std::to_string(nodes));

	return nodes;
}

std::tuple<uint64_t, uint64_t> extract_number_of_states(const std::string& prism_log_content) {
	uint64_t states{ 0 };
	uint64_t initial{ 0 };


	standard_logger().info("Searching number of states...");

	std::list<std::pair<std::string::const_iterator, std::string::const_iterator>> result_locations;

	auto search_result = regex_iterator(prism_log_content.cbegin(), prism_log_content.cend(), boost::regex(R_NUMBER_OF_STATES));

	while (search_result != regex_iterator()) {
		result_locations.push_back(std::make_pair(search_result->prefix().end(), search_result->suffix().begin()));
		++search_result;
	}

	standard_logger().info(std::string("Found ") + std::to_string(result_locations.size()) + " state number clauses.");
	if (result_locations.size() != 1) {
		auto error_message = std::string("Expected 1 but found ") + std::to_string(result_locations.size()) + " state number clauses.";
		throw std::logic_error(error_message);
	}
	standard_logger().info("Reading values...");
	boost::match_results<std::string::const_iterator> m; // boost::smatch

	if (boost::regex_match(result_locations.front().first, result_locations.front().second, m, boost::regex(R_NUMBER_OF_STATES))) {
		states = std::stoull(m[1]);
		initial = std::stoull(m[2]);
	}
	else {
		throw std::logic_error("Internal software error.");
	}
	standard_logger().info(std::string("Recognized states=") + std::to_string(states) + "  and  initial=" + std::to_string(initial));

	return std::make_tuple(states, initial);
}


uint64_t extract_number_of_transitions(const std::string& prism_log_content) {
	uint64_t transitions{ 0 };


	standard_logger().info("Searching number of states...");

	std::list<std::pair<std::string::const_iterator, std::string::const_iterator>> result_locations;

	auto search_result = regex_iterator(prism_log_content.cbegin(), prism_log_content.cend(), boost::regex(R_NUMBER_OF_TRANSITIONS));

	while (search_result != regex_iterator()) {
		result_locations.push_back(std::make_pair(search_result->prefix().end(), search_result->suffix().begin()));
		++search_result;
	}

	standard_logger().info(std::string("Found ") + std::to_string(result_locations.size()) + " transition number clauses.");
	if (result_locations.size() != 1) {
		auto error_message = std::string("Expected 1 but found ") + std::to_string(result_locations.size()) + " transition number clauses.";
		throw std::logic_error(error_message);
	}
	standard_logger().info("Reading values...");
	boost::match_results<std::string::const_iterator> m; // boost::smatch

	if (boost::regex_match(result_locations.front().first, result_locations.front().second, m, boost::regex(R_NUMBER_OF_TRANSITIONS))) {
		transitions = std::stoull(m[1]);
	}
	else {
		throw std::logic_error("Internal software error.");
	}
	standard_logger().info(std::string("Recognized transitions=") + std::to_string(transitions));

	return transitions;
}

void prepare_files(int argc, char** argv, std::string& prism_log_content, std::ofstream& extracted_output_file) {
#ifdef debug_local
	std::string prism_log_file_path{ R"(C:\Users\F-NET-ADMIN\Desktop\some_prism_log.txt)" };
	std::string output_file_path{ R"(C:\Users\F-NET-ADMIN\Desktop\extracted.json)" };

#else
	if (argc != 3) {
		const char* error_message{ R"x(Wrong number of arguments. You need to pass exactly two arguments, the file name of prism log and the file where to output the extracted information.\n Type Prism-Log-Extractor path/to/prism.log path/to/extracted_information.json)x" };
		throw std::logic_error(error_message);
	}

	std::string prism_log_file_path{ argv[1] };
	std::string output_file_path{ argv[2] };
#endif

	standard_logger().info("Creating file objects...");

	auto prism_log_file = std::ifstream(prism_log_file_path);
	extracted_output_file = std::ofstream(output_file_path);
	//## check if files could be opened here

	standard_logger().info("Reading prism log...");

	prism_log_content = std::string(std::istreambuf_iterator<char>(prism_log_file), std::istreambuf_iterator<char>());
}

nlohmann::json analyze(const std::string& prism_log_content) {

	standard_logger().info("Searching model checking result...");
	auto [min, max] = extract_result(prism_log_content);
	standard_logger().info("Searching model checking result   ...DONE!");

	standard_logger().info("Searching number of nodes...");
	auto nodes = extract_number_of_nodes(prism_log_content);
	standard_logger().info("Searching number of nodes   ...DONE!");

	standard_logger().info("Searching number of states...");
	auto [count_states, initial_states] = extract_number_of_states(prism_log_content);
	standard_logger().info("Searching number of states   ...DONE!");

	standard_logger().info("Searching number of transitions...");
	auto count_transitions = extract_number_of_transitions(prism_log_content);
	standard_logger().info("Searching number of transitions   ...DONE!");


	standard_logger().info("Building json...");

	nlohmann::json result;
	result["result"] = { {"min", min}, {"max", max} };
	result["nodes"] = nodes;
	result["states"] = { {"count", count_states}, {"initial", initial_states} };
	result["count_transitions"] = count_transitions;

	standard_logger().info("Built up the following json:");
	std::cout << "\n\n" << result.dump(3) << "\n\n";

	return result;
}

int main(int argc, char** argv)
{
	init_logger();

	try {
		std::string prism_log_content;
		std::ofstream extracted_output_file;

		standard_logger().info("Preparing files...");

		prepare_files(argc, argv, prism_log_content, extracted_output_file);

		standard_logger().info("Extracting information...");

		nlohmann::json result{ analyze(prism_log_content) };

		standard_logger().info("Writing extracted json file...");

		extracted_output_file << result.dump(3);

	}
	catch (const std::system_error& e) {
		standard_logger().error(e.what());
		throw e;
	}
	catch (const std::exception& e) {
		standard_logger().error(e.what());
		throw e;
	}

	standard_logger().info("");
	standard_logger().info("Finished.");
}