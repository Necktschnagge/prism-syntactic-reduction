#include "logger.h"

#include "internal_error.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/ostream_sink.h"

#include <iostream>

static auto sink_std_cout = std::make_shared<spdlog::sinks::ostream_sink_mt>(std::cout);

namespace {

	const std::string STANDARD_LOGGER_NAME{ "main" };

}

spdlog::logger& standard_logger(){
	auto raw_ptr = spdlog::get(STANDARD_LOGGER_NAME).get();
	internal_error::assert_true(raw_ptr != nullptr, "The standard logger has not been registered.");
	return *raw_ptr;
}

void init_logger(){
	//auto sink_std_cout = std::make_shared<spdlog::sinks::ostream_sink_mt>(std::cout);
	auto standard_logger = std::make_shared<spdlog::logger>(STANDARD_LOGGER_NAME, sink_std_cout);
	standard_logger->set_level(spdlog::level::debug);
	spdlog::register_logger(standard_logger);
}


void init_my_logger(void* id, std::string name) {
	auto my_logger = std::make_shared<spdlog::logger>(std::to_string(reinterpret_cast<unsigned long long>(id)), sink_std_cout);
	my_logger->set_level(spdlog::level::debug);
	spdlog::register_logger(my_logger);
}

spdlog::logger& my_logger(void* id, std::string name) {
	auto raw_ptr = spdlog::get(std::to_string(reinterpret_cast<unsigned long long>(id))).get();
	if (raw_ptr == nullptr) init_my_logger(id, name);
	auto raw_ptr2 = spdlog::get(std::to_string(reinterpret_cast<unsigned long long>(id))).get();
	return *raw_ptr2;
}

