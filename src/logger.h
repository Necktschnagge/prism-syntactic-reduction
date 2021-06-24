#pragma once
#include "spdlog/logger.h"


spdlog::logger& standard_logger();

void init_logger();

spdlog::logger& my_logger(void* id, std::string name);