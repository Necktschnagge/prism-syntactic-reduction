#pragma once

#include <string>
#include <exception>

class parse_error : public std::exception {
	std::string message;

public:
	static void assert_true(bool condition, const parse_error& exception) {
		if (!condition) throw exception;
	}

	parse_error(const std::string& message) : message(message) {}
	parse_error(const char* message) : parse_error(std::string(message)) {}
	parse_error(const parse_error&) = default;
	
	const char* what() const noexcept override {
		return message.c_str();
	}
};

class parse_primitive_error : public std::exception {
	std::string message;

public:
	static void assert_true(bool condition, const parse_primitive_error& exception) {
		if (!condition) throw exception;
	}

	parse_primitive_error(const std::string& message) : message(message) {}
	parse_primitive_error(const char* message) : parse_primitive_error(std::string(message)) {}
	parse_primitive_error(const parse_primitive_error&) = default;

	const char* what() const noexcept override {
		return message.c_str();
	}
};
