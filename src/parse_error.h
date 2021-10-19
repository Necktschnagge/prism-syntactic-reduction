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

class not_matching : public parse_error {
	std::string _actual;
	std::string _expected_description;
	std::string _details;

	not_matching(const std::string& actual, const std::string& expected_description) :
		parse_error(std::string("Got:   ") + actual + "\nExpected:   " + expected_description),
		_actual(actual),
		_expected_description(expected_description)
	{
	}

	not_matching(std::string::const_iterator actual_begin, std::string::const_iterator actual_end, const std::string& expected_description, const std::string& details) :
		parse_error(std::string("Got:   ") + std::string(actual_begin, actual_end) + "\nExpected:   " + expected_description + "\n\nDetails:\n" + details),
		_actual(actual_begin, actual_end),
		_expected_description(expected_description),
		_details(details)
	{
	}

	not_matching(const std::string& message) :
		parse_error(message)
	{
	}
};
