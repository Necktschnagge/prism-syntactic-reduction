
#include <boost/regex.hpp>

#include <nlohmann/json.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/ostream_sink.h>

#include <string>
#include <iostream>
#include <exception>
#include <fstream>

#include <cstdlib>
#include <filesystem>

//#define debug_local

namespace {

	const std::string STANDARD_LOGGER_NAME{ "main" };

}

using regex_iterator = boost::regex_iterator<std::string::const_iterator>;



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


int main(int argc, char** argv)
{
	init_logger();

	if (argc != 2) return 1;

	const auto run_directory_string = std::string(argv[0]);
	const auto arg_directory_string = std::string(argv[1]);
	const auto results_directory = std::filesystem::canonical(std::filesystem::path(run_directory_string).parent_path() / std::filesystem::path(arg_directory_string));
	
	standard_logger().info(std::string("Running on results directory:   ") + results_directory.string());

	auto meta_json_istream = std::ifstream(results_directory / "meta.json");

	nlohmann::json meta = nlohmann::json::parse(meta_json_istream);

	std::map<std::size_t, std::size_t> distribution_of_transitions;
	std::map<std::size_t, std::size_t> distribution_of_states;
	std::map<std::size_t, std::size_t> distribution_of_nodes;


	standard_logger().info("Reading result json files...");

	const std::size_t COUNT_MODELS{ meta["count_partitionings"] };
	for (std::size_t i = 0; i < COUNT_MODELS; ++i) {
		std::ifstream prism_data_istream = std::ifstream(results_directory / std::to_string(i) / "prism_data.json");
		auto prism_data = nlohmann::json::parse(prism_data_istream);
		const auto COUNT_TRANSITIONS{ prism_data["count_transitions"] };
		distribution_of_transitions.try_emplace(COUNT_TRANSITIONS, 0);
		++distribution_of_transitions[COUNT_TRANSITIONS];
		const auto COUNT_STATES{ prism_data["states"]["count"] };
		distribution_of_states.try_emplace(COUNT_STATES, 0);
		++distribution_of_states[COUNT_STATES];
		const auto COUNT_NODES{ prism_data["nodes"] };
		distribution_of_nodes.try_emplace(COUNT_NODES, 0);
		++distribution_of_nodes[COUNT_NODES];
	}

	{
		std::stringstream ss;
		double average{ 0 };
		ss << "Distribution of transitions :\n";
		for (const auto& pair : distribution_of_transitions) {
			ss << pair.first << "   :   " << pair.second;
			average += pair.first * pair.second;
		}
		standard_logger().info(ss.str());
		average /= meta["count_partitionings"];
		double variance{ 0 };
		for (const auto& pair : distribution_of_nodes) {
			ss << pair.first << "   :   " << pair.second;
			average += (static_cast<double>(pair.first) - average) * (static_cast<double>(pair.first) - average) * pair.second;
		}
		variance /= meta["count_partitionings"];
		standard_logger().info(std::string("average:  ") + std::to_string(average));
		standard_logger().info(std::string("variance:  ") + std::to_string(variance));	}
	{
		std::stringstream ss;
		double average{ 0 };
		ss << "Distribution of states :\n";
		for (const auto& pair : distribution_of_states) {
			ss << pair.first << "   :   " << pair.second;
			average += pair.first * pair.second;
		}
		standard_logger().info(ss.str());
		average /= meta["count_partitionings"];
		double variance{ 0 };
		for (const auto& pair : distribution_of_nodes) {
			ss << pair.first << "   :   " << pair.second;
			average += (static_cast<double>(pair.first) - average) * (static_cast<double>(pair.first) - average) * pair.second;
		}
		variance /= meta["count_partitionings"];
		standard_logger().info(std::string("average:  ") + std::to_string(average));
		standard_logger().info(std::string("variance:  ") + std::to_string(variance));	}
	{
		std::stringstream ss;
		double average{ 0 };
		ss << "Distribution of nodes :\n";
		for (const auto& pair : distribution_of_nodes) {
			ss << pair.first << "   :   " << pair.second;
			average += pair.first * pair.second;
		}
		standard_logger().info(ss.str());
		average /= meta["count_partitionings"];
		double variance{ 0 };
		for (const auto& pair : distribution_of_nodes) {
			ss << pair.first << "   :   " << pair.second;
			average += (static_cast<double>(pair.first) - average) * (static_cast<double>(pair.first) - average) * pair.second;
		}
		variance /= meta["count_partitionings"];
		standard_logger().info(std::string("average:  ") + std::to_string(average));
		standard_logger().info(std::string("variance:  ") + std::to_string(variance));
	}

#if false
	std::string original_model_path_string{ argv[1] };
	std::string syntactic_reducer_path_string{ argv[2] };
	std::string artifact_path_string{ argv[3] };

	auto original_model_path = std::filesystem::path(original_model_path_string);
	auto syntactic_reducer_path = std::filesystem::path(syntactic_reducer_path_string);
	auto artifact_path = std::filesystem::path(artifact_path_string) / "collect-data";
	auto copied_original_model_path = artifact_path / ORIGINAL_MODEL_FILE_NAME;

	//log_enumerator logs(artifact_path);

	/*
	copy original model into artifact path
	*/

	standard_logger().info("Creating directory...");
	system((std::string("mkdir ") + artifact_path.string()).c_str());
	standard_logger().info("Copying original model...");
	std::string command_copy_model = (std::string("cp ") + original_model_path.string() + " " + copied_original_model_path.string() /*+ logs.write_next()*/);
	system(command_copy_model.c_str());
	//logs.print_last_log();

	standard_logger().info("Check directory content...");
	system((std::string("ls -la ") + artifact_path.string() + logs.write_next()).c_str());
	logs.print_last_log();

	/*
	call synctactic reducer
	in : modle path, artifact output path
	out: json path containing all information about created files.
	*/
	std::string command_call_syntactic_reducer = syntactic_reducer_path.string() + " " + copied_original_model_path.string() + " " + artifact_path.string() + logs.write_next();
	standard_logger().info("Call Syntactic-Reducer...");
	standard_logger().info(command_call_syntactic_reducer);
	system(command_call_syntactic_reducer.c_str());
	logs.print_last_log();

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
#endif
	standard_logger().info("");
	standard_logger().info("Finished.");
}