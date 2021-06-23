
#include <boost/regex.hpp>

#include <nlohmann/json.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/ostream_sink.h>

#include <string>
#include <iostream>
#include <exception>
#include <fstream>

#include <stdlib.h>
#include <filesystem>

//#define debug_local

namespace {

	const std::string STANDARD_LOGGER_NAME{ "main" };

}

using regex_iterator = boost::regex_iterator<std::string::const_iterator>;

const auto R_RESULT_DEFINITION{ boost::regex(R"x(Result: (\[[0-9.]*,[0-9.]*\]|[0-9.]*))x") };
const auto R_RESULT_RANGE_DEFINITION{ boost::regex(R"x(Result: \[([0-9.]*),([0-9.]*)\])x") };
const auto R_RESULT_VALUE_DEFINITION{ boost::regex(R"x(Result: ([0-9.]*))x") };
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
		auto error_message = std::string("Expected 1 but found ") + std::to_string(result_locations.size()) + " result definitions";
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

	auto [min, max] = extract_result(prism_log_content);

	standard_logger().info("Searching number of nodes...  SKIPPED");


	standard_logger().info("Building json...");

	nlohmann::json result;
	result["result"] = { {"min", min}, {"max", max} };

	standard_logger().info("Built up the following json:");
	std::cout << "\n\n" << result.dump(3) << "\n\n";

	return result;
}

class log_enumerator {
	unsigned long long i{ 0 };
	std::filesystem::path base_path;

	std::filesystem::path log_file_path(unsigned long long i) { return base_path / (std::to_string(i) + ".log"); };
public:
	log_enumerator(const std::filesystem::path& base_path) : base_path(base_path) {}

	inline std::string write_next() { return std::string(" > ") + log_file_path(++i).string(); }
	inline std::filesystem::path last() { return log_file_path(i); }
	inline void print_last_log() { 
		auto file = std::ifstream(last().string());
		bool ok = file.is_open();
		std::cout << "read file from path   " << last().string() << " here:\n\n" << ok ? std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()) : "error: file not open";
	}
	decltype(i) ii() { return i; }
};

int main(int argc, char** argv)
{
	init_logger();
	standard_logger().info("Listing arguments...");
	for (int i = 0; i < argc; ++i) std::cout << i << "   " << argv[i] << "\n";

	if (argc != 4) {
		standard_logger().error("Expected 1 application path and 3 arguments.");
		return 1;
	}

	const std::string ORIGINAL_MODEL_FILE_NAME{ "model_original.prism" };

	std::string original_model_path_string{ argv[1] };
	std::string syntactic_reducer_path_string{ argv[2] };
	std::string artifact_path_string{ argv[3] };

	auto original_model_path = std::filesystem::path(original_model_path_string);
	auto syntactic_reducer_path = std::filesystem::path(syntactic_reducer_path_string);
	auto artifact_path = std::filesystem::path(artifact_path_string) / "collect-data";

	log_enumerator logs(artifact_path);

	/*
	copy original model into artifact path
	*/

	standard_logger().info("Creating directory...");
	system((std::string("mkdir ") + artifact_path.string()).c_str());
	standard_logger().info("Copying original model...");
	std::string command_copy_model = (std::string("cp ") + original_model_path.string() + " " + (artifact_path / ORIGINAL_MODEL_FILE_NAME).string() + logs.write_next());
	std::cout << "#1...";
	system(command_copy_model.c_str());
	std::cout << "#2...";
	logs.print_last_log();
	std::cout << "#3...";
	std::cout << "number i : " << logs.ii();
	standard_logger().info("Check directory content...");
	system((std::string("ls ") + artifact_path.string() + logs.write_next()).c_str());
	logs.print_last_log();
	std::cout << "number i : " << logs.ii();

	/*
	call synctactic reducer
	in : modle path, artifact output path
	out: json path containing all information about created files.
	*/
	std::string command_call_syntactic_reducer = syntactic_reducer_path.string() + " ";


	/*
	call prism on all models
	*/

	/*
	call data extractor on all models
	*/

	/*
	collect data, output data
	*/



	/*
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

	}
	catch (const std::exception& e) {
		standard_logger().error(e.what());
		throw e;
	}
	*/
	standard_logger().info("");
	standard_logger().info("Finished.");
}